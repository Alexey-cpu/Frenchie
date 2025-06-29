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

        /**
         * @brief Class that removes the copy constructor and operator from derived classes, while leaving move.
         */
        class NonCopyable {
        protected:
            NonCopyable() = default;
            virtual ~NonCopyable() = default;

        public:
            NonCopyable(const NonCopyable &) = delete;
            NonCopyable(NonCopyable &&) noexcept = default;
            NonCopyable &operator=(const NonCopyable &) = delete;
            NonCopyable &operator=(NonCopyable &&) noexcept = default;
        };

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

            Object* get_parent_recursive(const std::function<bool(Object*)>& _Predicate) const;
            std::string get_name() const;
            bool is_selected() const;
            bool is_focused() const;
            std::list<Object*> get_children() const;

            // setters
            void set_name(const std::string& _Value);
            void set_parent(Object* _Parent);
            void set_selected(bool _Value);
            void set_focused(bool _Value);
            void set_flag(int _N, bool _Value);
            
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

            enum Flags
            {
                Marked,
                Selected,
                Hovered,
                Focused
            };

            std::string                  m_Name     = std::string();
            Object*                      m_Parent   = nullptr;
            mutable std::list<Object*>   m_Children = std::list<Object*>();
            unsigned int                 m_Flags    = 0;

        private:
            
            std::list<Object*>::iterator m_SelfIterator;
        };
    };
}