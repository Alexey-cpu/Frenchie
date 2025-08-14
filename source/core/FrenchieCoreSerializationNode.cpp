#include <FrenchieCoreSerializationNode.hpp>

#include <stack>
#include <vector>
#include <type_traits>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Serialization;

namespace Frenchie
{
    namespace Core
    {
        namespace Helpers
        {
            template<typename __type>
            bool is_number(const __type& _Value)
            {
                return std::is_same<decltype(_Value), short>::value           || 
                    std::is_same<decltype(_Value), int>::value                ||
                    std::is_same<decltype(_Value), long>::value               ||
                    std::is_same<decltype(_Value), long long>::value          ||
                    std::is_same<decltype(_Value), unsigned short>::value     ||
                    std::is_same<decltype(_Value), unsigned int>::value       ||
                    std::is_same<decltype(_Value), unsigned long>::value      ||
                    std::is_same<decltype(_Value), unsigned long long>::value ||
                    std::is_same<decltype(_Value), float>::value              ||
                    std::is_same<decltype(_Value), double>::value;
            }

            template<typename __type>
            bool is_bool(const __type& _Value)
            {
                return std::is_same<decltype(_Value), bool>::value;
            }

            template<typename __type>
            bool is_null(const __type& _Value)
            {
                return std::is_same<decltype(_Value), nullptr_t>::value;
            }
        }
    }
}

// NodeHierarchy
void NodeHierarchy::build(const std::vector<NodeInfo*>& _Nodes)
{
    if(_Nodes.empty()) 
        return;

    m_Items.resize(_Nodes.size());
    m_Pointers.resize(_Nodes.size() + 1);
    m_Singletones.clear();
    std::vector<int> workspace(_Nodes.size() + 1);

    for (size_t i = 0; i < _Nodes.size(); i++)
    {
        m_Items   [i]     = _Nodes[i];
        m_Pointers[i] = 0;
        workspace [i] = 0;
    }

    m_Pointers[_Nodes.size()] = 0;
    workspace[_Nodes.size()] = 0;

    for(auto&& item : _Nodes) 
    {
        if(!item->Parent) 
            continue;

        m_Pointers[item->Parent->Self]++;
        workspace[item->Parent->Self]++;
    }

    // cumulative sum
    for( int i = 0, j = 0, k = 0 ; i < _Nodes.size() + 1; i++ )
    {
        k += workspace[i];
        workspace[i] = j;
        m_Pointers[i] = j;
        j = k;
    }

    // count sort
    for(int i = 0; i < _Nodes.size(); i++ )
    {
        if(!_Nodes[i]->Parent) 
            continue;

        int index      = workspace[_Nodes[i]->Parent->Self]++;
        m_Items[index] = _Nodes[i];
    }

    // collect singletones
    for(auto node : _Nodes)
    {
        if(node->Parent == nullptr) 
            m_Singletones.push_back(node);
    }

    m_IsDirty = false;
}

// NodeIterator
NodeIterator::NodeIterator(const Node& _Node, int _Index) : 
    m_Document(_Node.is_valid() ? _Node.m_Info->Document : nullptr), 
    m_Index(_Index){}

NodeIterator::~NodeIterator(){}

Node NodeIterator::operator*() const 
{
    if(m_Document == nullptr) 
        return Node();

    const auto& hierarchy = m_Document->hierarchy();

    return Node(hierarchy.m_Items[m_Index]);
}

const NodeInfo* NodeIterator::operator->() const
{
    if(m_Document == nullptr) 
        return nullptr;

    const auto& hierarchy = m_Document->hierarchy();

    return hierarchy.m_Items[m_Index];
}

NodeIterator& NodeIterator::operator++() 
{ 
    m_Index++; 
    return *this; 
}

NodeIterator& NodeIterator::operator--() 
{ 
    m_Index--; 
    return *this; 
}

NodeIterator NodeIterator::operator++(int) 
{ 
    NodeIterator tmp = *this; 
    ++(*this); 
    return tmp; 
}

NodeIterator NodeIterator::operator--(int) 
{ 
    NodeIterator tmp = *this; 
    --(*this); 
    return tmp; 
}

int NodeIterator::distance(const NodeIterator& _First, const NodeIterator& _Last)
{
    return _Last.m_Index - _First.m_Index;
}

// DocumentIterator
DocumentIterator::DocumentIterator(const Document* _Document, int _Index) : 
    m_Document(_Document), 
    m_Index(_Index){}

DocumentIterator::~DocumentIterator(){}

