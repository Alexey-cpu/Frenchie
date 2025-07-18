#pragma once

#include <FrenchieCoreFactory.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreReference.hpp>
#include <FrenchieCoreNonCopyable.hpp>

#include <FrenchieRendererIEditor.hpp>

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

        typedef std::unique_ptr<Object> object;
        typedef Object* objectRef;
        typedef std::shared_ptr<Component> component;
        typedef Reference<Component> componentRef;

        class Component : 
            public NonCopyable,
            public Frenchie::Core::Factory::Creator<Component>
        {
        public:

            Component();
            virtual ~Component();

            template<typename T = Object>
            T* get_object() const
            {
                return dynamic_cast<T*>(m_Object);
            }

            virtual std::string get_name() const
            {
                return STRINGIFY(Component);
            }

            bool is_enabled() const;
            void set_enabled(bool _Value);

            // IRenderer
            virtual void on_enabled();
            virtual void on_disabled();

            virtual bool awake();
            virtual void frame_start();
            virtual void frame_update();
            virtual void frame_finish();
            
        protected:
            
            mutable objectRef m_Object  = nullptr;
            mutable bool      m_Enabled = true;
            friend class Object;
        };

        class Object : public NonCopyable, public Frenchie::Renderer::IEditor
        {
        private:

        public:
            
            enum Flags
            {
                Marked,
                Selected,
                Focused
            };

            Object(const std::string& _Name = std::string());
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
                        [](objectRef _Object)->bool
                        {
                            return dynamic_cast<T*>(_Object) != nullptr;}
                        )
                    );
            }

            objectRef get_parent_recursive(const std::function<bool(objectRef)>& _Predicate) const;
            std::list<object>& get_children() const;

            std::string get_name() const;
            bool check_flag(int _N) const;

            // setters
            void set_name(const std::string& _Value);
            void set_flag(int _N, bool _Value);
            
            // API
            void apply_to_children(const std::function<void(objectRef _Object)>& _Callback) const;
            void apply_to_children_recursive(const std::function<void(objectRef _Object)>& _Callback) const;

            objectRef find_child(const std::function<bool(objectRef)>& _Predicate) const;

            template<typename T> 
            T* find_child() const
            {
                return dynamic_cast<T*>(
                    find_child(
                        [](objectRef _Object)->bool
                        {
                            return dynamic_cast<T*>(_Object) != nullptr;}
                        )
                    );
            }

            objectRef find_child_recursive(const std::function<bool(objectRef)>& _Predicate) const;
            std::list<objectRef> find_children_recursive(const std::function<bool(objectRef)>& _Predicate) const;

            template<typename T> 
            T* find_child_recursive() const
            {
                return dynamic_cast<T*>(
                    find_child_recursive(
                        [](objectRef _Object)->bool
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

            void remove_child(const std::function<bool(objectRef)>& _Predicate)
            {
                if(_Predicate == nullptr) 
                    return;

                for(auto it = m_Children.begin(); it != m_Children.end(); it++)
                {
                    if(_Predicate((*it).get()))
                    {
                        m_Children.erase(it);
                        return;
                    }
                }
            }

            template<typename T> 
            void remove_child()
            {
                remove_child([](objectRef _Object)->bool
                {
                    return dynamic_cast<T>(_Object) != nullptr;
                }
                );
            }

            void remove_children(const std::function<bool(objectRef)>& _Predicate)
            {
                if(_Predicate == nullptr) 
                    return;

                std::list<objectRef> objects = 
                    find_children_recursive(_Predicate);

                std::set<objectRef> objectsToRemove;

                for(auto&& object : objects)
                {
                    objectRef topMost = object;
                    objectRef parent  = object->get_parent();

                    while (parent != nullptr)
                    {
                        if(parent->check_flag(Object::Flags::Focused)) 
                            topMost = parent;
                        parent = parent->get_parent();
                    }

                    objectsToRemove.insert(topMost);
                }

                for(auto&& objectToRemove : objectsToRemove) 
                    objectToRemove->remove_self();
            }

            void remove_self()
            {
                get_parent()->remove_child(
                    [this](objectRef _Object)->bool
                    {
                        return _Object == this;
                    }
                );
            }

            void move(objectRef _Destination)
            {
                if(_Destination == nullptr || 
                    _Destination->get_parent_recursive([this](objectRef _Object)->bool{return _Object == this;})) 
                    return;

                if(m_Parent == nullptr)
                {
                    auto self = this;
                    _Destination->m_Children.push_back(object(self));
                    self->m_Parent = _Destination;
                    return;
                }

                for(auto it = m_Parent->m_Children.begin(); it != m_Parent->m_Children.end(); it++)
                {
                    if((*it).get() == this)
                    {
                        auto self = (*it).release();

                        m_Parent->m_Children.erase(it);

                        _Destination->m_Children.push_back(object(self));
                        self->m_Parent = _Destination;
                        return;
                    }
                }
            }

            // components
            template<typename T, typename ... Arguments>
            Reference<T> add_component(Arguments ... _Args)
            {
                // attach component
                m_Components.push_back(std::make_unique<T>(_Args ...));
                auto component = Reference<T>(m_Components.back());
                component->m_Object = this;

                // enable component
                component->set_enabled(true);

                return component;
            }

            template<typename T>
            Reference<T> get_component() const
            {
                for(auto&& component : m_Components)
                {
                    auto casted = Reference<T>(component);

                    if(casted != nullptr) 
                        return casted;
                }
                return nullptr;
            }

            template<typename T>
            void remove_component()
            {
                auto iterator = 
                    std::find_if(
                        m_Components.begin(), 
                        m_Components.end(), 
                        [](component& _Component)->bool
                        { 
                            return Reference<T>(_Component);
                        }
                    );

                if(iterator != m_Components.end()) 
                    m_Components.erase(iterator);
            }

            // template<typename T>
            // T* get_component_in_children() const
            // {
            //     for(auto&& child : m_Children)
            //     {
            //         auto component = child->get_component<T>();

            //         if(component != nullptr) 
            //             return component;
            //     }

            //     return nullptr;
            // }

            // template<typename T>
            // T* get_component_in_children_recursive() const
            // {
            //     for(auto&& child : m_Children)
            //     {
            //         auto component = child->get_component_in_children_recursive<T>();

            //         if(component != nullptr) 
            //             return component;
            //     }

            //     return nullptr;
            // }

            virtual bool awake();
            virtual void frame_start();
            virtual void frame_update();
            virtual void frame_finish();

            // IEditor
            virtual void draw_editor() override;
            virtual void draw_self();

        protected:

            std::string  m_Name   = std::string();
            objectRef    m_Parent = nullptr;
            unsigned int m_Flags  = 0;

            mutable std::list<object> m_Children   = std::list<object>();
            std::list<component>      m_Components = std::list<component>();

            static class Editor
            {
                public:
                    char m_Name[512]{};
            } m_Editor;
        };
    };
}