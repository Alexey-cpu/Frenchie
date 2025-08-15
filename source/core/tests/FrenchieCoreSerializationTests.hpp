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

                            std::cout << top.Padding <<  top.Node.name() << "\t" <<  top.Node.value() << "\n";

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

                        document.append_node("Child_1");
                        document.append_node("Child_2");
                        document.append_node("Child_3");

                        std::cout << "------------------------------------------------------------------------------\n";
                        std::cout << "before removal: \n";
                        std::cout << "------------------------------------------------------------------------------\n";
                        auto next = tree;

                        for(int i = 0; i < N; i++)
                        {
                            for(int j = 0; j < M; j++) 
                                next = next.append_node(fmt::format("Node_{}_{}", i, j).c_str(), "");
                        }

                        print_document(document);

                        std::cout << "------------------------------------------------------------------------------\n";
                        std::cout << "after removal: \n";
                        std::cout << "------------------------------------------------------------------------------\n";
                        document.remove_node("Tree");
                        std::cout << "------------------------------------------------------------------------------\n";

                        //for(auto node : document.m_Nodes) std::cout << node->Name << "\n";
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