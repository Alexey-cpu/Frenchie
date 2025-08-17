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
                        std::cout << "this is document before modifying: \n";
                        std::cout << "------------------------------------------------------------------------------\n";

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

                        auto root    = document.append_node("Root");
                        auto scalars = root.append_node("Scalars");
                        auto vectors = root.append_node("Vectors");
                        auto lists   = root.append_node("Lists");
                        auto sets    = root.append_node("Sets");

                        #define __test_append_value__(__node, __type, __value) __node.append_node<__type>("Value", __value);
                        #define __test_append_vector__(__node, __type, __value) __node.append_node<std::vector<__type>>("Value", std::vector<__type>({__value, __value, __value}));
                        #define __test_append_list__(__node, __type, __value) __node.append_node<std::list<__type>>("Value", std::list<__type>({__value, __value, __value}));
                        #define __test_append_set__(__node, __type, __value) __node.append_node<std::set<__type>>("Value", std::set<__type>({__value, __value, __value}));

                        __test_append_value__(scalars, bool, false)
                        __test_append_value__(scalars, float, 1.5f)
                        __test_append_value__(scalars, double, 1.12313)
                        __test_append_value__(scalars, int, 1000)
                        __test_append_value__(scalars, unsigned int, 2000)
                        __test_append_value__(scalars, long, 2000)
                        __test_append_value__(scalars, unsigned long, 2000)
                        __test_append_value__(scalars, long long, 2000)
                        __test_append_value__(scalars, unsigned long long, 2000)

                        __test_append_vector__(vectors, bool, false)
                        __test_append_vector__(vectors, float, 1.5f)
                        __test_append_vector__(vectors, double, 1.12313)
                        __test_append_vector__(vectors, int, 1000)
                        __test_append_vector__(vectors, unsigned int, 2000)
                        __test_append_vector__(vectors, long, 2000)
                        __test_append_vector__(vectors, unsigned long, 2000)
                        __test_append_vector__(vectors, long long, 2000)
                        __test_append_vector__(vectors, unsigned long long, 2000)

                        __test_append_list__(lists, bool, false)
                        __test_append_list__(lists, float, 1.5f)
                        __test_append_list__(lists, double, 1.12313)
                        __test_append_list__(lists, int, 1000)
                        __test_append_list__(lists, unsigned int, 2000)
                        __test_append_list__(lists, long, 2000)
                        __test_append_list__(lists, unsigned long, 2000)
                        __test_append_list__(lists, long long, 2000)
                        __test_append_list__(lists, unsigned long long, 2000)

                        __test_append_set__(sets, bool, false)
                        __test_append_set__(sets, float, 1.5f)
                        __test_append_set__(sets, double, 1.12313)
                        __test_append_set__(sets, int, 1000)
                        __test_append_set__(sets, unsigned int, 2000)
                        __test_append_set__(sets, long, 2000)
                        __test_append_set__(sets, unsigned long, 2000)
                        __test_append_set__(sets, long long, 2000)
                        __test_append_set__(sets, unsigned long long, 2000)

                        #undef __append_value__
                        #undef __append_vector__
                        #undef __test_append_set__
                        #undef __test_append_list__

                        const int N    = 3;
                        const int M    = 3;
                        auto      tree = document.append_node("Tree");

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
                        //document.remove_node("Child_2");
                        //document.remove_node("Child_5");

                        // child_3.set_name("c_3");
                        // child_3.set_value("v_3");

                        // child_4.set_name("Child_4_SomeVeryLongLongLongLongLongLongLongName");
                        // child_4.set_value("Value_4_SomeVeryLongLongLongLongLongLongLongString");

                        // print_document(document);

                        // std::cout << "------------------------------------------------------------------------------\n";
                        // std::cout << "after next modifying: \n";
                        // std::cout << "------------------------------------------------------------------------------\n";

                        // child_3.set_value("Value_3_SomeVeryLongLongLongLongLongLongLongString");
                        // child_4.set_value("value_4");

                        // print_document(document);

                        // std::cout << "------------------------------------------------------------------------------\n";
                        // std::cout << "reset document: \n";
                        // std::cout << "------------------------------------------------------------------------------\n";
                        // document.reset();
                        // print_document(document);
                    }

                protected:
                    std::filesystem::path m_Path = std::filesystem::path();
                };
            }
        }
    }
}