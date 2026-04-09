#pragma once

#include <chrono>

namespace Frenchie
{
    namespace Core
    {
        namespace Clock
        {
            typedef std::chrono::high_resolution_clock::time_point HighResolutionClockTimePoint;

            inline HighResolutionClockTimePoint tic()
            {
                return std::chrono::high_resolution_clock::now();
            }

            template<typename Type = std::chrono::nanoseconds>
            double elapsed(const HighResolutionClockTimePoint& _Then, const HighResolutionClockTimePoint& _Now)
            {
                return (double)std::chrono::duration_cast<Type>(_Now - _Then).count();
            }
        }
    }
}
