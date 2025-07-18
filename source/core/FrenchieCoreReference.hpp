#pragma once

// STL
#include <memory>

namespace Frenchie
{
    namespace Core
    {
        template<typename T> 
        class Reference final
        {
        public:
            
            template<typename U = T> 
            Reference(const std::shared_ptr<U>& _Pointer = nullptr) : 
                m_Reference(std::dynamic_pointer_cast<T>(_Pointer)){}
            
            Reference(const std::nullptr_t& _NullPointer){}
            
            ~Reference(){}

            template<typename U = T>
            U* operator -> () const
            {
                return m_Reference.lock().get(); 
            }

            // ==
            template<typename U = T>
            bool operator==(const Reference<U>& _Pointer) const
            {
                auto pointer = std::dynamic_pointer_cast<T>(_Pointer.m_Reference.lock());

                return !m_Reference.expired() &&
                        m_Reference.lock().get() == pointer.get(); 
            }

            template<typename U = T>
            bool operator==(const std::shared_ptr<U>& _Pointer) const
            {
                auto pointer = std::dynamic_pointer_cast<T>(_Pointer);

                return !m_Reference.expired() &&
                        m_Reference.lock().get() == pointer.get(); 
            }

            bool operator==(const std::nullptr_t& _Pointer) const
            {
                return m_Reference.expired(); 
            }

            // !=
            template<typename U = T>
            bool operator!=(const Reference<U>& _Pointer) const
            {
                auto pointer = std::dynamic_pointer_cast<T>(_Pointer.m_Reference.lock());

                return m_Reference.expired() || 
                       m_Reference.lock().get() != pointer.get(); 
            }

            template<typename U = T>
            bool operator!=(const std::shared_ptr<U>& _Pointer) const
            {
                auto pointer = std::dynamic_pointer_cast<T>(_Pointer);

                return m_Reference.expired() || 
                       m_Reference.lock().get() != pointer.get(); 
            }

            bool operator!=(const std::nullptr_t& _Pointer) const
            {
                return !m_Reference.expired(); 
            }

            // ()
            template<typename U = T>
            operator std::shared_ptr<U>()
            {
                return std::dynamic_pointer_cast<U>(m_Reference.lock());
            }

            // <
            bool operator < (const Reference<T>& _Other) const
            {
                return m_Reference.lock().get() < _Other.m_Reference.lock().get();
            }

        protected:
            std::weak_ptr<T> m_Reference;
        };
    }
}