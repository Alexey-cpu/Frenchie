#include <FrenchieCoreSerialization.hpp>

#include <stack>
#include <vector>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Serialization;

Node::Node(const std::string& _Name, const Value& _Value) : 
    m_Name(_Name), m_Value(_Value){}

Node::~Node()
{
    for(auto&& child : m_Children) 
        delete child;
    m_Children.clear();
}

std::string& Node::name() const
{
    return m_Name;
}

Value& Node::value()
{
    return m_Value;
}

const std::vector<Node*>& Node::children() const
{
    return m_Children;
}

Node* Node::append_child(const std::string& _Name, const Value& _Value)
{
    m_Children.push_back(new Node(_Name, _Value));
    return m_Children[m_Children.size() - 1];
}