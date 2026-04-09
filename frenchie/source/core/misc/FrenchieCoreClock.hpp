#pragma once

#include <chrono>

namespace Frenchie
{
    namespace Core
    {
        namespace Clock
        {
            typedef std::chrono::high_resolution_clock::time_point HighResolutionTimePoint;

            inline std::chrono::high_resolution_clock::time_point tic()
            {
                return std::chrono::high_resolution_clock::now();
            }

            template<typename Type = std::chrono::nanoseconds>
            double elapsed(std::chrono::high_resolution_clock::time_point _Then, std::chrono::high_resolution_clock::time_point _Now)
            {
                return (double)std::chrono::duration_cast<Type>(_Now - _Then).count();
            }
        }
    }
}
