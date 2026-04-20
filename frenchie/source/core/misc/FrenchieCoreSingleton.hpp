#pragma once

/*! \defgroup <Core> (Core)
 *  @brief The module contains core utility functions and classes.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Core
    {
        /*! \defgroup <Singleton> (Singleton)
        *  @ingroup Core
        *  @brief The module contains core utility template class that implementes singletone.
        * @{
        */

        /**
         * @brief implements a singletone of a given type
         * @class Singleton
         */
        template<typename T>
        class Singleton
        {
        public:

            /**
             * @brief returns a singletone instance
             * @return returns a singletone instance pointer
             */
            static T* instance()
            {
                static T enity;
                return &enity;
            }

            Singleton(const Singleton &) = delete;
            Singleton & operator = (const Singleton &) = delete;

        private:
            Singleton(){}
            ~Singleton(){}
        };

        /*! @} */
    }
}