#pragma once

#include <string>

namespace Frenchie
{
    namespace Core
    {
        namespace String
        {
            // to UTF-8
            std::string convert_utf16_to_utf8(const std::u16string&);
            std::string convert_utf32_to_utf8(const std::u32string&);

            // from UTF-8
            std::u16string convert_utf8_to_utf16(const std::string&);
            std::u32string convert_utf8_to_utf32(const std::string&);
        }
    }
}