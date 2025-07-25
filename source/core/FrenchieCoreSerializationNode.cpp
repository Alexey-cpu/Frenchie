#include <FrenchieCoreSerializationNode.hpp>

#include <stack>
#include <vector>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Serialization;

// Iterator
Iterator::Iterator(const Document* _Document, int _Index) : m_Document(_Document), m_Index(_Index){}
Iterator::~Iterator(){}

Node Iterator::operator*() const 
{
    if(m_Document == nullptr) 
        return Node();

    const auto& hierarchy = m_Document->m_NodeConstructor.hierarchy();

    return Node(hierarchy.m_Items[m_Index], m_Document);
}

const NodeInfo* Iterator::operator->() const
{
    if(m_Document == nullptr) 
        return nullptr;

    const auto& hierarchy = m_Document->m_NodeConstructor.hierarchy();

    return hierarchy.m_Items[m_Index];
}

// prefix
Iterator& Iterator::operator++() 
{ 
    m_Index++; 
    return *this; 
}

Iterator& Iterator::operator--() 
{ 
    m_Index--; 
    return *this; 
}

Iterator Iterator::operator++(int) 
{ 
    Iterator tmp = *this; 
    ++(*this); 
    return tmp; 
}

Iterator Iterator::operator--(int) 
{ 
    Iterator tmp = *this; 
    --(*this); 
    return tmp; 
}

int Iterator::distance(const Iterator& _First, const Iterator& _Last)
{
    return _Last.m_Index - _First.m_Index;
}

// Node
Node::Node(NodeInfo* _Info, const Document* _Document) : 
    m_Info(_Info), m_Document(_Document){}

bool Node::valid() const
{
    return m_Info != nullptr && m_Document != nullptr;
}

const char* Node::name() const
{
    return m_Info != nullptr ? m_Info->Name : m_EmptyNode.Name;
}

const char* Node::value() const
{
    return m_Info != nullptr ? m_Info->Value : m_EmptyNode.Value;
}

const Iterator Node::begin() const
{
    if(!valid()) 
        return Iterator(nullptr, -1);

    const auto& hierarchy = 
        m_Document->m_NodeConstructor.hierarchy();

    return Iterator(m_Document, hierarchy.m_Pointers[m_Info->Self]);
}

const Iterator Node::end() const
{
    if(!valid()) 
        return Iterator(nullptr, -1);

    const auto& hierarchy = 
        m_Document->m_NodeConstructor.hierarchy();

    return Iterator(m_Document, hierarchy.m_Pointers[m_Info->Self + 1]);
}

Node Node::append_node(const char* _Name, const char* _Value)
{
    if(m_Document == nullptr) 
        return Node();

    return Node(m_Document->append_node(_Name, _Value, *this));
}

// Document
template<> Node Document::append_value(const char* _Name, const bool& _Value, const Node& _Parent)
{
    return Node(m_NodeConstructor.append_node(_Name, Helpers::to_string<bool>(_Value).c_str(), _Parent.m_Info));
}

template<> Node Document::append_value(const char* _Name, const float& _Value, const Node& _Parent)
{
    return Node(m_NodeConstructor.append_node(_Name, Helpers::to_string<float>(_Value).c_str(), _Parent.m_Info));
}

template<> Node Document::append_value(const char* _Name, const double& _Value, const Node& _Parent)
{
    return Node(m_NodeConstructor.append_node(_Name, Helpers::to_string<double>(_Value).c_str(), _Parent.m_Info));
}

template<> Node Document::append_value(const char* _Name, const int& _Value, const Node& _Parent)
{
    return Node(m_NodeConstructor.append_node(_Name, Helpers::to_string<int>(_Value).c_str(), _Parent.m_Info));
}

template<> Node Document::append_value(const char* _Name, const unsigned int& _Value, const Node& _Parent)
{
    return Node(m_NodeConstructor.append_node(_Name, Helpers::to_string<unsigned int>(_Value).c_str(), _Parent.m_Info));
}

template<> Node Document::append_value(const char* _Name, const long& _Value, const Node& _Parent)
{
    return Node(m_NodeConstructor.append_node(_Name, Helpers::to_string<long>(_Value).c_str(), _Parent.m_Info));
}

template<> Node Document::append_value(const char* _Name, const unsigned long& _Value, const Node& _Parent)
{
    return Node(m_NodeConstructor.append_node(_Name, Helpers::to_string<unsigned long>(_Value).c_str(), _Parent.m_Info));
}

template<> Node Document::append_value(const char* _Name, const long long& _Value, const Node& _Parent)
{
    return Node(m_NodeConstructor.append_node(_Name, Helpers::to_string<long long>(_Value).c_str(), _Parent.m_Info));
}

template<> Node Document::append_value(const char* _Name, const unsigned long long& _Value, const Node& _Parent)
{
    return Node(m_NodeConstructor.append_node(_Name, Helpers::to_string<unsigned long long>(_Value).c_str(), _Parent.m_Info));
}

// template<>
// Node Document::append_value(const char* _Name, const std::vector<bool>& _Values, const Node& _Parent)
// {
//     auto container = append_value(_Name, "", _Parent);

//     for(auto&& value : _Values) 
//         append_value<bool>(_Name, value, container);

//     return container;
// }

// template<>
// Node Document::append_value(const char* _Name, const std::vector<float>& _Values, const Node& _Parent)
// {
//     auto container = append_value(_Name, "", _Parent);

//     for(auto&& value : _Values) 
//         append_value<float>(_Name, value, container);

//     return container;
// }

template<>
Node Document::append_value(const char* _Name, const std::vector<double>& _Values, const Node& _Parent)
{
    auto container = Document::append_node(_Name, "", _Parent);

    for(auto&& value : _Values) 
        append_value<double>("", value, container);

    return container;
}