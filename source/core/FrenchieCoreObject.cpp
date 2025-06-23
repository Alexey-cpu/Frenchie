#include <FrenchieCoreObject.hpp>

using namespace Frenchie::Core;

Object::Object(std::string _Name, Object* _Parent) : m_Name(_Name)
{
    set_parent(_Parent);
}

Object::~Object()
{
    set_parent(nullptr);
    remove_all_children();
}

std::list<Object*> Object::get_children() const
{
    return m_Children;
}

void Object::set_parent(Object* _Parent)
{
    if(m_Parent != nullptr) 
        m_Parent->m_Children.erase(m_SelfIterator);
    m_Parent = _Parent;

    if(m_Parent == nullptr) 
        return;
    
    m_Parent->m_Children.push_back(this);
    m_SelfIterator = std::prev(m_Parent->m_Children.end());
}

void Object::remove_all_children()
{
    auto children = get_children();

    for(auto& child : children) 
    {
        if(child != nullptr) 
            delete child;
    }

    m_Children.clear();
}

void Object::apply_to_children_recursive(const std::function<void(Object* _Object)>& _Callback)
{
    if(_Callback == nullptr) 
        return;

    for(auto& child : m_Children) 
    {
        if(child == nullptr) 
            continue;

        _Callback(child);
        
        child->apply_to_children_recursive(_Callback);
    }
}