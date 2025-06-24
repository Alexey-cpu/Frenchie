#include <FrenchieCoreObject.hpp>
#include <FrenchieCoreSingleton.hpp>

// STL
#include <set>

using namespace Frenchie::Core;

// Object
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
    // check tree
    if(_Parent != nullptr)
    {
        auto found = _Parent->get_parent_recursive(
            [this](Object* _Object)->bool
            {
                return _Object == this;
            });

        if(found != nullptr) 
            _Parent->set_parent(nullptr);
    }

    // setup parent
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

void Object::apply_to_children(const std::function<void(Object* _Object)>& _Callback) const
{
    if(_Callback == nullptr) 
        return;

    for(auto& child : m_Children) 
    {
        if(child != nullptr) 
            _Callback(child);
    }
}

void Object::apply_to_children_recursive(const std::function<void(Object* _Object)>& _Callback) const
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

Object* Object::find_child_recursive(const std::function<bool(Object*)>& _Predicate) const
{
    for(auto& child : m_Children) 
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