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

        typedef std::unique_ptr<Object>    object;
        typedef std::shared_ptr<Component> component;
        typedef Reference<Component>       componentRef;

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
            
            mutable Object* m_Object  = nullptr;
            mutable bool    m_Enabled = true;
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
            template<typename Type = Object> 
            Type* get_parent(const std::function<bool(Object*)>& _Predicate = 
                [](Object* _Object)->bool{return dynamic_cast<Type*>(_Object);}) const
            {
                if(_Predicate == nullptr) 
                    return dynamic_cast<Type*>(m_Parent);

                auto parent = m_Parent;

                while (parent != nullptr)
                {
                    if(_Predicate(parent)) 
                        return dynamic_cast<Type*>(parent);

                    parent = parent->m_Parent;
                }
                
                return nullptr;
            }

            std::list<object>& get_children() const;

            std::string get_name() const;
            bool check_flag(int _N) const;

            // setters
            void set_name(const std::string& _Value);
            void set_flag(int _N, bool _Value);
            
            // API
            template<bool _Recursive = true>
            void apply_to_children(const std::function<void(Object* _Object)>& _Callback) const
            {
                if(_Callback == nullptr) 
                    return;

                for(auto&& child : m_Children) 
                {
                    if(child == nullptr) 
                        continue;

                    _Callback(child.get());

                    if(_Recursive)
                        child->apply_to_children(_Callback);
                }
            }

            template<bool _Recursive = true>
            std::list<Object*> find_children(const std::function<bool(Object*)>& _Predicate) const
            {
                if(_Predicate == nullptr) 
                    return std::list<Object*>();

                std::list<Object*> result;

                apply_to_children<_Recursive>([&result, &_Predicate](Object* _Object)
                {
                    if(_Object != nullptr && _Predicate(_Object)) 
                        result.push_back(_Object);
                });

                return result;
            }

            template<typename Type, typename ...Arguments> 
            Type* create_child(Arguments ... _Args)
            {
                m_Children.push_back(std::make_unique<Type>( _Args ...));
                auto& child = m_Children.back();
                child->m_Parent = this;
                return dynamic_cast<Type*>(child.get());
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

            template<bool _Recursive = true>
            void remove_children(const std::function<bool(Object*)>& _Predicate)
            {
                if(_Predicate == nullptr) 
                    return;

                std::list<Object*> objects = find_children<_Recursive>(_Predicate);
                std::set<Object*>  objectsToRemove;

                for(auto&& object : objects)
                {
                    auto topMost = object;
                    auto parent  = object->get_parent();

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
                auto parent = get_parent();

                if(parent == nullptr) 
                    return;

                parent->remove_child(
                    [this](Object* _Object)->bool
                    {
                        return _Object == this;
                    }
                );
            }

            void move(Object* _Destination)
            {
                if(_Destination == nullptr || 
                    _Destination->get_parent([this](Object* _Object)->bool{return _Object == this;})) 
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
            template<typename Type, typename ... Arguments>
            Reference<Type> add_component(Arguments ... _Args)
            {
                // attach component
                m_Components.push_back(std::make_unique<Type>(_Args ...));
                auto component = Reference<Type>(m_Components.back());
                component->m_Object = this;

                // enable component
                component->set_enabled(true);

                return component;
            }

            template<typename Type>
            Reference<Type> get_component() const
            {
                for(auto&& component : m_Components)
                {
                    auto casted = Reference<Type>(component);

                    if(casted != nullptr) 
                        return casted;
                }
                return nullptr;
            }

            template<typename Type>
            void remove_component()
            {
                auto iterator = 
                    std::find_if(
                        m_Components.begin(), 
                        m_Components.end(), 
                        [](component& _Component)->bool
                        { 
                            return Reference<Type>(_Component);
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
            virtual void draw_self();

        protected:

            std::string  m_Name   = std::string();
            Object*      m_Parent = nullptr;
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