#pragma once

#include <FrenchieCoreSerializationNode.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class XMLReader
            {
            public:
                static bool read(Document* _Document, const std::filesystem::path& _Path);
            };

            class XMLCompactWriter
            {
            public:
                static bool write(Document* _Document, const std::filesystem::path& _Path);
            };

            class XMLBeautifulWriter
            {
            public:
                static bool write(Document* _Document, const std::filesystem::path& _Path);
            };
        }
    }
}