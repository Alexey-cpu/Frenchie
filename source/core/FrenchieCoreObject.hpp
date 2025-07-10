#pragma once

#include <FrenchieCoreFactory.hpp>
#include <FrenchieCoreHelpers.hpp>
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
            virtual bool awake();
            virtual void frame_start();
            virtual void frame_update();
            virtual void frame_finish();
            
        protected:
            
            mutable Object* m_Object  = nullptr;
            bool            m_Enabled = true;
            friend class Object;
        };

        class Object : public NonCopyable, public Frenchie::Renderer::IEditor
        {
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
                        [](Object* _Object)->bool
                        {
                            return dynamic_cast<T*>(_Object) != nullptr;}
                        )
                    );
            }

            Object* get_parent_recursive(const std::function<bool(Object*)>& _Predicate) const;
            std::string get_name() const;
            bool check_flag(int _N) const;
            std::list<std::unique_ptr<Object>>& get_children() const;

            // setters
            void set_name(const std::string& _Value);
            void set_flag(int _N, bool _Value);
            
            // API
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

            template<typename T, typename ...Arguments> 
            T* create_child(Arguments ... _Args)
            {
                m_Children.push_back(std::make_unique<T>( _Args ...));
                T* child = dynamic_cast<T*>(m_Children.back().get());
                child->m_Parent = this;
                return child;
            }

            void remove_child(const std::function<bool(Object*)>& _Predicate)
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
                remove_child([](Object* _Object)->bool
                {
                    return dynamic_cast<T>(_Object) != nullptr;
                }
                );
            }

            void remove_children(const std::function<bool(Object*)>& _Predicate)
            {
                if(_Predicate == nullptr) 
                    return;

                std::list<Object*> objects = 
                    find_children_recursive(_Predicate);

                std::set<Object*> objectsToRemove;

                for(auto&& object : objects)
                {
                    Object* topMost = object;
                    Object* parent  = object->get_parent();

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
                    [this](Object* _Object)->bool
                    {
                        return _Object == this;
                    }
                );
            }

            void move(Object* _Destination)
            {
                if(_Destination == nullptr || 
                    _Destination->get_parent_recursive([this](Object* _Object)->bool{return _Object == this;})) 
                    return;

                for(auto it = m_Parent->m_Children.begin(); it != m_Parent->m_Children.end(); it++)
                {
                    if((*it).get() == this)
                    {
                        auto self = (*it).release();

                        m_Parent->m_Children.erase(it);

                        _Destination->m_Children.push_back(std::unique_ptr<Object>(self));
                        self->m_Parent = _Destination;
                        return;
                    }
                }
            }

            template<typename T>
            T* get_component() const
            {
                for(auto&& component : m_Components)
                {
                    T* casted = dynamic_cast<T*>(component.get());

                    if(casted != nullptr) 
                        return casted;
                }
                return nullptr;
            }

            template<typename T, typename ... Arguments>
            T* add_component(Arguments ... _Args)
            {
                m_Components.push_back(std::make_unique<T>(_Args ...));
                auto component = dynamic_cast<T*>(m_Components.back().get());
                component->m_Object = this;
                return component;
            }

            template<typename T>
            void remove_component()
            {
                auto iterator = 
                    std::find_if(
                        m_Components.begin(), 
                        m_Components.end(), 
                        [](std::unique_ptr<Component>& _Component)->bool
                        { 
                            return dynamic_cast<T*>(_Component.get());
                        }
                    );

                if(iterator != m_Components.end()) 
                    m_Components.erase(iterator);
            }

            virtual bool awake();
            virtual void frame_start();
            virtual void frame_update();
            virtual void frame_finish();

            // IEditor
            virtual void draw_editor() override;

        protected:

            std::string  m_Name   = std::string();
            Object*      m_Parent = nullptr;
            unsigned int m_Flags  = 0;

            mutable std::list<std::unique_ptr<Object>> m_Children = 
                std::list<std::unique_ptr<Object>>();

            std::list<std::unique_ptr<Component>> m_Components = 
                std::list<std::unique_ptr<Component>>();

            static class Editor
            {
                public:
                    char m_Name[512]{};
            } m_Editor;
        };
    };
}