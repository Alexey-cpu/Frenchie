#include <FrenchieCoreSerialization.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Serialization;

// DocumentNode
std::string& Node::get_name() const
{
    return m_Name;
}

std::vector<Reference<Node>> Node::get_children_references() const
{
    if(m_Children.empty()) 
        return std::vector<Reference<Node>>();
    
    std::vector<Reference<Node>> children;
    for(auto child : m_Children) 
        children.push_back(Reference<Node>(child));

    return children;
}

const std::vector<std::shared_ptr<Node>>& Node::get_children() const
{
    return m_Children;
}

std::string Node::get_value_as_string() const
{
    if(is_of_type<bool>()) 
        return Helpers::to_string<bool>(get_value<bool>());

    if(is_of_type<char>()) 
        return Helpers::to_string<char>(get_value<char>());

    if(is_of_type<unsigned char>()) 
        return Helpers::to_string<unsigned char>(get_value<unsigned char>());

    if(is_of_type<short>()) 
        return Helpers::to_string<short>(get_value<short>());

    if(is_of_type<unsigned short>()) 
        return Helpers::to_string<unsigned short>(get_value<unsigned short>());

    if(is_of_type<int>()) 
        return Helpers::to_string<int>(get_value<int>());

    if(is_of_type<unsigned int>()) 
        return Helpers::to_string<unsigned int>(get_value<unsigned int>());

    if(is_of_type<long>()) 
        return Helpers::to_string<long>(get_value<long>());

    if(is_of_type<unsigned long>()) 
        return Helpers::to_string<unsigned long>(get_value<unsigned long>());

    if(is_of_type<long long>()) 
        return Helpers::to_string<long long>(get_value<long long>());

    if(is_of_type<unsigned long long>()) 
        return Helpers::to_string<unsigned long long>(get_value<unsigned long long>());

    if(is_of_type<float>()) 
        return Helpers::to_string<float>(get_value<float>());

    if(is_of_type<double>()) 
        return Helpers::to_string<double>(get_value<double>());

    if(is_of_type<long double>()) 
        return Helpers::to_string<long double>(get_value<long double>());

    if(is_of_type<std::string>()) 
        return Helpers::to_string<std::string>(get_value<std::string>());

    return std::string();
}

Reference<Node> Node::append_child(const std::string& _Name, const Value& _Value)
{
    m_Children.push_back(std::make_shared<Node>(_Name, _Value));
    auto& child = m_Children.back();
    return Reference<Node>(child);
}