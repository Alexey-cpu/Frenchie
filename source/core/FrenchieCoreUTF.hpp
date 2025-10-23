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

            // custom strings
            class UtfChar
            {
            public:
                UtfChar(const char32_t& _Char) : m_Char(_Char){}
                ~UtfChar(){}

                char32_t get() const
                {
                    return m_Char;
                }

                void set(const char32_t& _Char)
                {
                    m_Char = _Char;
                }

                std::string as_utf8() const
                {
                    return utf32_to_utf8(std::u32string(1, m_Char));
                }

                std::u16string as_utf6() const
                {
                    return utf8_to_utf16(as_utf8());
                }

            protected:
                utf8::utfchar32_t m_Char = '\0';
            };

            class String final
            {
            public:
                String(const std::string& _String) :
                    m_String(utf8_to_utf32(_String)){}

                ~String(){}

                std::u32string::const_iterator begin() const
                {
                    return m_String.begin();
                }
                
                std::u32string::const_iterator end() const
                {
                    return m_String.end();
                }

                size_t size() const
                {
                    return m_String.size();
                }

                utf8::utfchar32_t at(const size_t& _Index) const
                {
                    return m_String.at(_Index);
                }

            protected:
                std::u32string m_String;
            };
        }
    }
}