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


// utility functions
#define __support_scalar__(__type)\
template<> Node Document::append_value_node(const char* _Name, const __type& _Value, const Node& _Parent) const\
{\
    return Node(m_NodeConstructor.append_node(_Name, Helpers::to_string<__type>(_Value).c_str(), _Parent.m_Info));\
}\

#define __support_vector__(__type)\
template<> Node Document::append_value_node(const char* _Name, const std::vector<__type>& _Values, const Node& _Parent) const\
{\
    auto container = Document::append_node(_Name, STRINGIFY(std::vector<__type>), _Parent);\
    for(auto&& value : _Values) append_value_node<__type>("item", value, container);\
    return container;\
}\

#define __support_list__(__type)\
template<> Node Document::append_value_node(const char* _Name, const std::list<__type>& _Values, const Node& _Parent) const\
{\
    auto container = Document::append_node(_Name, STRINGIFY(std::list<__type>), _Parent);\
    for(auto&& value : _Values) append_value_node<__type>("item", value, container);\
    return container;\
}\

#define __support_set__(__type)\
template<> Node Document::append_value_node(const char* _Name, const std::set<__type>& _Values, const Node& _Parent) const\
{\
    auto container = Document::append_node(_Name, STRINGIFY(std::set<__type>), _Parent);\
    for(auto&& value : _Values) append_value_node<__type>("item", value, container);\
    return container;\
}\

__support_scalar__(bool)
__support_scalar__(float)
__support_scalar__(double)
__support_scalar__(int)
__support_scalar__(unsigned int)
__support_scalar__(long)
__support_scalar__(unsigned long)
__support_scalar__(long long)
__support_scalar__(unsigned long long)

__support_vector__(bool)
__support_vector__(float)
__support_vector__(double)
__support_vector__(int)
__support_vector__(unsigned int)
__support_vector__(long)
__support_vector__(unsigned long)
__support_vector__(long long)
__support_vector__(unsigned long long)

__support_list__(bool)
__support_list__(float)
__support_list__(double)
__support_list__(int)
__support_list__(unsigned int)
__support_list__(long)
__support_list__(unsigned long)
__support_list__(long long)
__support_list__(unsigned long long)

__support_set__(bool)
__support_set__(float)
__support_set__(double)
__support_set__(int)
__support_set__(unsigned int)
__support_set__(long)
__support_set__(unsigned long)
__support_set__(long long)
__support_set__(unsigned long long)

#undef __support_scalar__
#undef __support_vector__