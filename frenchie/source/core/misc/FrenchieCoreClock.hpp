#pragma once

#include <chrono>

namespace Frenchie
{
    namespace Core
    {
        namespace Clock
        {
            /*! \defgroup <FrenchieCoreClock> (FrenchieCoreClock)
            *  @brief The module contains core utility functions and classes to work with high resolution clock.
                @{
            */

            typedef std::chrono::high_resolution_clock::time_point HighResolutionClockTimePoint;    ///< high resolution clock time point
            typedef std::chrono::nanoseconds                       HighResolutionClockNanoseconds;  ///< high resolution clock nanoseconds duration
            typedef std::chrono::microseconds                      HighResolutionClockMicroseconds; ///< high resolution clock microseconds duration
            typedef std::chrono::milliseconds                      HighResolutionClockMilliseconds; ///< high resolution clock milliseconds duration
            typedef std::chrono::seconds                           HighResolutionClockSeconds;      ///< high resolution clock seconds duration
            typedef std::chrono::minutes                           HighResolutionClockMinutres;     ///< high resolution clock minutes duration
            typedef std::chrono::hours                             HighResolutionClockHours;        ///< high resolution clock hours duration

            /**
             * @brief Returns high resolution clock time point at this moment
             * @return Returns high resolution clock time point at this moment
             */
            inline HighResolutionClockTimePoint tic()
            {
                return std::chrono::high_resolution_clock::now();
            }

            /**
             * @brief Returns elapsed time between two time stamps
             * @param [_Then] previous time stamp
             * @param [_Now ] current time stamps
             * @return returns elapsed time between two time stamps
             */
            template<typename Type = HighResolutionClockNanoseconds>
            double elapsed(const HighResolutionClockTimePoint& _Then, const HighResolutionClockTimePoint& _Now)
            {
                return (double)std::chrono::duration_cast<Type>(_Now - _Then).count();
            }

            /*! @} */
        }
    }
}