Node DocumentIterator::operator*() const 
{
    if(m_Document == nullptr) 
        return Node();

    const auto& hierarchy = m_Document->hierarchy();    

    return Node(hierarchy.m_Singletones[m_Index]);
}

const NodeInfo* DocumentIterator::operator->() const
{
    if(m_Document == nullptr) 
        return nullptr;

    const auto& hierarchy = m_Document->hierarchy();

    return hierarchy.m_Singletones[m_Index];
}

DocumentIterator& DocumentIterator::operator++() 
{ 
    m_Index++; 
    return *this; 
}

DocumentIterator& DocumentIterator::operator--() 
{ 
    m_Index--; 
    return *this; 
}

DocumentIterator DocumentIterator::operator++(int) 
{ 
    DocumentIterator tmp = *this; 
    ++(*this); 
    return tmp; 
}

DocumentIterator DocumentIterator::operator--(int) 
{ 
    DocumentIterator tmp = *this; 
    --(*this); 
    return tmp; 
}

int DocumentIterator::distance(const DocumentIterator& _First, const DocumentIterator& _Last)
{
    return _Last.m_Index - _First.m_Index;
}

// Node
Node::Node(NodeInfo* _Info) : m_Info(_Info){}

bool Node::is_valid() const
{
    return m_Info != nullptr && m_Info->Document != nullptr;
}

size_t Node::type() const
{
    return m_Info != nullptr ? m_Info->Type : NodeValueType::OBJECT;
}

const char* Node::name() const
{
    return m_Info != nullptr ? m_Info->Name : Node::m_EmptyNode.Name;
}

const char* Node::value() const
{
    return m_Info != nullptr ? m_Info->Value : Node::m_EmptyNode.Value;
}

const NodeIterator Node::begin() const
{
    if(!is_valid()) 
        return NodeIterator(nullptr, -1);

    const auto& hierarchy = 
        m_Info->Document->hierarchy();

    return NodeIterator(*this, hierarchy.m_Pointers[m_Info->Self]);
}

const NodeIterator Node::end() const
{
    if(!is_valid()) 
        return NodeIterator(nullptr, -1);

    const auto& hierarchy = 
        m_Info->Document->hierarchy();

    return NodeIterator(*this, hierarchy.m_Pointers[m_Info->Self + 1]);
}

Node Node::append_node(const char* _Name, const char* _Value, const size_t& _Type)
{
    if(!is_valid()) 
        return Node();

    return Node(m_Info->Document->append_node(_Name, _Value, *this, _Type));
}

void Node::remove_node(std::function<bool(const Node& _Node)> _Predicate)
{
    if(is_valid())
        m_Info->Document->remove_node(_Predicate, *this);
}

void Node::remove_node(const char* _Name)
{
    if(is_valid())
        m_Info->Document->remove_node(_Name);
}

Node Node::find_node(std::function<bool(const Node& _Node)> _Predicate)
{
    return is_valid() ? m_Info->Document->find_node(_Predicate, *this) : Node();
}

Node Node::find_node(const char* _Name)
{
    return is_valid() ? m_Info->Document->find_node(_Name, *this) : Node(); 
}

// Document
Node Document::append_node(const char* _Name, const char* _Value, const Node& _Parent, const size_t& _Type) const
{
    NodeInfo* node   = m_NodeAllocator.PolymorphicAllocator.allocate(1);
    node->Name       = m_StringAllocator.copy(_Name);
    node->Value      = m_StringAllocator.copy(_Value);
    node->Self       = m_Nodes.size();
    node->Parent     = _Parent.m_Info;
    node->Document   = this;
    node->Type       = _Type;
    
    m_Nodes.push_back(node);
    m_Hierarchy.m_IsDirty = true;

    return Node(node);
}

