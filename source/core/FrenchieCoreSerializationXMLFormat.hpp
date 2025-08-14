#pragma once

#include <FrenchieCoreSerializationNode.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class XMLCompact
            {
            public:
                static bool read(Document* _Document, const std::filesystem::path& _Path);
                static bool write(Document* _Document, const std::filesystem::path& _Path);
            };

            class XMLBeautiful
            {
            public:
                static bool read(Document* _Document, const std::filesystem::path& _Path);
                static bool write(Document* _Document, const std::filesystem::path& _Path);
            };
        }
    }
}