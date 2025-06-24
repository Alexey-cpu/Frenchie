#pragma once

// STL
#include <functional>
#include <memory>
#include <string>
#include <list>

namespace Frenchie
{
    namespace Core
    {
        class Object
        {
        public:
            
            Object(std::string _Name = std::string(), Object* _Parent = nullptr);
            virtual ~Object();

            template<typename T = Object> 
            T* get_parent() const
            {
                return dynamic_cast<T*>(m_Parent);
            }

            template<typename T> 
            T* get_parent_recursive() const
            {
                auto parent = get_parent();

                while (parent != nullptr)
                {
                    auto object = dynamic_cast<T*>(parent);

                    if(object != nullptr) 
                        return object;

                    parent = parent->get_parent();
                }
                
                return nullptr;
            }

            std::list<Object*> get_children() const;

            void set_parent(Object* _Parent);
            
            void remove_all_children();
            void apply_to_children_recursive(const std::function<void(Object* _Object)>& _Callback);

        protected:
            
            std::string                  m_Name         = std::string();
            Object*                      m_Parent       = nullptr;
            std::list<Object*>           m_Children     = std::list<Object*>();
            std::list<Object*>::iterator m_SelfIterator;
        };
    }
}