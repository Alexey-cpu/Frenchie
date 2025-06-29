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
        class Hierarchy;
        class Object;

        class NonCopyable 
        {
        protected:
            NonCopyable() = default;
            virtual ~NonCopyable() = default;

        public:
            NonCopyable(const NonCopyable &) = delete;
            NonCopyable(NonCopyable &&) noexcept = default;
            NonCopyable &operator=(const NonCopyable &) = delete;
            NonCopyable &operator=(NonCopyable &&) noexcept = default;
        };

        class Component : public NonCopyable
        {
        public:
            // add some interface here ...
        protected:
            Object* m_Object = nullptr;
            friend class Object;
        };

        class Object : public NonCopyable
        {
        public:

            template<typename Type, typename ... Arguments>
            Type add_component(Arguments ... _Arguments)
            {
                m_Components.push_back(std::make_unique<Type>(_Arguments));
                return m_Components.last().last();
            }

            template<typename Type>
            void remove_component()
            {
                auto iterator = 
                    std::find_if(
                        m_Components.begin(), 
                        m_Components.end(), 
                        [](std::unique_ptr<Component> _Component)->bool
                        { 
                            return dynamic_cast<Type*>(_Component.get());
                        }
                    );

                if(iterator != m_Components.end()) 
                    m_Components.erase(iterator);
            }


        protected:
            std::list<std::unique_ptr<Component>> m_Components = 
                std::list<std::unique_ptr<Component>>();
        };

        class Root final
        {
        public:
            Root();
            ~Root();

            void push(Hierarchy* _Object);
            void pop(Hierarchy* _Object);
            bool is_being_restroyed() const;

        private:

            bool              m_IsBeingDestroed   = false;
            int               m_ReferenceCounter  = 0;
            std::set<Hierarchy*> m_Objects           = std::set<Hierarchy*>();

            friend class Hierarchy;
        };

        class Hierarchy
        {
        public:
            
            Hierarchy(const std::string& _Name = std::string(), Hierarchy* _Parent = nullptr);
            virtual ~Hierarchy();

            // getters
            template<typename T = Hierarchy> 
            T* get_parent() const
            {
                return dynamic_cast<T*>(m_Parent);
            }

            template<typename T> 
            T* get_parent_recursive() const
            {                
                return dynamic_cast<T*>(
                    get_parent_recursive(
                        [](Hierarchy* _Object)->bool
                        {
                            return dynamic_cast<T*>(_Object) != nullptr;}
                        )
                    );
            }

            Hierarchy* get_parent_recursive(const std::function<bool(Hierarchy*)>& _Predicate) const;
            std::string get_name() const;
            bool is_selected() const;
            bool is_focused() const;
            std::list<Hierarchy*> get_children() const;

            // setters
            void set_name(const std::string& _Value);
            void set_parent(Hierarchy* _Parent);
            void set_selected(bool _Value);
            void set_focused(bool _Value);
            void set_flag(int _N, bool _Value);
            
            // API
            void remove_all_children();
            
            void apply_to_children(const std::function<void(Hierarchy* _Object)>& _Callback) const;
            void apply_to_children_recursive(const std::function<void(Hierarchy* _Object)>& _Callback) const;

            Hierarchy* find_child(const std::function<bool(Hierarchy*)>& _Predicate) const;

            template<typename T> 
            T* find_child() const
            {
                return dynamic_cast<T*>(
                    find_child(
                        [](Hierarchy* _Object)->bool
                        {
                            return dynamic_cast<T*>(_Object) != nullptr;}
                        )
                    );
            }

            Hierarchy* find_child_recursive(const std::function<bool(Hierarchy*)>& _Predicate) const;
            std::list<Hierarchy*> find_children_recursive(const std::function<bool(Hierarchy*)>& _Predicate) const;

            template<typename T> 
            T* find_child_recursive() const
            {
                return dynamic_cast<T*>(
                    find_child_recursive(
                        [](Hierarchy* _Object)->bool
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

            std::string                   m_Name     = std::string();
            Hierarchy*                    m_Parent   = nullptr;
            mutable std::list<Hierarchy*> m_Children = std::list<Hierarchy*>();
            unsigned int                  m_Flags    = 0;

        private:
            
            std::list<Hierarchy*>::iterator m_SelfIterator;
        };
    };
}