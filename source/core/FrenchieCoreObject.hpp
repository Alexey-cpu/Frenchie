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
        class Object;
        class Component;
        class Hierarchy;

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

            bool is_enabled() const
            {
                return m_Enabled;
            }

            void set_enabled(bool _Value)
            {
                m_Enabled = _Value;
            }
            
        protected:
            Object* m_Object  = nullptr;
            bool    m_Enabled = true;

            friend class Object;
        };

        class Object : public NonCopyable
        {
        public:

            template<typename T, typename ... Arguments>
            T add_component(Arguments ... _Arguments)
            {
                m_Components.push_back(std::make_unique<T>(_Arguments));
                return dynamic_cast<T*>(m_Components.back().get());
            }

            template<typename T>
            void remove_component()
            {
                auto iterator = 
                    std::find_if(
                        m_Components.begin(), 
                        m_Components.end(), 
                        [](std::unique_ptr<Component> _Component)->bool
                        { 
                            return dynamic_cast<T*>(_Component.get());
                        }
                    );

                if(iterator != m_Components.end()) 
                    m_Components.erase(iterator);
            }

        protected:
            std::list<std::unique_ptr<Component>> m_Components = 
                std::list<std::unique_ptr<Component>>();
        };

        class Hierarchy
        {
        public:
            
            Hierarchy(const std::string& _Name = std::string());
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
            std::list<std::unique_ptr<Hierarchy>>& get_children() const;

            // setters
            void set_name(const std::string& _Value);
            void set_selected(bool _Value);
            void set_focused(bool _Value);
            void set_flag(int _N, bool _Value);
            
            // API
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

            template<typename T, typename ...Arguments> 
            T* create_child(Arguments ... _Args)
            {
                m_Children.push_back(std::make_unique<T>( _Args ...));
                T* child = dynamic_cast<T*>(m_Children.back().get());
                child->m_Parent = this;
                return child;
            }

            template<typename T> 
            void remove_child()
            {
                auto iterator = 
                    std::find_if(
                        m_Components.begin(), 
                        m_Components.end(), 
                        [](std::unique_ptr<Hierarchy> _Component)->bool
                        { 
                            return dynamic_cast<T*>(_Component.get());
                        }
                    );

                if(iterator != m_Components.end()) 
                    m_Components.erase(iterator);
            }

        protected:

            enum Flags
            {
                Marked,
                Selected,
                Hovered,
                Focused
            };

            std::string  m_Name  = std::string();
            Hierarchy*   m_Parent = nullptr;
            unsigned int m_Flags  = 0;

            mutable std::list<std::unique_ptr<Hierarchy>> m_Children = 
                std::list<std::unique_ptr<Hierarchy>>();
        };
    };
}