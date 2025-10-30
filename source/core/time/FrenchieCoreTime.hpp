#pragma once

#include <chrono>

namespace Frenchie
{
    namespace Core
    {
        inline std::chrono::high_resolution_clock::time_point tic()
        {
            return std::chrono::high_resolution_clock::now();
        }

        template< typename __type = std::chrono::nanoseconds >
        double elapsed(std::chrono::high_resolution_clock::time_point _Then, std::chrono::high_resolution_clock::time_point _Now)
        {
            return (double)std::chrono::duration_cast<__type>(_Now - _Then).count();
        }
    }
}