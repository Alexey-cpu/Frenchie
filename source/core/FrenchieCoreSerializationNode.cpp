#include <FrenchieCoreSerializationNode.hpp>

#include <stack>
#include <vector>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Serialization;

Node::Node(const std::string& _Name, const Value& _Value) : 
    m_Name(_Name), m_Value(_Value){}

Node::~Node()
{
    //std::cout << "Node::~Node() " << name() << "\n";

    if(m_Parent != nullptr) 
        return; // is not a root item

    // collect items recursivelly
    std::vector<Node*> stack;
    std::vector<Node*> objects;
    stack.push_back(this);

    while(!stack.empty())
    {
        auto top = stack.back();
        stack.pop_back();

        for(auto&& child : top->m_Children) 
        {
            stack.push_back(child);
            objects.push_back(child);
        }
    }

    // remove items recursivelly
    for(auto&& object : objects) 
        delete object;
}

std::string& Node::name() const
{
    return m_Name;
}

Value& Node::value()
{
    return m_Value;
}

const std::pmr::vector<Node*>& Node::children() const
{
    return m_Children;
}

Node* Node::append_child(const std::string& _Name, const Value& _Value)
{
    m_Children.push_back(new Node(_Name, _Value));
    auto child = m_Children[m_Children.size() - 1];
    child->m_Parent = this;
    return child;
}

Node* Node::find_child(const std::function<bool(Node*)>& _Predicate, bool _Recursive) const
{
    if(_Predicate == nullptr) 
        return nullptr;

    // iterative search
    if(!_Recursive)
    {
        for(auto&& child : m_Children)
        {
            if(_Predicate(child)) 
                return child;
        }

        return nullptr;
    }

    // recursive search
    std::vector<const Node*> stack;
    stack.push_back(this);

    while(!stack.empty())
    {
        auto top = stack.back();
        stack.pop_back();

        for(auto&& child : top->m_Children) 
        {
            if(_Predicate(child)) 
                return child;

            stack.push_back(child);
        }
    }

    return nullptr;
}

size_t Node::size() const
{
    return m_Children.size();
}

bool Node::empty() const
{
    return m_Children.empty();
}