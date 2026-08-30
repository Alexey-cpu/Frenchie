#pragma once

// UTF-8
#include <utf8.h>

// STL
#include <string_view>
#include <charconv>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <set>

#define STRINGIFY(_VALUE) #_VALUE

/*! \defgroup <Core> (Core)
 *  @brief The module contains core utility functions and classes.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Core
    {
        namespace String
        {
            /*! \defgroup <String> (String)
            *  @ingroup Core
            *  @brief The module contains core utility functions to work with UTF strings.
            * @{
            */

            /**
             * @brief transforms input UTF-8 string to lower case
             * @param _Input input UTF-8 string 
             * @return UTF-8 lower case string
             */
            std::string    utf8_to_lower(const std::string& _Input);

            /**
             * @brief transforms input UTF-16 string to lower case
             * @param _Input input UTF-16 string 
             * @return UTF-16 lower case string
             */
            std::u16string utf16_to_lower(const std::u16string& _Input);

            /**
             * @brief transforms input UTF-32 string to lower case
             * @param _Input input UTF-32 string 
             * @return UTF-32 lower case string
             */
            std::u32string utf32_to_lower(const std::u32string& _Input);

            /**
             * @brief transforms input UTF-8 string to upper case
             * @param _Input input UTF-8 string 
             * @return UTF-8 upper case string
             */
            std::string    utf8_to_upper(const std::string& _Input);

            /**
             * @brief transforms input UTF-16 string to upper case
             * @param _Input input UTF-16 string 
             * @return UTF-16 upper case string
             */
            std::u16string utf16_to_upper(const std::u16string& _Input);

            /**
             * @brief transforms input UTF-32 string to upper case
             * @param _Input input UTF-32 string 
             * @return UTF-32 upper case string
             */
            std::u32string utf32_to_upper(const std::u32string& _Input);
         
            /**
             * @brief removes some symbols from UTF-8 string
             * @param _Input input UTF-8 string 
             * @param _Remove string with symbols that need to be removed
             * @return UTF-8 string without symbols to remove
             */
            std::string    utf8_remove_symbols(const std::string& _Input, const std::set<char>& _Remove);

            /**
             * @brief removes some symbols from UTF-16 string
             * @param _Input input UTF-16 string 
             * @param _Remove string with symbols that need to be removed
             * @return UTF-16 string without symbols to remove
             */
            std::u16string utf16_remove_symbols(const std::u16string& _Input, const std::set<char16_t>& _Remove);

            /**
             * @brief removes some symbols from UTF-32 string
             * @param _Input input UTF-32 string 
             * @param _Remove string with symbols that need to be removed
             * @return UTF-32 string without symbols to remove
             */
            std::u32string utf32_remove_symbols(const std::u32string& _Input, const std::set<char32_t>& _Remove);

            /**
             * @brief splits UTF-8 string onto several UTF-8 strings separated by a given delimiter
             * @param _Input input UTF-8 string 
             * @param _Delimeter splitting delimiter
             * @return vector of  UTF-8 strings
             */
            std::vector<std::string>    split_utf8_string (const std::string& _Input, const std::string& _Delimeter);

            /**
             * @brief splits UTF-16 string onto several UTF-16 strings separated by a given delimiter
             * @param _Input input UTF-8 string 
             * @param _Delimeter splitting delimiter
             * @return vector of  UTF-16 strings
             */
            std::vector<std::u16string> split_utf16_string(const std::u16string& _Input, const std::u16string& _Delimeter);

            /**
             * @brief splits UTF-32 string onto several UTF-32 strings separated by a given delimiter
             * @param _Input input UTF-32 string 
             * @param _Delimeter splitting delimiter
             * @return vector of  UTF-32 strings
             */
            std::vector<std::u32string> split_utf32_string(const std::u32string& _Input, const std::u32string& _Delimeter);

            /**
             * @brief checks if an input UTF-8 string contains some substring
             * @param _String input UTF-8 string 
             * @param _Substring input UTF-8 substring 
             * @return returns true if a _Substring is within _String
             */
            bool utf8_string_contains_substring(const std::string& _String, const std::string& _Substring);

            /**
             * @brief checks if an input UTF-16 string contains some substring
             * @param _String input UTF-16 string 
             * @param _Substring input UTF-16 substring 
             * @return returns true if a _Substring is within _String
             */
            bool utf16_string_contains_substring(const std::u16string& _String, const std::u16string& _Substring);

            /**
             * @brief checks if an input UTF-32 string contains some substring
             * @param _String input UTF-32 string 
             * @param _Substring input UTF-32 substring 
             * @return returns true if a _Substring is within _String
             */
            bool utf32_string_contains_substring(const std::u32string& _String, const std::u32string& _Substring);

            /**
             * @brief converts input string to a value of other type
             * @param _Input input string 
             * @return returns a value parsed out-of input string
             */
            template<typename Type> 
            Type from_string(const std::string_view& _Input);

            /**
             * @brief converts input value to a string
             * @param _Input input value 
             * @return writes an input value into a string and returns this string
             */
            template <typename Type>
            std::string to_string(const Type& _Input);

            /**
             * @brief converts input UTF-16 string to UTF-8 string
             * @return returns UTF-8 string
             */
            std::string convert_utf16_to_utf8(const std::u16string&);

            /**
             * @brief converts input UTF-32 string to UTF-8 string
             * @return returns UTF-32 string
             */
            std::string convert_utf32_to_utf8(const std::u32string&);

            /**
             * @brief converts input UTF-8 string to UTF-16 string
             * @return returns UTF-16 string
             */
            std::u16string convert_utf8_to_utf16(const std::string&);

            /**
             * @brief converts input UTF-8 string to UTF-32 string
             * @return returns UTF-32 string
             */
            std::u32string convert_utf8_to_utf32(const std::string&);

            /**
             * @brief increments UTF-8 string iterator and reads next Unicode point
             * @param _Iterator input iterator 
             * @return next Unicode point
             */
            template<typename octet_iterator>
            unsigned int utf8_next(octet_iterator& _Iterator)
            {
                return utf8::unchecked::next(_Iterator);
            }

            /**
             * @brief decrements UTF-8 string iterator and reads previous Unicode point
             * @param _Iterator input iterator 
             * @return previous Unicode point
             */
            template<typename octet_iterator>
            unsigned int utf8_prior(octet_iterator& _Iterator)
            {
                return utf8::unchecked::prior(_Iterator);
            }

            /**
             * @brief outputs a formatted string
             * @param _Format input format string 
             * @param _Args input format string arguments 
             * @return returns a formatted string
             */
            template<typename ... Args>
            std::string format(const std::string& _Format, Args... _Args)
            {
                // identify size
                int size = snprintf(nullptr, 0, _Format.c_str(), _Args ...);
                
                if(size <= 0)
                    return _Format;

                // allocate buffer
                std::string buffer;
                buffer.resize(size);

                // print result
                snprintf(buffer.data(), size + 1, _Format.c_str(), _Args ...);
                return buffer;
            }

            /*! @} */
        }
    }
}