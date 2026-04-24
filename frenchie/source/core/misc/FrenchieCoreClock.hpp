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

            typedef std::chrono::high_resolution_clock::time_point TimePoint;    ///< high resolution clock time point
            typedef std::chrono::nanoseconds                       Nanoseconds;  ///< high resolution clock duration in nanoseconds
            typedef std::chrono::microseconds                      Microseconds; ///< high resolution clock duration in microseconds
            typedef std::chrono::milliseconds                      Milliseconds; ///< high resolution clock duration in milliseconds
            typedef std::chrono::seconds                           Seconds;      ///< high resolution clock duration in seconds
            typedef std::chrono::minutes                           Minutres;     ///< high resolution clock duration in minutes
            typedef std::chrono::hours                             Hours;        ///< high resolution clock duration in hours

            /**
             * @brief Returns high resolution clock time point at this moment
             * @return Returns high resolution clock time point at this moment
             */
            inline TimePoint tic()
            {
                return std::chrono::high_resolution_clock::now();
            }

            /**
             * @brief Returns elapsed time between two time stamps
             * @param _Then previous time stamp
             * @param _Now  current time stamps
             * @return returns elapsed time between previous and current time stamp.
             */
            template<typename Type = Nanoseconds>
            double elapsed(const TimePoint& _Then, const TimePoint& _Now)
            {
                return (double)std::chrono::duration_cast<Type>(_Now - _Then).count();
            }

            /*! @} */
        }
    }
}
