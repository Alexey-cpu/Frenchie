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
        /*! \defgroup <Optional> (Optional)
        *  @ingroup Core
        *  @brief The module contains core utility class that provides optional values storage.
        * @{
        */

        /**
         * @class Optional
         * @brief This is an analog class for C++ std::optional.
         */
        template <class Type>
        class Optional final
        {
        public:
            union 
            {
                char PlaceHolder;
                Type Value;
            };
            bool m_HasValue{false};   // Flag indicating if a value is present

            constexpr Optional(const Optional<Type>& _Other) : Value(_Other.Value), m_HasValue(_Other.m_HasValue){}
            constexpr Optional() noexcept : PlaceHolder(0), m_HasValue(false) {}
            constexpr Optional(const Type& _Value) : Value(_Value), m_HasValue(true) {}

            Optional<Type>& operator=(const Optional<Type>& _Other)
            {
                Value      = _Other.Value;
                m_HasValue = _Other.m_HasValue;
                return *this;
            }

            ~Optional() 
            {
                if(m_HasValue && std::is_destructible<Type>::value) Value.~Type();
            }

            /**
             * @brief resets stored optional value
             */
            void reset()
            {
                if(m_HasValue && std::is_destructible<Type>::value) Value.~Type();
                m_HasValue = false;
            }

            /**
             * @brief checks if any optional value is stored
             */
            bool has_value() const
            {
                return m_HasValue;
            }

            /**
             * @brief returns stored optional value
             */
            Type value() const
            {
                GS_ASSERT(m_HasValue);
                return Value;
            }
        };

        /*! @} */
    }
}