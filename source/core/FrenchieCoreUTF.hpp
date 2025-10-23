#pragma once

#include <utf8.h>

namespace Frenchie
{
    namespace Core
    {
        namespace UTF
        {
            void append_utf32_to_utf8(utf8::utfchar32_t, std::string&);
            void append_utf32_to_utf16(utf8::utfchar32_t, std::u16string&);

            std::string utf16_to_utf8(const std::u16string&);
            std::string utf32_to_utf8(const std::u32string&);

            std::u16string utf8_to_utf16(const std::string&);
            std::u32string utf8_to_utf32(const std::string&);
        }
    }
}