#pragma once

namespace Frenchie
{
    namespace Core
    {
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

            void reset()
            {
                if(m_HasValue && std::is_destructible<Type>::value) Value.~Type();
                m_HasValue = false;
            }

            bool has_value() const
            {
                return m_HasValue;
            }

            Type value() const
            {
                return Value;
            }
        };
    }
}