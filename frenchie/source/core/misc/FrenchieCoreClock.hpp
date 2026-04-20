#pragma once

#include <chrono>

/*! \defgroup <Core> (Core)
 *  @brief The module contains core utility functions and classes.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Core
    {
        namespace Clock
        {
            /*! \defgroup <Clock> (Clock)
            *  @ingroup Core
            *  @brief The module contains core utility functions and classes to work with high resolution clock.
            *  @details The module contains core utility functions and classes to work with high resolution clock.
            * Actually this is a wrapper over std::chrono library.
            * @{
            */

            typedef std::chrono::high_resolution_clock::time_point HighResolutionClockTimePoint;    ///< high resolution clock time point
            typedef std::chrono::nanoseconds                       HighResolutionClockNanoseconds;  ///< high resolution clock duration in nanoseconds
            typedef std::chrono::microseconds                      HighResolutionClockMicroseconds; ///< high resolution clock duration in microseconds
            typedef std::chrono::milliseconds                      HighResolutionClockMilliseconds; ///< high resolution clock duration in milliseconds
            typedef std::chrono::seconds                           HighResolutionClockSeconds;      ///< high resolution clock duration in seconds
            typedef std::chrono::minutes                           HighResolutionClockMinutres;     ///< high resolution clock duration in minutes
            typedef std::chrono::hours                             HighResolutionClockHours;        ///< high resolution clock duration in hours

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
             * @return returns elapsed time between previous and current time stamp.
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
