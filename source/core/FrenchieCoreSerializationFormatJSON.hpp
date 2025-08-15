#pragma once

#include <FrenchieCoreSerializationNode.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class JSONReader
            {
            public:
                static bool read(Document* _Document, const std::filesystem::path& _Path);
            };

            class JSONCompactWriter
            {
            public:
                static bool write(Document* _Document, const std::filesystem::path& _Path);
            };

            class JSONBeautifulWriter
            {
            public:
                static bool write(Document* _Document, const std::filesystem::path& _Path);
            };
        }
    }
}