#pragma once

#include <filesystem>

#include <FrenchieCoreSerializationFormatXML.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            namespace Tests
            {
                void print_document(const Document& _Document)
                {
                    struct Element
                    {
                        Node        Node;
                        std::string Padding;
                    };

                    std::cout << "Document \n";
                    std::cout << "Free memory " << _Document.get_free_memory_amount() << " Bytes \n";

                    for(auto&& child : _Document)
                    {
                        // parse in depth
                        Helpers::Stack<Element> stack;
                        stack.push({child, ""});

                        while(!stack.empty())
                        {
                            auto top = stack.top();
                            stack.pop();

                            std::cout << top.Padding <<  top.Node.get_name() << "\t" <<  top.Node.get_value() << "\n";

                            for (auto&& child : top.Node) 
                                stack.push({child, top.Padding + "\t"});
                        }
                    }
                }

                class APITests
                {
                public:
                    void run()
                    {
                        Document document;

                        const int N    = 3;
                        const int M    = 3;
                        auto      tree = document.append_node("Tree");

                        auto child_1 = document.append_node("Child_1");
                        auto child_2 = document.append_node("Child_2");
                        auto child_3 = document.append_node("Child_3");

                        child_1.set_value("Value_1");
                        child_2.set_value("Value_2");
                        child_3.set_value("Value_3");

                        std::cout << "------------------------------------------------------------------------------\n";
                        std::cout << "before modifying: \n";
                        std::cout << "------------------------------------------------------------------------------\n";
                        auto next = tree;

                        for(int i = 0; i < N; i++)
                        {   
                            next = tree;

                            for(int j = 0; j < M; j++) 
                                next = next.append_node(fmt::format("Node_{}_{}", i, j).c_str(), "");
                        }

                        print_document(document);

                        std::cout << "------------------------------------------------------------------------------\n";
                        std::cout << "after modifying: \n";
                        std::cout << "------------------------------------------------------------------------------\n";
                        document.remove_node("Tree");

                        child_1.set_value("v_1");
                        child_2.set_value("v_2");
                        child_3.set_value("v_3");

                        child_1.set_name("c_1");
                        child_2.set_name("c_2");
                        child_3.set_name("c_3");

                        print_document(document);
                    }

                    void run1()
                    {
                        Document document;

                        auto tree = document.append_node("Tree");
                        
                        auto child_1 = tree.append_node("Child_1");
                        child_1.append_node("Child_1_1");

                        auto child_2 = tree.append_node("Child_2");
                        child_2.append_node("Child_2_1");

                        auto child_3 = tree.append_node("Child_3");
                        child_3.append_node("Child_3_1");

                        auto print_children = [](const Node& _Node)
                        {
                            std::cout << "---------------------------------------------------------------\n";
                            std::cout << _Node.get_name() << " children: " << "\n";
                            std::cout << "---------------------------------------------------------------\n";

                            for(auto&& child : _Node)  std::cout << child.get_name() << "\n";

                            // auto head = _Node.m_Info->FirstChild;

                            // while(head)
                            // {
                            //     std::cout << head->Name << "\n";

                            //     head = head->NextSibling;
                            // }
                        };
                        
                        // print_children(tree);
                        // print_children(child_1);
                        // print_children(child_2);
                        // print_children(child_3);

                        //print_children(document);

                        print_document(document);
                    }
                };

                class SerializationTests final
                {
                public:
                    SerializationTests(const std::filesystem::path& _Path) : m_Path(_Path){}
                    ~SerializationTests(){}

                    void run()
                    {
                        //TODO: add tests here...
                    }

                    std::filesystem::path m_Path;
                };
            }
        }
    }
}