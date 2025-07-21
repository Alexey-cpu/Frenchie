#include <FrenchieCoreSerialization.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Serialization;

// DocumentNode
Node::Node(const std::string& _Name) : 
    m_Name(_Name){}

Node::~Node(){}

std::string Node::get_name() const
{
    return m_Name;
}

std::vector<Reference<Node>> Node::get_children() const
{
    if(m_Children.empty()) 
        return std::vector<Reference<Node>>();
    
    std::vector<Reference<Node>> children;
    for(auto child : m_Children) 
        children.push_back(Reference<Node>(child));

    return children;
}

void Node::set_name(const std::string& _Name) 
{
    m_Name = _Name;
}

Reference<Node> Node::append_child(const std::string& _Name)
{
    m_Children.push_back(std::make_shared<Node>(_Name));
    auto& child = m_Children.back();
    return Reference<Node>(child);
}