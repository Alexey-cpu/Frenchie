#include <FrenchieCoreSerializationTest.hpp>

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreSerizliationXML.hpp>
#include <FrenchieCoreSerizliationJSON.hpp>
#include <FrenchieCoreStringUtilities.hpp>

// STL
#include <algorithm>
#include <iostream>
#include <string>
#include <cctype>
#include <cstring>
#include <ctype.h>
#include <list>
#include <utility>

void display(const Frenchie::Core::Serizliation::ElementObj& _Element, const std::string& _Prefix = "", const bool& _ShowType = false)
{
    if(_ShowType)
    {
        std::cout
            << _Prefix << "{" << _Element.get_name()         << "}" << " : " << "{" << _Element.get_value()         << "}" << " --> "
                    << "{" << _Element.get_type_of_node() << "}" << " : " << "{" << _Element.get_type_of_value() << "}" << "\n";
    }
    else
    {
        std::cout << _Prefix << "{" << _Element.get_name() << "}" << " : " << "{" << _Element.get_value() << "}" << "\n";
    }
    
    for(const auto& child : _Element)
        display(child, _Prefix + "|---", _ShowType);
}

void Frenchie::Core::Tests::frenchie_core_serialization_dom_tree_test()
{
    printf("%s\n", GS_STRINGIFY(frenchie_core_serialization_dom_tree_test()));

    // getters test
    {
        std::cout << "dom tree node getters test..." << "\n";

        Frenchie::Core::Serizliation::Document    document;
        Frenchie::Core::Serizliation::ElementObj doc = document.get_root();

        auto root = doc.append_node("Root");

        root.append_node("Child1");
        root.append_node("Child2");
        root.append_node("Child3");
        root.append_node("Child4");
        root.append_node("Child5");

        GS_ASSERT(root.get_first().get_name() == "Child1");
        GS_ASSERT(root.get_last().get_name() == "Child5");
        GS_ASSERT(root.get_first().get_next().get_name() == "Child2");
        GS_ASSERT(root.get_first().get_next().get_prev().get_name() == "Child1");

        for(const auto& child : root)
            GS_ASSERT(child.get_parent() == root && child.get_parent().get_name() == root.get_name());
    }

    // setters test
    {
        std::cout << "setters test..." << "\n";

        Frenchie::Core::Serizliation::Document    document;
        Frenchie::Core::Serizliation::ElementObj doc = document.get_root();

        doc.set_name("Root");
        doc.set_value("RootValue");
        GS_ASSERT(doc.get_name() == "Root");
        GS_ASSERT(doc.get_value() == "RootValue");

        doc.set_name("AnotherRoot");
        doc.set_value("AnotherRootValue");
        GS_ASSERT(doc.get_name() == "AnotherRoot");
        GS_ASSERT(doc.get_value() == "AnotherRootValue");
    }

    // empty nodes, null handles, attributes, and string ownership
    {
        using namespace Frenchie::Core::Serizliation;

        Document document;
        ElementObj root = document.get_root();
        GS_ASSERT(root.is_not_null() && !root.is_null());
        GS_ASSERT(root.empty() && root.size() == 0);
        GS_ASSERT(root.get_first().is_null() && root.get_last().is_null());
        GS_ASSERT(root.get_next().is_null() && root.get_prev().is_null());
        GS_ASSERT(root.find_node([](const ElementObj&) { return true; }).is_null());
        GS_ASSERT(ElementObj().is_null() && ElementObj().get_attributes() == -1);
        GS_ASSERT(ElementObj().get_ref() == nullptr && ElementObj().get_document() == nullptr);
        GS_ASSERT(ElementObj().get_name().empty() && ElementObj().get_value().empty());
        GS_ASSERT(root.get_ref() != nullptr && root.get_document() == &document);

        const int attributes =
            ElementAttributes_ElementTypeAttribute |
            ElementAttributes_ElementValueTypeString;
        auto child = root.append_node("attribute", "initial", attributes);
        GS_ASSERT(child.is_not_null() && child.get_attributes() == attributes);
        GS_ASSERT(child.get_ref() != nullptr && child.get_document() == &document);
        GS_ASSERT(child.get_type_of_node() == "attribute");
        GS_ASSERT(child.get_type_of_value() == "string");
        child.set_attributes(
            ElementAttributes_ElementTypeObject |
            ElementAttributes_ElementValueTypeBoolean);
        GS_ASSERT(child.get_type_of_node() == "object");
        GS_ASSERT(child.get_type_of_value() == "bool");

        const std::pair<int, const char*> valueTypes[] =
        {
            {ElementAttributes_ElementValueTypeBoolean, "bool"},
            {ElementAttributes_ElementValueTypeFloat, "float"},
            {ElementAttributes_ElementValueTypeDouble, "double"},
            {ElementAttributes_ElementValueTypeLongDouble, "long double"},
            {ElementAttributes_ElementValueTypeInt8, "int8"},
            {ElementAttributes_ElementValueTypeInt16, "int16"},
            {ElementAttributes_ElementValueTypeInt32, "int32"},
            {ElementAttributes_ElementValueTypeInt64, "int64"},
            {ElementAttributes_ElementValueTypeUint8, "uint8"},
            {ElementAttributes_ElementValueTypeUint16, "uint16"},
            {ElementAttributes_ElementValueTypeUint32, "uint32"},
            {ElementAttributes_ElementValueTypeUint64, "uint64"},
            {ElementAttributes_ElementValueTypeString, "string"},
            {ElementAttributes_ElementValueTypeNullptr, "nullptr"},
            {ElementAttributes_ElementValueTypeCDATA, "cdata"},
            {ElementAttributes_ElementValueTypeProlog, "xml prolog"},
            {ElementAttributes_ElementValueTypeComment, "comment"}
        };
        for(const auto& [valueType, expectedName] : valueTypes)
        {
            child.set_attributes(ElementAttributes_ElementTypeCollection | valueType);
            GS_ASSERT(child.get_type_of_node() == "collection");
            GS_ASSERT(child.get_type_of_value() == expectedName);
        }

        {
            std::string temporaryName = "owned-name";
            std::string temporaryValue = "owned-value";
            child.set_name(temporaryName);
            child.set_value(temporaryValue);
            temporaryName.assign("changed-name");
            temporaryValue.assign("changed-value");
        }
        GS_ASSERT(child.get_name() == "owned-name");
        GS_ASSERT(child.get_value() == "owned-value");

        child.set_name(std::string());
        child.set_value(std::string());
        GS_ASSERT(child.get_name().empty() && child.get_value().empty());
    }

    // modificators test
    auto compare = [](
        const std::string&                              _Test,
        const std::list<std::string>&                   _Nodes,
        const Frenchie::Core::Serizliation::ElementObj& _Root)->bool
    {
        std::string str1;
        for(const auto& node : _Nodes)
            str1.append(node).append(" --> ");

        std::string str2;
        for(const auto& node : _Root)
            str2.append(node.get_name()).append(" --> ");

        std::cout << _Test << "\n";
        std::cout << str1 << "\n";
        std::cout << str2 << "\n\n";

        return str1 == str2;
    };

    // append test
    {
        Frenchie::Core::Serizliation::Document    document;
        Frenchie::Core::Serizliation::ElementObj root  = document.get_root();
        std::list<std::string>                   nodes = {"Child1", "Child2", "Child3"};
        for(const auto& node : nodes) root.append_node(node);
        for(int i = 0; i < 4; i++)nodes.push_back("Child4");
        for(int i = 0; i < 4; i++)root.append_node("Child4");
        GS_ASSERT(compare("append test", nodes, root));
    }

    // prepend test
    {
        Frenchie::Core::Serizliation::Document    document;
        Frenchie::Core::Serizliation::ElementObj root  = document.get_root();
        std::list<std::string>                   nodes = {"Child1", "Child2", "Child3"};
        for(const auto& node : nodes) root.append_node(node);
        for(int i = 0; i < 4; i++)nodes.push_front("Child4");
        for(int i = 0; i < 4; i++)root.prepend_node("Child4");
        GS_ASSERT(compare("prepend test", nodes, root));
    }

    // remove node test
    {
        Frenchie::Core::Serizliation::Document    document;
        Frenchie::Core::Serizliation::ElementObj root  = document.get_root();
        std::list<std::string>                   nodes = {"Child1", "Child1", "Child1"};
        for(const auto& node : nodes) root.append_node(node);

        for(int i = 0; i < 2; i++)nodes.erase(std::find(nodes.begin(), nodes.end(), "Child1"));
        for(int i = 0; i < 2; i++)root.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Node){return _Node.get_name() == "Child1";}).remove();
        GS_ASSERT(compare("remove node test", nodes, root));
    }

    // prepend before
    {
        std::cout << "prepend before test..." << "\n";

        Frenchie::Core::Serizliation::Document    document;
        Frenchie::Core::Serizliation::ElementObj root  = document.get_root();
        std::list<std::string>                   nodes = {"Child1", "Child2", "Child3"};
        for(const auto& node : nodes) root.append_node(node);

        {
            for(int i = 0; i < 3; i++)
            {
                auto name = Frenchie::Core::String::format("Child%d", i + i);

                nodes.insert(std::find(nodes.begin(), nodes.end(), "Child1"), name);
                root.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Node){return _Node.get_name() == "Child1";}).prepend_node_before(name);
                GS_ASSERT(compare(Frenchie::Core::String::format("iteration %d", i), nodes, root));
            }
        }
    }

    // append after
    {
        std::cout << "append after test..." << "\n";

        Frenchie::Core::Serizliation::Document    document;
        Frenchie::Core::Serizliation::ElementObj root  = document.get_root();
        std::list<std::string>                   nodes = {"Child1", "Child2", "Child3"};
        for(const auto& node : nodes) root.append_node(node);

        {
            for(int i = 0; i < 3; i++)
            {
                auto name = Frenchie::Core::String::format("Child%d", i + i);

                nodes.insert(++std::find(nodes.begin(), nodes.end(), "Child3"), name);
                root.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Node){return _Node.get_name() == "Child3";}).append_node_after(name);
                GS_ASSERT(compare(Frenchie::Core::String::format("iteration %d", i), nodes, root));
            }
        }
    }

    // list endpoints and links after detaching and moving nodes
    {
        using namespace Frenchie::Core::Serizliation;

        Document document;
        ElementObj root = document.get_root();
        auto first = root.append_node("first");
        auto middle = root.append_node("middle");
        auto last = root.append_node("last");

        auto beforeFirst = first.prepend_node_before("before-first");
        auto afterLast = last.append_node_after("after-last");
        GS_ASSERT(root.get_first() == beforeFirst);
        GS_ASSERT(root.get_last() == afterLast);
        GS_ASSERT(beforeFirst.get_next() == first && first.get_prev() == beforeFirst);
        GS_ASSERT(last.get_next() == afterLast && afterLast.get_prev() == last);

        middle.remove();
        GS_ASSERT(middle.get_parent().is_null());
        GS_ASSERT(first.get_next() == last && last.get_prev() == first);
        GS_ASSERT(root.size() == 4);

        auto secondParent = root.append_node("second-parent");
        GS_ASSERT(document.append_node(middle, secondParent));
        GS_ASSERT(middle.get_parent() == secondParent);
        GS_ASSERT(root.size() == 5 && secondParent.size() == 1);
        GS_ASSERT(secondParent.get_first() == middle && secondParent.get_last() == middle);

        GS_ASSERT(document.prepend_node(middle, root));
        GS_ASSERT(secondParent.empty());
        GS_ASSERT(root.get_first() == middle && middle.get_next() == beforeFirst);
        GS_ASSERT(middle.get_prev().is_null() && middle.get_parent() == root);

        middle.remove();
        GS_ASSERT(middle.get_next().is_null() && middle.get_prev().is_null());
        GS_ASSERT(middle.get_parent().is_null());

        beforeFirst.remove();
        afterLast.remove();
        GS_ASSERT(root.get_first() == first && root.get_last() == secondParent);
        GS_ASSERT(first.get_prev().is_null() && last.get_next() == secondParent);
        GS_ASSERT(root.size() == 3);
    }

    // invalid attachment and child lookup boundaries
    {
        using namespace Frenchie::Core::Serizliation;

        Document firstDocument;
        Document secondDocument;
        ElementObj firstRoot = firstDocument.get_root();
        ElementObj secondRoot = secondDocument.get_root();
        auto firstChild = firstRoot.append_node("first-child");
        auto foreignChild = secondRoot.append_node("foreign-child");
        auto detached = firstDocument.create_node("detached");
        auto secondDetached = firstDocument.create_node("second-detached");

        GS_ASSERT(firstRoot.find_node([](const ElementObj& node) { return node.get_name() == "first-child"; }) == firstChild);
        GS_ASSERT(firstRoot.find_node([](const ElementObj& node) { return node.get_name() == "missing"; }).is_null());
        GS_ASSERT(firstChild.find_node([](const ElementObj&) { return true; }).is_null());

        GS_ASSERT(!firstDocument.append_node(foreignChild, firstRoot));
        GS_ASSERT(firstDocument.prepend_node_before(detached, firstChild));
        GS_ASSERT(!firstDocument.append_node_after(secondDetached, firstRoot));
        GS_ASSERT(firstRoot.size() == 2 && firstRoot.get_first() == detached);
        GS_ASSERT(detached.get_next() == firstChild && firstChild.get_prev() == detached);
        GS_ASSERT(secondRoot.size() == 1 && secondRoot.get_first() == foreignChild);
        GS_ASSERT(secondDetached.get_parent().is_null());
    }

    // iterator and depth-first traversal behavior
    {
        using namespace Frenchie::Core::Serizliation;

        Document document;
        ElementObj root = document.get_root();
        auto first = root.append_node("first");
        auto branch = root.append_node("branch");
        auto leaf = branch.append_node("leaf");
        auto last = root.append_node("last");

        ElementItr iterator = root.begin();
        GS_ASSERT(*iterator == first);
        GS_ASSERT(iterator->get_name() == "first");
        ElementItr oldIterator = iterator++;
        GS_ASSERT(*oldIterator == first);
        GS_ASSERT(*iterator == branch);
        ElementItr oldBackward = iterator--;
        GS_ASSERT(*oldBackward == branch && *iterator == first);
        ++iterator;
        GS_ASSERT(*iterator == branch);
        ++iterator;
        GS_ASSERT(*iterator == last);
        ++iterator;
        GS_ASSERT(iterator == root.end());
        GS_ASSERT(root.size() == 3 && !root.empty());
        GS_ASSERT(branch.size() == 1 && !branch.empty());
        GS_ASSERT(leaf.empty());

        std::list<std::string> starts;
        std::list<std::string> ends;
        std::list<int> depths;
        root.traverse(
            [&](const ElementObj& node, int depth)
            {
                starts.emplace_back(node.get_name());
                depths.push_back(depth);
            },
            [&](const ElementObj& node, int)
            {
                ends.emplace_back(node.get_name());
            });
        GS_ASSERT((starts == std::list<std::string>{"first", "branch", "leaf", "last"}));
        GS_ASSERT((ends == std::list<std::string>{"first", "leaf", "branch", "last"}));
        GS_ASSERT((depths == std::list<int>{1, 1, 2, 1}));
    }

    // releasing the document allows its root to be recreated empty
    {
        using namespace Frenchie::Core::Serizliation;

        Document document;
        ElementObj root = document.get_root();
        root.set_name("root");
        root.append_node("child", "value");
        document.release();

        ElementObj recreatedRoot = document.get_root();
        GS_ASSERT(recreatedRoot.is_not_null());
        GS_ASSERT(recreatedRoot.get_name().empty() && recreatedRoot.empty());
    }

    printf("\n");
}

