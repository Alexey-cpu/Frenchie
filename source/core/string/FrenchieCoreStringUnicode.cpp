#include <FrenchieCoreStringUnicode.hpp>

using namespace Frenchie::Core::String;

#include <utf8.h>

std::string Frenchie::Core::String::convert_utf16_to_utf8(const std::u16string& _Input)
{
    std::string result;
    utf8::unchecked::utf16to8(_Input.begin(), _Input.end(), std::back_inserter(result));
    return result;
}

std::string Frenchie::Core::String::convert_utf32_to_utf8(const std::u32string& _Input)
{
    std::string result;
    utf8::unchecked::utf32to8(_Input.begin(), _Input.end(), std::back_inserter(result));
    return result;
}

std::u16string Frenchie::Core::String::convert_utf8_to_utf16(const std::string& _Input)
{
    std::u16string result;
    utf8::unchecked::utf8to16(_Input.begin(), _Input.end(), std::back_inserter(result));
    return result;
}

std::u32string Frenchie::Core::String::convert_utf8_to_utf32(const std::string& _Input)
{
    std::u32string result;
    utf8::unchecked::utf8to32(_Input.begin(), _Input.end(), std::back_inserter(result));
    return result;
}