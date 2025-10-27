#include <FrenchieCoreSerializationNode.hpp>

#include <FrenchieCoreContainersStack.hpp>
#include <FrenchieCoreContainersQueue.hpp>
#include <FrenchieCoreStringConvert.hpp>

// STL
#include <type_traits>
#include <vector>
#include <stack>
#include <set>

using namespace Frenchie::Core;
using namespace Frenchie::Core::String;
using namespace Frenchie::Core::Containers;
using namespace Frenchie::Core::Serialization;


namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            struct NodeInfo final
            {
                char*           Name        = nullptr;
                char*           Value       = nullptr;
                NodeInfo*       Parent      = nullptr;
                const Document* Document    = nullptr;
                size_t          Type        = NodeType::OBJECT;
                NodeInfo*       FirstChild  = nullptr;
                NodeInfo*       LastChild   = nullptr;
                NodeInfo*       NextSibling = nullptr;
                NodeInfo*       PrevSibling = nullptr;
            };

            class StringUtility
            {
            public:

                static char* copy(
                    const char*                       _Source, 
                    const MemoryChunkAllocator<char>& _PolymorphicAllocator)
                {
                    size_t length  = _Source != nullptr ? std::strlen(_Source) : 0;
                    char*  buffer  = _PolymorphicAllocator.allocate(length + 1);
                    buffer[length] = '\0';

                    if(!length) 
                        return buffer;

                    std::memcpy(buffer, _Source, length);
                    return buffer;
                }

                static char* copy(
                    const char*                       _Source,
                    char*                             _Destination, 
                    const MemoryChunkAllocator<char>& _PolymorphicAllocator)
                {
                    size_t sourceLength = std::strlen(_Source);

                    AllocationInfo* info = reinterpret_cast<AllocationInfo*>(_Destination - sizeof(AllocationInfo));
                    size_t destinationLength = info->Amount - sizeof(AllocationInfo);

                    if(sourceLength < destinationLength)
                    {
                        std::memcpy(_Destination, _Source, sourceLength);
                        _Destination[sourceLength] = '\0';
                        return _Destination;
                    }
                    else
                    {
                        _PolymorphicAllocator.deallocate(_Destination);
                        return StringUtility::copy(_Source, _PolymorphicAllocator);
                    }
                }  
            };

            template<typename __type>
            bool is_number()
            {
                return std::is_same<__type, short>::value           || 
                    std::is_same<__type, int>::value                ||
                    std::is_same<__type, long>::value               ||
                    std::is_same<__type, long long>::value          ||
                    std::is_same<__type, unsigned short>::value     ||
                    std::is_same<__type, unsigned int>::value       ||
                    std::is_same<__type, unsigned long>::value      ||
                    std::is_same<__type, unsigned long long>::value ||
                    std::is_same<__type, float>::value              ||
                    std::is_same<__type, double>::value;
            }

            template<typename __type>
            bool is_bool()
            {
                return std::is_same<__type, bool>::value;
            }

            template<typename __type>
            bool is_null()
            {
                return std::is_same<__type, nullptr_t>::value;
            }
        }
    }
}

// NodeIterator
NodeIterator::NodeIterator(const Node& _Node) : 
    m_Info(_Node.m_Info){}

NodeIterator::~NodeIterator(){}

Node NodeIterator::operator*() const 
{
    return Node(m_Info);
}

const NodeInfo* NodeIterator::operator->() const
{
    return m_Info;
}

NodeIterator& NodeIterator::operator++() 
{
    if(m_Info != nullptr) 
        m_Info = m_Info->NextSibling;
    
    return *this; 
}