void Frenchie::Core::Tests::frenchie_core_serialization_xml_test()
{
    printf("%s\n", GS_STRINGIFY(frenchie_core_serialization_xml_test()));

    // tests
    auto xml_empty_file_parsing_test = []()
    {
        std::cout << GS_STRINGIFY(xml_empty_file_parsing_test) << "\n";
        char test0[] = R"()";
        Frenchie::Core::Serizliation::Document document;
        auto status = document.read_string<Frenchie::Core::Serizliation::XML::Parser>(test0, test0 + strlen(test0));
        std::cout << "status: " << status.m_Message << "\n";
        GS_ASSERT(!status);
        std::cout << "\n\n";
    };

    auto xml_comment_parsing_test = []()
    {
        std::cout << GS_STRINGIFY(xml_comment_parsing_test) << "\n";
        char test0[] = R"(<!-- <- SOME "COMMENT" {}[]() -> -->)";
        Frenchie::Core::Serizliation::Document document;
        auto status = document.read_string<Frenchie::Core::Serizliation::XML::Parser>(test0, test0 + strlen(test0));
        std::cout << "status: " << status.m_Message << "\n";
        std::cout << "source string:\n";
        std::cout << test0 << "\n\n";
        std::cout << "parsed string:\n";
        std::string parsedString = document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>();
        std::cout << parsedString << "\n";
        GS_ASSERT(parsedString == std::string(test0));
        std::cout << "\n\n";
    };

    auto xml_prolog_parsing_test = []()
    {
        std::cout << GS_STRINGIFY(xml_prolog_parsing_test) << "\n";
        char test0[] = R"(<? !!!HERE WE HAVE PROLOG NODE I KNOW IT'S NOT CORRECT!!! ?>)";
        Frenchie::Core::Serizliation::Document document;
        auto status = document.read_string<Frenchie::Core::Serizliation::XML::Parser>(test0, test0 + strlen(test0));
        std::cout << "status: " << status.m_Message << "\n";
        std::cout << "source string:\n";
        std::cout << test0 << "\n\n";
        std::cout << "parsed string:\n";
        std::string parsedString = document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>();
        std::cout << parsedString << "\n";
        GS_ASSERT(parsedString == std::string(test0));
        std::cout << "\n\n";
    };

    auto xml_default_node_test = []()
    {
        std::cout << GS_STRINGIFY(xml_default_node_test) << "\n";
        char test0[] = R"(<Root><Child><Sibling ATTRIBUTE="VALUE" ATTRIBUTE="VALUE" ATTRIBUTE="VALUE"/></Child></Root>)";
        Frenchie::Core::Serizliation::Document document;
        auto status = document.read_string<Frenchie::Core::Serizliation::XML::Parser>(test0, test0 + strlen(test0));
        std::cout << "status: " << status.m_Message << "\n";
        std::cout << "source string:\n";
        std::cout << test0 << "\n\n";
        std::cout << "parsed string:\n";
        std::string parsedString = document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>();
        std::cout << parsedString << "\n";
        GS_ASSERT(parsedString == std::string(test0));
        std::cout << "\n\n";
    };

    auto xml_invalid_comment_parsing_test = []()
    {
        std::cout << GS_STRINGIFY(xml_invalid_comment_parsing_test) << "\n";
        Frenchie::Core::Serizliation::Document document;

        std::vector<std::string> strings = 
        {
            R"(<! THIS INVALID COMMENT -->)",
            R"(<!- THIS INVALID COMMENT -->)",
            R"(<!-- THIS INVALID COMMENT ->)",
            R"(<!- THIS INVALID COMMENT >)",
            R"(<! THIS INVALID COMMENT >)",
            R"(<!THIS INVALID COMMENT-->)",
            R"(<!-THIS INVALID COMMENT-->)",
            R"(<!--THIS INVALID COMMENT->)",
            R"(<!-THIS INVALID COMMENT>)",
            R"(<!THIS INVALID COMMENT>)"
        };

        for(auto s : strings)
        {
            auto status = document.read_string<Frenchie::Core::Serizliation::XML::Parser>(s.data(), s.data() + s.size());

            std::cout << s << " : " << status.m_Message << "\t" << status.m_Status << "\n";
            GS_ASSERT(!status.m_Status);
        }
    };

    auto xml_invalid_prolog_parsing_test = []()
    {
        std::cout << GS_STRINGIFY(xml_invalid_prolog_parsing_test) << "\n";
        Frenchie::Core::Serizliation::Document document;

        std::vector<std::string> strings = 
        {
            R"(<?THIS IS INVALID PROLOG>)",
        };

        for(auto s : strings)
        {
            auto status = document.read_string<Frenchie::Core::Serizliation::XML::Parser>(s.data(), s.data() + s.size());

            std::cout << status.m_Message << "\n";
            GS_ASSERT(!status);
        }
    };

    auto xml_invalid_cdata_section_parsing_test = []()
    {
        std::cout << GS_STRINGIFY(xml_invalid_prolog_parsing_test) << "\n";
        Frenchie::Core::Serizliation::Document document;

        std::vector<std::string> strings = 
        {
            R"(<Node><!CDATA[]]></Node>)",
            R"(<Node><!DATA[]]></Node>)",
            R"(<Node><!ATA[]]></Node>)",
            R"(<Node><!TA[]]></Node>)",
            R"(<Node><!A[]]></Node>)",
            R"(<Node><![]]></Node>)",
            R"(<Node><!]]></Node>)",
            R"(<Node><!]></Node>)",
            R"(<Node><!></Node>)",
        };

        for(auto s : strings)
        {
            auto status = document.read_string<Frenchie::Core::Serizliation::XML::Parser>(s.data(), s.data() + s.size());

            std::cout << status.m_Message << "\n";
            GS_ASSERT(!status);
        }
    };

    auto xml_manual_document_building = []()
    {
        std::cout << GS_STRINGIFY(xml_manual_document_building) << "\n";

        Frenchie::Core::Serizliation::Document    document;
        Frenchie::Core::Serizliation::ElementObj root = document.get_root();

        root.append_node(
            "Prolog",
            "xml version=\"1.0\"",
            Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementTypeObject
            | Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeProlog);

        root.append_node(
            "Comment", "<!- The following code demonstrates how to use \"Frenchie\" micro framework ->",
            Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementTypeObject
            | Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeComment);

        Frenchie::Core::Serizliation::ElementObj main = root.append_node("Main", R"(
        #include <FrenchieImmediateUserInterfaceTestLayer.hpp>
        
        int main(int argc, char *argv[])
        {
            // escape unused variables
            (void)argc;
            (void)argv;

            // add test layer
            Frenchie::Application::Application::push_layer<Frenchie::Application::FrenchieImmediateUserInterfaceTestLayer>();

            // launch application
            return Frenchie::Application::Application::execute();
        })",
            Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementTypeObject
            | Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeCDATA);

        main.append_node("Language", "C++", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementTypeAttribute);
        main.append_node("CCompiler", "gcc", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementTypeAttribute);
        main.append_node("CppCompiler", "g++", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementTypeAttribute);
        main.append_node("BuildSystem", "CMake", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementTypeAttribute);

        auto tools = main.append_node("Tools");
        tools.append_node("Language", "C/C++");
        tools.append_node("CCompiler", "gcc");
        tools.append_node("CppCompiler", "g++");

        auto docWrittenString = document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>();
        document.read_string<Frenchie::Core::Serizliation::XML::Parser>(docWrittenString.data(), docWrittenString.data() + docWrittenString.size());
        auto docParsedString = document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>();

        std::cout << "docWrittenString:\n";
        std::cout << docWrittenString << "\n\n";
        std::cout << "docParsedString:\n";
        std::cout << docParsedString << "\n";

        GS_ASSERT(docWrittenString == docParsedString);
    };

    auto xml_document_parse_test = []()
    {
        std::cout << GS_STRINGIFY(xml_document_parse_test) << "\n";

        const char XML[] = R"(
<?xml version="1.0"?>
<!--<!- The following code demonstrates [123] [asdas] how to use "Frenchie" micro framework ->-->
<Main         
Language
=
	"C++"          CCompiler="gcc"              CppCompiler=
"g++"         BuildSystem=
"CMake"           
><![CDATA[
    #include <FrenchieImmediateUserInterfaceTestLayer.hpp>
    
    int main(int argc, char *argv[])
    {
        // escape unused variables
        (void)argc;
        (void)argv;

        // add test layer
        Frenchie::Application::Application::push_layer<Frenchie::Application::FrenchieImmediateUserInterfaceTestLayer>();

        // launch application
        return Frenchie::Application::Application::execute();
    }]]>
        <Tools>
                <Language>C/C++
                </Language>
                <CCompiler>gcc
                </CCompiler>
                <CppCompiler>g++
                </CppCompiler>
        </Tools>
</Main>
        )";

        Frenchie::Core::Serizliation::Document document;

        document.read_string<Frenchie::Core::Serizliation::XML::Parser>(XML, XML + strlen(XML));

        auto docWrittenString = document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>();
        document.read_string<Frenchie::Core::Serizliation::XML::Parser>(docWrittenString.data(), docWrittenString.data() + docWrittenString.size());
        auto docParsedString = document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>();

        GS_ASSERT(docWrittenString == docParsedString);

        std::cout << docParsedString << "\n";
    };

    auto xml_partial_document_read_test = []()
    {
        std::cout << GS_STRINGIFY(xml_partial_document_read_test) << "\n";

        Frenchie::Core::Serizliation::Document    fullDock;
        Frenchie::Core::Serizliation::ElementObj root = fullDock.get_root();

        auto child1 = root.append_node("Child-1", "SomeValue");
        root.append_node("Child-2", "SomeValue");
        root.append_node("Child-3", "SomeValue");

        const char XML[] = R"(
<Main         
Language
=
	"C++"          CCompiler="gcc"              CppCompiler=
"g++"         BuildSystem=
"CMake"           
><![CDATA[
    #include <FrenchieImmediateUserInterfaceTestLayer.hpp>
    
    int main(int argc, char *argv[])
    {
        // escape unused variables
        (void)argc;
        (void)argv;

        // add test layer
        Frenchie::Application::Application::push_layer<Frenchie::Application::FrenchieImmediateUserInterfaceTestLayer>();

        // launch application
        return Frenchie::Application::Application::execute();
    }]]>
        <Tools>
                <Language>C/C++
                </Language>
                <CCompiler>gcc
                </CCompiler>
                <CppCompiler>g++
                </CppCompiler>
        </Tools>
</Main>
        )";

        Frenchie::Core::Serizliation::Document partialDock;
        partialDock.read_string<Frenchie::Core::Serizliation::XML::Parser>(XML, XML + strlen(XML));

        fullDock.read_string<Frenchie::Core::Serizliation::XML::Parser>(XML, XML + strlen(XML), child1);
        
        GS_ASSERT(
            fullDock.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>(child1) ==
            partialDock.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>()
        );

        std::cout << fullDock.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>(child1) << "\n\n";
        
        std::cout << XML << "\n";
    };

    auto xml_writer_terminator_test = []()
    {
        Frenchie::Core::Serizliation::Document document;
        auto root = document.get_root();
        root.append_node(
            "Node",
            "before]]>after",
            Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementTypeObject |
                Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeCDATA);
        root.append_node(
            "Comment",
            "note--><Injected/>",
            Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementTypeObject |
                Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeComment);

        const std::string output = document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>();
        GS_ASSERT(output.find("<![CDATA[before]]]]><![CDATA[>after]]>") != std::string::npos);
        GS_ASSERT(output.find("<!--note- -><Injected/>-->") != std::string::npos);
    };

    auto xml_writer_entity_escaping_test = []()
    {
        Frenchie::Core::Serizliation::Document document;
        auto root = document.get_root();
        auto node = root.append_node("Node", "<>&'\"");
        node.append_node(
            "attribute",
            "<>&'\"",
            Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementTypeAttribute |
                Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeString);

        const std::string output = document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>();
        GS_ASSERT(output == "<Node attribute=\"&lt;&gt;&amp;&apos;&quot;\">&lt;&gt;&amp;&apos;&quot;</Node>");
    };

    auto xml_document_input_copy_test = []()
    {
        Frenchie::Core::Serizliation::Document document;
        std::string input = "<Root attribute=\"value\">content</Root>";
        const std::string expected = input;
        auto status = document.read_string<Frenchie::Core::Serizliation::XML::Parser>(
            input.data(), input.data() + input.size());
        GS_ASSERT(status);
        input.assign(input.size(), 'x');

        GS_ASSERT(document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>() == expected);
    };

    auto xml_parser_insitu_view_test = []()
    {
        Frenchie::Core::Serizliation::Document document;
        const std::string input = "<Root attribute=\"value\">content</Root>";
        auto status = Frenchie::Core::Serizliation::XML::Parser::read_string(
            document.get_root(), input.data(), input.data() + input.size());
        GS_ASSERT(status);

        auto node = document.get_root().get_first();
        auto attribute = node.get_first();
        GS_ASSERT(node.get_name().data() == input.data() + 1);
        GS_ASSERT(node.get_value().data() == input.data() + input.find("content"));
        GS_ASSERT(attribute.get_value().data() == input.data() + input.find("value"));
    };

    // run tests
    xml_empty_file_parsing_test();
    xml_comment_parsing_test();
    xml_prolog_parsing_test();
    xml_default_node_test();
    xml_invalid_comment_parsing_test();
    xml_invalid_prolog_parsing_test();
    xml_invalid_cdata_section_parsing_test();
    xml_manual_document_building();
    xml_document_parse_test();
    xml_partial_document_read_test();
    xml_writer_terminator_test();
    xml_writer_entity_escaping_test();
    xml_document_input_copy_test();
    xml_parser_insitu_view_test();
}

