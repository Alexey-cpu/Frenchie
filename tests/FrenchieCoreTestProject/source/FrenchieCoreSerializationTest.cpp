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

        Frenchie::Core::Serizliation::DOMTree    document;
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

        Frenchie::Core::Serizliation::DOMTree    document;
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
        Frenchie::Core::Serizliation::DOMTree    document;
        Frenchie::Core::Serizliation::ElementObj root  = document.get_root();
        std::list<std::string>                   nodes = {"Child1", "Child2", "Child3"};
        for(const auto& node : nodes) root.append_node(node);
        for(int i = 0; i < 4; i++)nodes.push_back("Child4");
        for(int i = 0; i < 4; i++)root.append_node("Child4");
        GS_ASSERT(compare("append test", nodes, root));
    }

    // prepend test
    {
        Frenchie::Core::Serizliation::DOMTree    document;
        Frenchie::Core::Serizliation::ElementObj root  = document.get_root();
        std::list<std::string>                   nodes = {"Child1", "Child2", "Child3"};
        for(const auto& node : nodes) root.append_node(node);
        for(int i = 0; i < 4; i++)nodes.push_front("Child4");
        for(int i = 0; i < 4; i++)root.prepend_node("Child4");
        GS_ASSERT(compare("prepend test", nodes, root));
    }

    // remove node test
    {
        Frenchie::Core::Serizliation::DOMTree    document;
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

        Frenchie::Core::Serizliation::DOMTree    document;
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

        Frenchie::Core::Serizliation::DOMTree    document;
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

    printf("\n");
}

void Frenchie::Core::Tests::frenchie_core_serialization_xml_test()
{
    printf("%s\n", GS_STRINGIFY(frenchie_core_serialization_xml_test()));

    // manual document building
    {
        Frenchie::Core::Serizliation::DOMTree    document;
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

        GS_ASSERT(docWrittenString == docParsedString);
        std::cout << docParsedString << "\n";
    }

    // document parse test
    {
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

        Frenchie::Core::Serizliation::DOMTree document;

        document.read_string<Frenchie::Core::Serizliation::XML::Parser>(XML, XML + strlen(XML));

        auto docWrittenString = document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>();
        document.read_string<Frenchie::Core::Serizliation::XML::Parser>(docWrittenString.data(), docWrittenString.data() + docWrittenString.size());
        auto docParsedString = document.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>();

        GS_ASSERT(docWrittenString == docParsedString);

        std::cout << docParsedString << "\n";
    }

    // partial document read test
    {
        Frenchie::Core::Serizliation::DOMTree    fullDock;
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

        Frenchie::Core::Serizliation::DOMTree partialDock;
        partialDock.read_string<Frenchie::Core::Serizliation::XML::Parser>(XML, XML + strlen(XML));

        fullDock.read_string<Frenchie::Core::Serizliation::XML::Parser>(XML, XML + strlen(XML), child1);
        
        GS_ASSERT(
            fullDock.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>(child1) ==
            partialDock.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>()
        );

        std::cout << fullDock.write_string<Frenchie::Core::Serizliation::XML::CompactWriter>(child1) << "\n\n";
        
        std::cout << XML << "\n";
    }
}

void Frenchie::Core::Tests::frenchie_core_serialization_json_test()
{
    // manual document building test
    {
        Frenchie::Core::Serizliation::DOMTree    document;
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

        GS_ASSERT(writtenString == parsedString);
    }

    // partial document write
    {
    }

    // document parse test
    {
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
        Frenchie::Core::Serizliation::DOMTree document;
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
    }
}