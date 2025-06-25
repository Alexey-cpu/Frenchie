#include <FrenchieCoreObject.hpp>
#include <FrenchieCoreSingleton.hpp>

// STL
#include <set>
#include <iostream>

using namespace Frenchie::Core;

// Object
Object::Object(const std::string& _Name, Object* _Parent) : m_Name(_Name)
{
    set_parent(_Parent);
}

Object::~Object()
{
    // detach self from parent
    if(m_Parent != nullptr) 
        m_Parent->m_Children.erase(m_SelfIterator);

    remove_all_children();
}

std::list<Object*> Object::get_children() const
{
    return m_Children;
}

void Object::set_parent(Object* _Parent)
{
    // detach self from parent
    if(m_Parent != nullptr) 
        m_Parent->m_Children.erase(m_SelfIterator);
    m_Parent = _Parent;

    if(m_Parent == nullptr) 
        return;
    
    // break cycle
    auto found = m_Parent->get_parent_recursive(
        [this](Object* _Object)->bool
        {
            return _Object == this;
        });

    if(found != nullptr) 
        m_Parent->set_parent(nullptr);

    // attach to a new parent
    m_Parent->m_Children.push_back(this);
    m_SelfIterator = std::prev(m_Parent->m_Children.end());
}

void Object::remove_all_children()
{
    auto children = m_Children;

    for(auto&& child : children)
    {
        if(child != nullptr) 
            delete child;
    }

    m_Children.clear();
}

void Object::apply_to_children(const std::function<void(Object* _Object)>& _Callback) const
{
    if(_Callback == nullptr) 
        return;

    for(auto&& child : m_Children)
    {
        if(child != nullptr) 
            _Callback(child);
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

        _Callback(child);

        child->apply_to_children_recursive(_Callback);
    }
}

Object* Object::find_child(const std::function<bool(Object*)>& _Predicate) const
{
    for(auto&& child : m_Children) 
    {
        if(child != nullptr && _Predicate(child)) 
            return child;
    }

    return nullptr;
}

Object* Object::find_child_recursive(const std::function<bool(Object*)>& _Predicate) const
{
    for(auto&& child : m_Children) 
    {
        if(child == nullptr) 
            continue;
        
        if(_Predicate(child)) 
            return child;

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