void Frenchie::Core::Tests::frenchie_core_serialization_json_test()
{
    printf("%s\n", GS_STRINGIFY(frenchie_core_serialization_json_test()));

    auto json_empty_file_parsing_test = []()
    {
        std::cout << GS_STRINGIFY(json_empty_file_parsing_test) << "\n";
        char file[] = R"()";
        Frenchie::Core::Serizliation::Document document;
        auto status = document.read_string<Frenchie::Core::Serizliation::JSON::Parser>(file, file + strlen(file));
        std::cout << "status: " << status.m_Message << "\n";
        GS_ASSERT(!status);
        std::cout << "\n\n";
    };

    auto json_key_value_pairs_pasing_test = []()
    {
        std::cout << GS_STRINGIFY(json_key_value_pairs_pasing_test) << "\n";
        char file[] = R"({"string":"value","float":0.5,"float":10.5e+12,"float":-10.5e-12,"bool":true,"bool":false,"null":null})";
        Frenchie::Core::Serizliation::Document document;
        auto status = document.read_string<Frenchie::Core::Serizliation::JSON::Parser>(file, file + strlen(file));
        std::cout << "status: " << status.m_Message << "\n";
        auto parsedString  = std::string(file);
        auto writtenString = document.write_string<Frenchie::Core::Serizliation::JSON::CompactWriter>();
        std::cout << "parsed string:\n" << std::string(file) << "\n\n";
        std::cout << "written string:\n" << writtenString << "\n";
        GS_ASSERT(status && writtenString == parsedString);
        std::cout << "\n\n";
    };

    auto json_objects_and_arrays_parsing_test = []()
    {
        std::cout << GS_STRINGIFY(json_objects_and_arrays_parsing_test) << "\n";
        char file[] = R"({"emptyObject":{},"emptyArray":[],"object":{"name":"object"},"array":[1,2,3,4]})";
        Frenchie::Core::Serizliation::Document document;
        auto status = document.read_string<Frenchie::Core::Serizliation::JSON::Parser>(file, file + strlen(file));
        std::cout << "status: " << status.m_Message << "\n";
        auto parsedString  = std::string(file);
        auto writtenString = document.write_string<Frenchie::Core::Serizliation::JSON::CompactWriter>();
        std::cout << "parsed string:\n" << std::string(file) << "\n\n";
        std::cout << "written string:\n" << writtenString << "\n";
        GS_ASSERT(status && writtenString == parsedString);
        std::cout << "\n\n";
    };

    auto json_invalid_file_parsing_test = []()
    {
        std::cout << GS_STRINGIFY(json_invalid_file_parsing_test) << "\n";
        Frenchie::Core::Serizliation::Document document;

        std::vector<std::string> strings = 
        {
            R"({"object":{})",
            R"("object":{}})",
            R"({"object":{})",
            R"({"object":}})",
            R"({"object:{}})",
            R"({object":{}})",
            R"({object:{}})",
            R"({"array":[})",
            R"({"array":]})",
            R"({"array:[]})",
            R"({array":[]})",
            R"({"array":[1,""3]})",
            R"({"object":{}"array":[]})",
            R"({"object":{},"array":[],})",
            R"({"object":{},"array":[123,123,]})",
            R"({"object":{},,,"array":[123,123]})",
        };

        for(auto s : strings)
        {
            auto status = document.read_string<Frenchie::Core::Serizliation::JSON::Parser>(s.data(), s.data() + s.size());
            std::cout << s << " : " << status.m_Message << "\t" << status.m_Status << "\n";
            if(status.m_Status) display(document.get_root());
            GS_ASSERT(!status.m_Status);
        }
    };

    auto json_writer_escaping_test = []()
    {
        Frenchie::Core::Serizliation::Document document;
        auto root = document.get_root().append_node();
        const std::string key = "key\", \"injected\":true, \"other";
        const std::string value =
            std::string("quote\" slash\\ newline\n") + char(1) + "&lt;&gt;&amp;&apos;&quot;";
        root.append_node(
            key,
            value,
            Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeString);

        const std::string output = document.write_string<Frenchie::Core::Serizliation::JSON::CompactWriter>();
        GS_ASSERT(output == "{\"key\\\", \\\"injected\\\":true, \\\"other\":\"quote\\\" slash\\\\ newline\\n\\u0001<>&'\\\"\"}");
    };

    auto json_document_input_copy_test = []()
    {
        Frenchie::Core::Serizliation::Document document;
        std::string input = "{\"long temporary key\":\"value that outlives its input buffer\"}";
        const std::string expected = input;
        auto status = document.read_string<Frenchie::Core::Serizliation::JSON::Parser>(
            input.data(), input.data() + input.size());
        GS_ASSERT(status);
        input.assign(input.size(), 'x');

        GS_ASSERT(document.write_string<Frenchie::Core::Serizliation::JSON::CompactWriter>() == expected);
    };

    auto json_parser_input_boundary_test = []()
    {
        Frenchie::Core::Serizliation::Document document;
        const char input[] = {'{', '"', 'k', '"', ':', '"', 'v', '"', '}'};
        auto status = Frenchie::Core::Serizliation::JSON::Parser::read_string(
            document.get_root(), input, input + sizeof(input));
        GS_ASSERT(status);
        GS_ASSERT(document.write_string<Frenchie::Core::Serizliation::JSON::CompactWriter>() == R"({"k":"v"})");
    };

    auto json_parser_unicode_and_escapes_test = []()
    {
        Frenchie::Core::Serizliation::Document document;
        const std::string utf8 = "caf\xc3\xa9 \xf0\x9f\x8c\x8d";
        const std::string input =
            std::string(R"({"unicode":")") + utf8 + R"(","escaped":"\/\b\f\n\r\t\u0061\"\\ "})";
        auto status = Frenchie::Core::Serizliation::JSON::Parser::read_string(
            document.get_root(), input.data(), input.data() + input.size());
        GS_ASSERT(status);

        auto unicode = document.get_root().get_first().get_first();
        auto escaped = unicode.get_next();
        GS_ASSERT(unicode.get_value() == utf8);
        GS_ASSERT(escaped.get_value() == R"(\/\b\f\n\r\t\u0061\"\\ )");
        GS_ASSERT(unicode.get_value().data() == input.data() + input.find(utf8));
        GS_ASSERT(escaped.get_value().data() == input.data() + input.find(R"(\/\b\f\n\r\t\u0061\"\\ )"));
    };

    auto json_parser_invalid_escape_test = []()
    {
        Frenchie::Core::Serizliation::Document document;
        const std::vector<std::string> inputs =
        {
            R"({"value":"bad\q"})",
            R"({"value":"trailing\"})",
            R"({"value":"bad unicode \u12G4"})",
            R"({"value":"short unicode \u123"})",
            std::string("{\"value\":\"raw") + char(1) + "\"}"
        };

        for(const auto& input : inputs)
        {
            auto status = Frenchie::Core::Serizliation::JSON::Parser::read_string(
                document.get_root(), input.data(), input.data() + input.size());
            GS_ASSERT(!status);
        }
    };

    auto json_manual_document_building_test = []()
    {
        printf("%s\n", GS_STRINGIFY(json_manual_document_building_test()));

        Frenchie::Core::Serizliation::Document    document;
        Frenchie::Core::Serizliation::ElementObj root = document.get_root().append_node(); // JSON always starts with an empty root element

        {
            auto object = root.append_node("Object");
            object.append_node("Name", "Child-1", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeString);
            object.append_node("ID1", "12354123", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeFloat);
            object.append_node("ID2", "12354123", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeDouble);
            object.append_node("ID3", "12354123", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeInt32);

            auto array = object.append_node("Array", "", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementTypeCollection);
            array.append_node("element-0", "1234", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeFloat);
            array.append_node("element-1", "1234", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeFloat);
            array.append_node("element-2", "1234", Frenchie::Core::Serizliation::ElementAttributes_::ElementAttributes_ElementValueTypeFloat);
        }

        auto writtenString = document.write_string<Frenchie::Core::Serizliation::JSON::CompactWriter>();
        GS_ASSERT(document.read_string<Frenchie::Core::Serizliation::JSON::Parser>(writtenString.data(), writtenString.data() + writtenString.size()));
        auto parsedString  = document.write_string<Frenchie::Core::Serizliation::JSON::CompactWriter>();

        std::cout << "writtenString \n";
        std::cout << writtenString << "\n\n";
        std::cout << "parsedString \n";
        std::cout << parsedString << "\n";

        GS_ASSERT(writtenString == parsedString);
    };

    auto json_document_parse_test = []()
    {
        printf("%s\n", GS_STRINGIFY(json_document_parse_test()));

        const char JSON[] = R"(
{
    "emptyObject":{},
    "emptyArray":[],
    "emptyPair":[],
    "objectWithNestedEmptyObjectsAndArrays":{
        "emptyObject":{},
        "emptyArray":[],
        "emptyPair":"",
        "objectWithEmptyArrayOfDeeplyNestedObjects":{
            "arrayOfDeeplyNestedObjects":[
                {
                    "emptyObject":{
                        "emptyObject":{
                            "emptyObject":{
                                "emptyObject":{
                                }
                            }
                        }
                    }
                },
                {
                    "emptyObject":{
                        "emptyObject":{
                            "emptyObject":{
                                "emptyObject":{
                                }
                            }
                        }
                    }
                },
                {
                    "emptyObject":{
                        "emptyObject":{
                            "emptyObject":{
                                "emptyObject":{
                                }
                            }
                        }
                    }
                }
            ]
        },
        "objectWithDeeplyNestedEmptyArrays":{
            "DeeplyNestedEmptyArrays":[
                [
                    [
                        [
                        ]
                    ]
                ]
            ]
        }
    },
    "objectWithDeeplyNestedArrays":{
        "DeeplyNestedArray":[
            [
                "Child-1",
                "Child-2",
                "Child-3",
                [
                    "Child-4",
                    "Child-5",
                    "Child-6",
                    [
                        "Child-7",
                        "Child-8",
                        "Child-9",
                        [
                            "Child-10",
                            "Child-11",
                            "Child-12"
                        ]
                    ]
                ]
            ]
        ]
    },
    "floatObject":123.123,
    "integerObject":123,
    "stringObject":"123.123",
    "nullObject":null,
    "trueObject":true,
    "falseObject":false
}
)";
        Frenchie::Core::Serizliation::Document document;
        GS_ASSERT(document.read_string<Frenchie::Core::Serizliation::JSON::Parser>(JSON, JSON + std::strlen(JSON)));

        std::string compactOutputJSON = document.write_string<Frenchie::Core::Serizliation::JSON::CompactWriter>();
        std::string compactInputJSON;

        for (size_t i = 0; i < std::strlen(JSON); i++)
        {
            if(!std::iscntrl(JSON[i]) && JSON[i] != ' ')
                compactInputJSON.push_back(JSON[i]);
        }

        std::cout << "initial compact doc string: \n";
        std::cout << compactInputJSON << "\n";

        std::cout << "written compact doc string: \n";
        std::cout << compactOutputJSON << "\n";

        GS_ASSERT((compactOutputJSON == compactInputJSON));
    };

    json_empty_file_parsing_test();
    json_key_value_pairs_pasing_test();
    json_objects_and_arrays_parsing_test();
    json_invalid_file_parsing_test();
    json_writer_escaping_test();
    json_document_input_copy_test();
    json_parser_input_boundary_test();
    json_parser_unicode_and_escapes_test();
    json_parser_invalid_escape_test();
    json_manual_document_building_test();
    json_document_parse_test();
}