#include <FrenchieCoreSerializationTest.hpp>

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreSerizliationXML.hpp>
#include <FrenchieCoreStringUtilities.hpp>

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

        for(auto& child : root)
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
        for(auto& node : _Nodes)
            str1.append(node).append(" --> ");

        std::string str2;
        for(auto& node : _Root)
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
        for(auto& node : nodes) root.append_node(node);
        for(int i = 0; i < 4; i++)nodes.push_back("Child4");
        for(int i = 0; i < 4; i++)root.append_node("Child4");
        GS_ASSERT(compare("append test", nodes, root));
    }

    // prepend test
    {
        Frenchie::Core::Serizliation::DOMTree    document;
        Frenchie::Core::Serizliation::ElementObj root  = document.get_root();
        std::list<std::string>                   nodes = {"Child1", "Child2", "Child3"};
        for(auto& node : nodes) root.append_node(node);
        for(int i = 0; i < 4; i++)nodes.push_front("Child4");
        for(int i = 0; i < 4; i++)root.prepend_node("Child4");
        GS_ASSERT(compare("prepend test", nodes, root));
    }

    // remove node test
    {
        Frenchie::Core::Serizliation::DOMTree    document;
        Frenchie::Core::Serizliation::ElementObj root  = document.get_root();
        std::list<std::string>                   nodes = {"Child1", "Child1", "Child1"};
        for(auto& node : nodes) root.append_node(node);

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
        for(auto& node : nodes) root.append_node(node);

        {
            for(int i = 0; i < 3; i++)
            {
                auto name = Frenchie::Core::String::format("Child%d", i + i);

                nodes.insert(std::find(nodes.begin(), nodes.end(), "Child1"), name);
                root.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Node){return _Node.get_name() == "Child1";}).prepend_before(name);
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
        for(auto& node : nodes) root.append_node(node);

        {
            for(int i = 0; i < 3; i++)
            {
                auto name = Frenchie::Core::String::format("Child%d", i + i);

                nodes.insert(++std::find(nodes.begin(), nodes.end(), "Child3"), name);
                root.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Node){return _Node.get_name() == "Child3";}).append_after(name);
                GS_ASSERT(compare(Frenchie::Core::String::format("iteration %d", i), nodes, root));
            }
        }
    }

    printf("\n");
}