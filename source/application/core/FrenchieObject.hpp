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
            
            Object(std::string _Name, Object* _Parent = nullptr)
            {
                set_parent(_Parent);
            }
            
            virtual ~Object()
            {
                set_parent(nullptr);
                remove_all_children();
            }

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

            std::list<Object*> get_children() const
            {
                return m_Children;
            }

            void set_parent(Object* _Parent)
            {
                if(m_Parent != nullptr) 
                    m_Parent->m_Children.erase(m_SelfIterator);
                m_Parent = _Parent;

                if(m_Parent == nullptr) 
                    return;
                
                m_Parent->m_Children.push_back(this);
                m_SelfIterator = std::prev(m_Parent->m_Children.end());
            }

            void remove_all_children()
            {
                auto children = get_children();

                for(auto& child : children) 
                {
                    if(child != nullptr) 
                        delete child;
                }

                m_Children.clear();
            }

            void apply_to_children_recursive(const std::function<void(Object* _Object)>& _Callback)
            {
                if(_Callback == nullptr) 
                    return;

                for(auto& child : m_Children) 
                {
                    _Callback(child);
                    child->apply_to_children_recursive(_Callback);
                }
            }

        protected:
            
            Object*                      m_Parent   = nullptr;
            std::list<Object*>           m_Children = std::list<Object*>();
            std::list<Object*>::iterator m_SelfIterator;
        };
    }

    template<typename __type, typename ... __arguments>
    std::shared_ptr<__type> Create(__arguments... _Parameters)
    {
        return std::make_shared<__type>(_Parameters ...);
    }
}