NodeIterator& NodeIterator::operator--() 
{ 
    if(m_Info != nullptr) 
        m_Info = m_Info->PrevSibling;

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

// Node
Node::Node(NodeInfo* _Info) : m_Info(_Info){}

bool Node::is_valid() const
{
    return m_Info != nullptr && m_Info->Document != nullptr;
}

bool Node::has_value() const
{
    return m_Info != nullptr && std::strlen(m_Info->Value) > 0;
}

Node Node::get_parent() const
{
    return m_Info != nullptr ? Node(m_Info->Parent) : Node();
}

size_t Node::get_type() const
{
    return m_Info != nullptr ? m_Info->Type : NodeType::OBJECT;
}

const char* Node::get_name() const
{
    return m_Info != nullptr ? m_Info->Name : "";
}

const char* Node::get_value() const
{
    return m_Info != nullptr ? m_Info->Value : "";
}

void Node::set_name(const char* _Value)
{
    if(is_valid())
        m_Info->Name = StringUtility::copy(_Value, m_Info->Name, m_Info->Document->m_StringAllocator);
}

void Node::set_value(const char* _Value)
{
    if(is_valid())
        m_Info->Value = StringUtility::copy(_Value, m_Info->Value, m_Info->Document->m_StringAllocator);
}

const NodeIterator Node::begin() const
{
    return is_valid() ? NodeIterator(Node(m_Info->FirstChild)) : end();
}

const NodeIterator Node::end() const
{
    return NodeIterator(Node());
}

bool Node::empty() const
{
    return begin() == end();
}

size_t Node::size() const
{
    size_t size = 0;
    for(auto&& child : *this) 
        size++;

    return size;
}

Node Node::append_node(const char* _Name, const char* _Value, const size_t& _Type) const
{
    if(!is_valid()) 
        return Node();

    const Document* document = m_Info->Document;

    NodeInfo* node    = document->m_NodeAllocator.allocate(1);
    node->Name        = StringUtility::copy(_Name, document->m_StringAllocator);
    node->Value       = StringUtility::copy(_Value, document->m_StringAllocator);
    node->Parent      = m_Info;
    node->Document    = document;
    node->Type        = _Type;
    node->FirstChild  = nullptr;
    node->LastChild   = nullptr;
    node->PrevSibling = nullptr;
    node->NextSibling = nullptr;

    // append to a parent
    if(m_Info)
    {
        if(m_Info->FirstChild == nullptr)
        {
            m_Info->FirstChild = node;
            m_Info->LastChild  = node;
            node->PrevSibling  = nullptr;
            node->NextSibling  = nullptr;
        }
        else
        {
            // go to the end
            auto head = m_Info->LastChild;

            // append node to the end
            head->NextSibling = node;
            node->PrevSibling = head;
            node->NextSibling = nullptr;
            m_Info->LastChild = node;
        }
    }

    return Node(node);
}

#define __support_scalar__(__type)\
template<> Node Node::append_node(const char* _Name, const __type& _Value) const\
{\
    if(is_number<__type>())\
    {\
        return Node(append_node(_Name, to_string<__type>(_Value).c_str(), NodeType::NUMBER));\
    }\
    \
    else if(is_bool<__type>())\
    {\
        return Node(append_node(_Name, to_string<__type>(_Value).c_str(), NodeType::BOOL));\
    }\
    \
    else if(is_null<__type>())\
    {\
        return Node(append_node(_Name, to_string<__type>(_Value).c_str(), NodeType::NULLPTR));\
    }\
    return Node(append_node(_Name, to_string<__type>(_Value).c_str(), NodeType::OBJECT));\
}\

#define __support_vector__(__type)\
template<> Node Node::append_node(const char* _Name, const std::vector<__type>& _Values) const\
{\
    auto container = Node::append_node(_Name, STRINGIFY(std::vector<__type>), NodeType::ARRAY);\
    for(auto&& value : _Values) container.append_node<__type>("item", value);\
    return container;\
}\

#define __support_list__(__type)\
template<> Node Node::append_node(const char* _Name, const std::list<__type>& _Values) const\
{\
    auto container = Node::append_node(_Name, STRINGIFY(std::list<__type>), NodeType::ARRAY);\
    for(auto&& value : _Values) container.append_node<__type>("item", value);\
    return container;\
}\

#define __support_set__(__type)\
template<> Node Node::append_node(const char* _Name, const std::set<__type>& _Values) const\
{\
    auto container = Node::append_node(_Name, STRINGIFY(std::set<__type>), NodeType::ARRAY);\
    for(auto&& value : _Values) container.append_node<__type>("item", value);\
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
__support_scalar__(nullptr_t)

__support_vector__(bool)
__support_vector__(float)
__support_vector__(double)
__support_vector__(int)
__support_vector__(unsigned int)
__support_vector__(long)
__support_vector__(unsigned long)
__support_vector__(long long)
__support_vector__(unsigned long long)
__support_vector__(nullptr_t)

__support_list__(bool)
__support_list__(float)
__support_list__(double)
__support_list__(int)
__support_list__(unsigned int)
__support_list__(long)
__support_list__(unsigned long)
__support_list__(long long)
__support_list__(unsigned long long)
__support_list__(nullptr_t)

__support_set__(bool)
__support_set__(float)
__support_set__(double)
__support_set__(int)
__support_set__(unsigned int)
__support_set__(long)
__support_set__(unsigned long)
__support_set__(long long)
__support_set__(unsigned long long)
__support_set__(nullptr_t)

#undef __support_scalar__
#undef __support_vector__
#undef __support_list__
#undef __support_set__

void Node::remove_node(std::function<bool(const Node& _Node)> _Predicate) const
{
    if(_Predicate == nullptr || !is_valid()) 
        return;

    auto toBeRemoved = find_node(_Predicate);

    if(!toBeRemoved.is_valid()) 
        return;

    // collect deallocated nodes
    auto document = m_Info->Document;
    Stack<Node> stack;
    stack.push(toBeRemoved);

    std::vector<Node> nodesToBeRemoved;

    while(!stack.empty())
    {
        auto top = stack.top();
        stack.pop();

        for(auto&& child : top) 
            stack.push(child);
        
        nodesToBeRemoved.push_back(top);
    }

    // update linken list
    auto info = toBeRemoved.m_Info;

    if(info->PrevSibling != nullptr)
    {
        info->PrevSibling->NextSibling = info->NextSibling;
    }
    else
    {
        if(info->Parent != nullptr)
            info->Parent->FirstChild = info->NextSibling;
    }

    if(info->NextSibling != nullptr) 
        info->NextSibling->PrevSibling = info->PrevSibling;

    // deallocate nodes
    for(auto&& node : nodesToBeRemoved)
    {
        document->m_StringAllocator.deallocate(node.m_Info->Name);
        document->m_StringAllocator.deallocate(node.m_Info->Value);
        document->m_NodeAllocator.deallocate(node.m_Info);
    }
}

void Node::remove_node(const char* _Name) const
{
    remove_node([_Name](const Node& _Node)->bool{return std::strcmp(_Node.get_name(), _Name) == 0;});
}

Node Node::find_node(std::function<bool(const Node& _Node)> _Predicate) const
{
    if(_Predicate == nullptr) 
        return Node();

    Node found;

    for(auto&& child : *this)
    {
        if(_Predicate(child)) 
            found = child;
    }

    return found;
}

Node Node::find_node(const char* _Name) const
{
    return find_node([_Name](const Node& _Node)->bool{return std::strcmp(_Node.get_name(), _Name) == 0;});
}

// Document
Document::Document()
{
    // create new root node
    m_Info              = m_NodeAllocator.allocate(1);
    m_Info->Name        = StringUtility::copy("", m_StringAllocator);
    m_Info->Value       = StringUtility::copy("", m_StringAllocator);
    m_Info->Parent      = nullptr;
    m_Info->Document    = this;
    m_Info->Type        = NodeType::OBJECT;
    m_Info->FirstChild  = nullptr;
    m_Info->LastChild   = nullptr;
    m_Info->PrevSibling = nullptr;
    m_Info->NextSibling = nullptr;
}

size_t Document::get_total_memory_size() const
{
    return m_NodeAllocator.get_total_memory_size() + 
           m_StringAllocator.get_total_memory_size();
}

size_t Document::get_free_memory_amount() const
{
    return m_NodeAllocator.get_free_memory_amount() + 
           m_StringAllocator.get_free_memory_amount();
}

size_t Document::get_busy_memory_amount() const
{
    return m_NodeAllocator.get_busy_memory_amount() + 
           m_StringAllocator.get_busy_memory_amount();
}

void Document::reset()
{
    // clear memory
    m_NodeAllocator.release();
    m_StringAllocator.release();
    
    // create new root node
    m_Info              = m_NodeAllocator.allocate(1);
    m_Info->Name        = StringUtility::copy("", m_StringAllocator);
    m_Info->Value       = StringUtility::copy("", m_StringAllocator);
    m_Info->Parent      = nullptr;
    m_Info->Document    = this;
    m_Info->Type        = NodeType::OBJECT;
    m_Info->FirstChild  = nullptr;
    m_Info->LastChild   = nullptr;
    m_Info->PrevSibling = nullptr;
    m_Info->NextSibling = nullptr;
}