#pragma once

#include <FrenchieCoreSerizliation.hpp>

/*! \defgroup <Core> (Core)
 *  @brief The module contains core utility functions and classes.
    @{
*/

/*! @} */

/*! \defgroup <Serizliation> (Serizliation)
*  @ingroup Core
*  @brief The module contains core utility classes for serialization/deserialization.
* @{
*/

/*! @} */

namespace Frenchie
{
    namespace Core
    {
        namespace Serizliation
        {
            namespace JSON
            {
                /*! \defgroup <JSON> (JSON)
                *  @ingroup Serizliation
                *  @brief The module contains interfaces for JSON parsing and writing for DOM tree model.
                * @{
                */

                /**
                 * @brief This JSON parser interface class
                 * @class Parser
                 */
                class Parser final
                {
                public:

                    /**
                     * @brief This function reads JSON DOM tree from string into input DOM tree _Object node
                     * @param _Object input DOM tree node into which we read JSON string contents
                     * @param _Begin input JSON string start pointer
                     * @param _End input JSON string end pointer
                     * @returns true if JSON string parsing succeeded 
                     */
                    static bool read_string(const ElementObj& _Object, const char* _Begin, const char* _End);
                };

                /**
                 * @brief This JSON pretty writer interface 
                 * @class PrettyWriter
                 * @details pretty writer interface provides a function for beautifull JSON documents generation
                 */
                class PrettyWriter final
                {
                public:

                    /**
                     * @brief This function writes subtree of the input DOM tree node into JSON file
                     * @param _Object input DOM tree node
                     * @param _Path file path
                     * @returns true if file write succeeded
                     */
                    static bool save_file(const ElementObj& _Object, const std::string& _Path);

                    /**
                     * @brief This function writes subtree of the input DOM tree node into JSON string
                     * @param _Object input DOM tree node
                     * @returns a string containing subtree of the input DOM tree node into JSON string
                     */
                    static std::string write_string(const ElementObj& _Object);
                };

                /**
                 * @brief This JSON compact writer interface 
                 * @class CompactWriter
                 * @details compact writer interface provides a function for compact JSON documents generation
                 */
                class CompactWriter final
                {
                public:

                    /**
                     * @brief This function writes subtree of the input DOM tree node into JSON file
                     * @param _Object input DOM tree node
                     * @param _Path file path
                     * @returns true if file write succeeded
                     */
                    static bool save_file(const ElementObj& _Object, const std::string& _Path);

                    /**
                     * @brief This function writes subtree of the input DOM tree node into JSON string
                     * @param _Object input DOM tree node
                     * @returns a string containing subtree of the input DOM tree node into JSON string
                     */
                    static std::string write_string(const ElementObj& _Object);
                };

                /*! @} */
            }
        }
    }
}