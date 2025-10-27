#pragma once

#include <string>

#define STRINGIFY(_VALUE) #_VALUE

namespace Frenchie
{
    namespace Core
    {
        namespace String
        {
            // from string conversion
            template<typename __type> 
            __type from_string(const std::string& _Input);

            // to string conversion
            template <typename __type>
            std::string to_string(const __type& _Input);
        }
    }
}