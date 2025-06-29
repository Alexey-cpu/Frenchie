#include <FrenchieCoreObject.hpp>
#include <FrenchieCoreLogger.hpp>
#include <FrenchieCoreSingleton.hpp>

// STL
#include <set>
#include <iostream>

using namespace Frenchie::Core;

// Component
Component::Component(){}

Component::~Component(){}

bool Component::is_enabled() const
{
    return m_Enabled;
}

void Component::set_enabled(bool _Value)
{
    m_Enabled = _Value;
}

// Object
Object::Object(const std::string& _Name) : m_Name(_Name){}

Object::~Object(){}

Object* Object::get_parent_recursive(const std::function<bool(Object*)>& _Predicate) const
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

std::string Object::get_name() const
{
    return m_Name;
}

bool Object::check_flag(int _N) const
{
    return (bool)((m_Flags >> _N) & 1);
}

std::list<std::unique_ptr<Object>>& Object::get_children() const
{
    return m_Children;
}

void Object::set_name(const std::string& _Value)
{
    m_Name = _Value;
}

void Object::set_flag(int _N, bool _Value)
{
    if(_Value)
        m_Flags |= ((unsigned int)1 << _N);
    else 
        m_Flags &= ~((unsigned int)1 << _N);
}

void Object::apply_to_children(const std::function<void(Object* _Object)>& _Callback) const
{
    if(_Callback == nullptr) 
        return;

    for(auto&& child : m_Children)
    {
        if(child != nullptr) 
            _Callback(child.get());
    }
}

void Object::apply_to_children_recursive(const std::function<void(Object* _Object)>& _Callback) const
{
    if(_Callback == nullptr) 
        return;

    for(auto&& child : m_Children) 
    {
        if(child == nullptr) 
            continue;

        _Callback(child.get());

        child->apply_to_children_recursive(_Callback);
    }
}

Object* Object::find_child(const std::function<bool(Object*)>& _Predicate) const
{
    for(auto&& child : m_Children) 
    {
        if(_Predicate(child.get())) 
            return child.get();
    }

    return nullptr;
}

Object* Object::find_child_recursive(const std::function<bool(Object*)>& _Predicate) const
{
    for(auto&& child : m_Children) 
    {
        if(child == nullptr) 
            continue;
        
        if(_Predicate(child.get())) 
            return child.get();

        return child->find_child_recursive(_Predicate);
    }

    return nullptr;
}

std::list<Object*> Object::find_children_recursive(const std::function<bool(Object*)>& _Predicate) const
{
    if(_Predicate == nullptr) 
        return std::list<Object*>();

    std::list<Object*> result;

    apply_to_children_recursive([&result, &_Predicate](Object* _Object)
    {
        if(_Object != nullptr && _Predicate(_Object)) 
            result.push_back(_Object);
    });

    return result;
}