void Document::remove_node(std::function<bool(const Node& _Node)> _Predicate, const Node& _Parent) const
{
    if(_Predicate == nullptr) 
        return;

    Node toBeRemoved;

    if(!_Parent.is_valid())
    {
        for(auto&& child : *this)
        {
            if(_Predicate(child)) 
                toBeRemoved = child;
        }
    }
    else
    {
        for(auto&& child : _Parent)
        {
            if(_Predicate(child)) 
                toBeRemoved = child;
        }
    }

    if(!toBeRemoved.is_valid()) 
        return;

    // TODO: this must be a recursive !!!
    Helpers::Queue<Node> queue;
    queue.push(toBeRemoved);

    size_t minIndex = toBeRemoved.m_Info->Self;
    size_t maxIndex = toBeRemoved.m_Info->Self;

    while (!queue.empty())
    {
        auto data = queue.front();
        queue.pop();

        maxIndex = std::max(maxIndex, data.m_Info->Self);

        for(auto&& child : data)
            queue.push(child);

        // deallocate node
        m_StringAllocator.PolymorphicAllocator.deallocate(data.m_Info->Name);
        m_StringAllocator.PolymorphicAllocator.deallocate(data.m_Info->Value);
        m_NodeAllocator.PolymorphicAllocator.deallocate(data.m_Info);
    }

    // erase a range of nodes from list
    m_Nodes.erase(
        m_Nodes.begin() + std::max<int>((int)minIndex - 1, 0), 
        m_Nodes.begin() + std::min<int>((int)maxIndex + 1, (int)m_Nodes.size() - 1));

    // make hierarchy dirty
    m_Hierarchy.m_IsDirty = true;

    // renumber nodes
    for(size_t i = 0; i < m_Nodes.size(); i++) 
        m_Nodes[i]->Self = i;
}

void Document::remove_node(const char* _Name, const Node& _Parent) const
{
    remove_node([_Name](const Node& _Node)->bool{return std::strcmp(_Node.name(), _Name) == 0;}, _Parent);
}

Node Document::find_node(std::function<bool(const Node& _Node)> _Predicate, const Node& _Parent) const
{
    if(_Predicate == nullptr) 
        return Node();

    Node found;

    if(!_Parent.is_valid())
    {
        for(auto&& child : *this)
        {
            if(_Predicate(child)) 
                found = child;
        }
    }
    else
    {
        for(auto&& child : _Parent)
        {
            if(_Predicate(child)) 
                found = child;
        }
    }

    return found;
}

Node Document::find_node(const char* _Name, const Node& _Parent) const
{
    return find_node([_Name](const Node& _Node)->bool{return std::strcmp(_Node.name(), _Name) == 0;}, _Parent);
}

DocumentIterator Document::begin() const
{
    return DocumentIterator(this, 0);
}

DocumentIterator Document::end() const
{
    return DocumentIterator(this, (int)hierarchy().m_Singletones.size());
}

bool Document::empty() const
{
    return m_Nodes.empty();
}

void Document::reset()
{
    m_Nodes.clear();
    m_NodeAllocator.PolymorphicAllocator.release();
    m_StringAllocator.PolymorphicAllocator.release();
    m_Hierarchy.m_IsDirty = true;
}

#define __support_scalar__(__type)\
template<> Node Document::append_node(const char* _Name, const __type& _Value, const Node& _Parent) const\
{\
    if(Helpers::is_number(_Value))\
    {\
        return Node(append_node(_Name, Helpers::to_string<__type>(_Value).c_str(), _Parent.m_Info, NodeValueType::NUMBER));\
    }\
    \
    if(Helpers::is_bool(_Value))\
    {\
        return Node(append_node(_Name, Helpers::to_string<__type>(_Value).c_str(), _Parent.m_Info, NodeValueType::BOOL));\
    }\
    \
    if(Helpers::is_null(_Value))\
    {\
        return Node(append_node(_Name, Helpers::to_string<__type>(_Value).c_str(), _Parent.m_Info, NodeValueType::NULLPTR));\
    }\
    return Node(append_node(_Name, Helpers::to_string<__type>(_Value).c_str(), _Parent.m_Info, NodeValueType::OBJECT));\
}\

#define __support_vector__(__type)\
template<> Node Document::append_node(const char* _Name, const std::vector<__type>& _Values, const Node& _Parent) const\
{\
    auto container = Document::append_node(_Name, STRINGIFY(std::vector<__type>), _Parent, NodeValueType::ARRAY);\
    for(auto&& value : _Values) append_node<__type>("item", value, container);\
    return container;\
}\

#define __support_list__(__type)\
template<> Node Document::append_node(const char* _Name, const std::list<__type>& _Values, const Node& _Parent) const\
{\
    auto container = Document::append_node(_Name, STRINGIFY(std::list<__type>), _Parent, NodeValueType::ARRAY);\
    for(auto&& value : _Values) append_node<__type>("item", value, container);\
    return container;\
}\

#define __support_set__(__type)\
template<> Node Document::append_node(const char* _Name, const std::set<__type>& _Values, const Node& _Parent) const\
{\
    auto container = Document::append_node(_Name, STRINGIFY(std::set<__type>), _Parent, NodeValueType::ARRAY);\
    for(auto&& value : _Values) append_node<__type>("item", value, container);\
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