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
                        //TODO: add tests here...
                    }

                    std::filesystem::path m_Path;
                };
            }
        }
    }
}