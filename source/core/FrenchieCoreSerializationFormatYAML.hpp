#pragma once

#include <FrenchieCoreSerializationNode.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class YAMLReader
            {
            public:
                static bool read(Document* _Document, const std::filesystem::path& _Path);
            };

            class YAMLCompactWriter
            {
            public:
                static bool write(Document* _Document, const std::filesystem::path& _Path);
            };

            class YAMLBeautifulWriter
            {
            public:
                static bool write(Document* _Document, const std::filesystem::path& _Path);
            };
        }
    }
}