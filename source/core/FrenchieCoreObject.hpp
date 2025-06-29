#pragma once

// STL
#include <functional>
#include <memory>
#include <string>
#include <list>
#include <set>

namespace Frenchie
{
    namespace Core
    {
        class Root;
        class Object;

        class Root final
        {
        public:
            Root();
            ~Root();

            void push(Object* _Object);
            void pop(Object* _Object);
            bool is_being_restroyed() const;

        private:

            bool              m_IsBeingDestroed   = false;
            int               m_ReferenceCounter  = 0;
            std::set<Object*> m_Objects           = std::set<Object*>();

            friend class Object;
        };

        class Object
        {
        public:
            
            Object(const std::string& _Name = std::string(), Object* _Parent = nullptr);
            virtual ~Object();

            // getters
            template<typename T = Object> 
            T* get_parent() const
            {
                return dynamic_cast<T*>(m_Parent);
            }

            template<typename T> 
            T* get_parent_recursive() const
            {                
                return dynamic_cast<T*>(
                    get_parent_recursive(
                        [](Object* _Object)->bool
                        {
                            return dynamic_cast<T*>(_Object) != nullptr;}
                        )
                    );
            }

            Object* get_parent_recursive(const std::function<bool(Object*)>& _Predicate) const
            {
                if(_Predicate == nullptr) 
                    return nullptr;

                auto parent = get_parent();

                while (parent != nullptr)
                {
                    if(_Predicate(parent)) 
                        return parent;

                    parent = parent->get_parent();
                }
                
                return nullptr;    
            }

            std::string get_name() const
            {
                return m_Name;
            }

            std::list<Object*> get_children() const;

            bool is_selected() const
            {
                return (bool)(m_Flags | Flags::Selected);
            }

            bool is_focused() const
            {
                return (bool)(m_Flags | Flags::Focused);
            }

            // setters
            void set_name(const std::string& _Value)
            {
                m_Name = _Value;
            }

            void set_parent(Object* _Parent);

                        void set_selected(bool _Value)
            {
                set_flag(Flags::Selected, _Value);
            }

            void set_focused(bool _Value)
            {
                set_flag(Flags::Focused, _Value);
            }

            void set_flag(int _N, bool _Value)
            {
                if(_Value)
                    m_Flags |= ((unsigned int)1 << _N);
                else 
                    m_Flags &= ~((unsigned int)1 << _N);
            }
            
            // API
            void remove_all_children();
            
            void apply_to_children(const std::function<void(Object* _Object)>& _Callback) const;
            void apply_to_children_recursive(const std::function<void(Object* _Object)>& _Callback) const;

            Object* find_child(const std::function<bool(Object*)>& _Predicate) const;

            template<typename T> 
            T* find_child() const
            {
                return dynamic_cast<T*>(
                    find_child(
                        [](Object* _Object)->bool
                        {
                            return dynamic_cast<T*>(_Object) != nullptr;}
                        )
                    );
            }

            Object* find_child_recursive(const std::function<bool(Object*)>& _Predicate) const;
            std::list<Object*> find_children_recursive(const std::function<bool(Object*)>& _Predicate) const;

            template<typename T> 
            T* find_child_recursive() const
            {
                return dynamic_cast<T*>(
                    find_child_recursive(
                        [](Object* _Object)->bool
                        {
                            return dynamic_cast<T*>(_Object) != nullptr;}
                        )
                    );
            }

        protected:

            std::string                  m_Name     = std::string();
            Object*                      m_Parent   = nullptr;
            mutable std::list<Object*>   m_Children = std::list<Object*>();

            enum Flags
            {
                Selected,
                Hovered,
                Focused,
                LeftMouseClicked,
                RightMouseClicked,
                MiddleMouseClicked,
                LeftMouseDoubleClicked,
                RightMouseDoubleClicked,
                MiddleMouseDoubleClicked,
            };

            unsigned int m_Flags = 0;

        private:
            
            std::list<Object*>::iterator m_SelfIterator;
        };
    };
}