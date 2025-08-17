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

                            std::cout << top.Padding <<  top.Node.get_name() << "\t" <<  top.Node.get_value() << "\t" <<  top.Node.get_type() << "\n";

                            for (auto&& child : top.Node) 
                                stack.push({child, top.Padding + "\t"});
                        }
                    }
                }

                class APITests
                {
                public:

                    APITests(const std::filesystem::path& _Path) : 
                        m_Path(_Path){}

                    void run()
                    {
                        std::cout << "------------------------------------------------------------------------------\n";
                        std::cout << "this is clean document: \n";
                        std::cout << "------------------------------------------------------------------------------\n";
                        
                        Document document;
                        print_document(document);

                        std::cout << "------------------------------------------------------------------------------\n";
                        std::cout << "before modifying: \n";
                        std::cout << "------------------------------------------------------------------------------\n";
                        const int N    = 3;
                        const int M    = 3;
                        auto      tree = document.append_node("Tree");

                        auto child_1 = document.append_node("Child_1");
                        auto child_2 = document.append_node("Child_2");
                        auto child_3 = document.append_node("Child_3");
                        auto child_4 = document.append_node("Child_4");
                        auto child_5 = document.append_node("Child_5");

                        child_1.set_value("Value_1");
                        child_2.set_value("Value_2");
                        child_3.set_value("Value_3");
                        child_4.set_value("Value_4");
                        child_5.set_value("Value_5");

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
                        document.remove_node("Child_1");
                        document.remove_node("Child_2");
                        document.remove_node("Child_5");

                        child_3.set_name("c_3");
                        child_3.set_value("v_3");

                        child_4.set_name("Child_4_SomeVeryLongLongLongLongLongLongLongName");
                        child_4.set_value("Value_4_SomeVeryLongLongLongLongLongLongLongString");

                        print_document(document);

                        std::cout << "------------------------------------------------------------------------------\n";
                        std::cout << "after next modifying: \n";
                        std::cout << "------------------------------------------------------------------------------\n";

                        child_3.set_value("Value_3_SomeVeryLongLongLongLongLongLongLongString");
                        child_4.set_value("value_4");

                        print_document(document);

                        std::cout << "------------------------------------------------------------------------------\n";
                        std::cout << "reset document: \n";
                        std::cout << "------------------------------------------------------------------------------\n";
                        document.reset();
                        print_document(document);
                    }

                protected:
                    std::filesystem::path m_Path = std::filesystem::path();
                };
            }
        }
    }
}