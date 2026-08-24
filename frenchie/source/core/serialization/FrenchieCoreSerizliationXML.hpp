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
            namespace XML
            {
                /*! \defgroup <XML> (XML)
                *  @ingroup Serizliation
                *  @brief The module contains interfaces for XML parsing and writing for DOM tree model.
                * @{
                */

                /**
                 * @brief This XML parser interface class
                 * @class Parser
                 */
                class Parser final
                {
                public:

                    /**
                     * @brief This function reads XML DOM tree from string into input DOM tree _Object node
                     * @param _Object input DOM tree node into which we read XML string contents
                     * @param _Begin input XML string start pointer
                     * @param _End input XML string end pointer
                     * @returns true if XML string parsing succeeded 
                     */
                    static Document::Status read_string(const ElementObj& _Object, const char* _Begin, const char* _End);
                };

                /**
                 * @brief This XML pretty writer interface 
                 * @class PrettyWriter
                 * @details pretty writer interface provides a function for beautifull XML documents generation
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
                     * @brief This function writes subtree of the input DOM tree node into XML string
                     * @param _Object input DOM tree node
                     * @returns a string containing subtree of the input DOM tree node into XML string
                     */
                    static std::string write_string(const ElementObj& _Object);
                };

                /**
                 * @brief This XML compact writer interface 
                 * @class CompactWriter
                 * @details compact writer interface provides a function for compact XML documents generation
                 */
                class CompactWriter final
                {
                public:

                    /**
                     * @brief This function writes subtree of the input DOM tree node into XML file
                     * @param _Object input DOM tree node
                     * @param _Path file path
                     * @returns true if file write succeeded
                     */
                    static bool save_file(const ElementObj& _Object, const std::string& _Path);

                    /**
                     * @brief This function writes subtree of the input DOM tree node into XML string
                     * @param _Object input DOM tree node
                     * @returns a string containing subtree of the input DOM tree node into XML string
                     */
                    static std::string write_string(const ElementObj& _Object);
                };

                /*! @} */
            }
        }
    }
}