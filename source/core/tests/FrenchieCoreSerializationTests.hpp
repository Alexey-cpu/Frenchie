#pragma once

#include <filesystem>

#include <FrenchieCoreSerializationXMLFormat.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            namespace Tests
            {
                class SerializationTests final
                {
                public:
                    SerializationTests(const std::filesystem::path& _Path) : m_Path(_Path){}
                    ~SerializationTests(){}

                    void run()
                    {
                        std::shared_ptr<Node> document = std::make_shared<Node>("NewDocument");
                        document->append_child("Child-1", 1);
                        document->append_child("Child-2", 2.5f);
                        document->append_child("Child-3", 1.5);

                        auto child = document.get();

                        for(int i = 4; i < 1e1; i++)
                        {
                            child = child->append_child(fmt::format("Child-{}", i), "Some info");
                        }

                        auto found = 
                            document->find_child([](Node* _Node)->bool{return  _Node->name() == "Child-1";});

                        if(found != nullptr)
                        {
                            std::cout << found->name() << "\t" << found->value().is_of_type<int>() << "\n";
                        }

                        found = document->find_child([](Node* _Node)->bool{return  _Node->name() == "Child-10";});

                        if(found != nullptr)
                        {
                            std::cout << found->name()  << "\t" << found->value().is_of_type<std::string>() << "\n";
                        }

                        Format<XML>::write(document.get(), std::filesystem::path(m_Path.string().append("/NewFile.xml")));

                        document = Format<XML>::read(std::filesystem::path(m_Path.string().append("/NewFile.xml")));

                        Format<XML>::write(document.get(), std::filesystem::path(m_Path.string().append("/NewFile1.xml")));
                    }

                    std::filesystem::path m_Path;
                };
            }
        }
    }
}