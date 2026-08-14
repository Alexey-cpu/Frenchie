#pragma once

#include <FrenchieCoreSerizliation.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serizliation
        {
            namespace JSON
            {
                // Parser
                class Parser final
                {
                public:
                    static bool read_string(const ElementObj& _Object, const char* _Begin, const char* _End);
                };

                // PrettyWriter
                class PrettyWriter final
                {
                public:
                    static bool save_file(const ElementObj& _Object, const std::string& _Path);
                    static std::string write_string(const ElementObj& _Object);
                };

                // CompactWriter
                class CompactWriter final
                {
                public:
                    static bool save_file(const ElementObj& _Object, const std::string& _Path);
                    static std::string write_string(const ElementObj& _Object);
                };
            }
        }
    }
}