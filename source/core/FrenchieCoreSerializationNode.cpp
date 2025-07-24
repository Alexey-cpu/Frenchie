#include <FrenchieCoreSerializationNode.hpp>

#include <stack>
#include <vector>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Serialization;

// Pointer
Pointer::Pointer(const Document* _Document) : Doc(_Document){}
Pointer::~Pointer(){}

// Iterator
Iterator::Iterator(const Document* _Document, int _Index) : m_Document(_Document), m_Index(_Index){}
Iterator::~Iterator(){}

Node Iterator::operator*() const 
{
    if(m_Document == nullptr) 
        return Node();

    const auto& hierarchy = m_Document->hierarchy();

    return hierarchy.items[m_Index];
}

const Node* Iterator::operator->() const
{
    if(m_Document == nullptr) 
        return nullptr;

    const auto& hierarchy = m_Document->hierarchy();

    return &hierarchy.items[m_Index];
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
Node::Node(const Pointer* _Pointer) : m_Pointer(_Pointer){}
Node::~Node(){}

const Pointer* Node::data() const
{
    return m_Pointer;
}

const Iterator Node::begin() const
{
    if(!valid()) 
        return Iterator(nullptr, -1);

    const auto& hierarchy = 
        m_Pointer->Doc->hierarchy();

    return Iterator(m_Pointer->Doc, hierarchy.pointers[self()]);
}

const Iterator Node::end() const
{
    if(!valid()) 
        return Iterator(nullptr, -1);

    const auto& hierarchy = 
        m_Pointer->Doc->hierarchy();

    return Iterator(m_Pointer->Doc, hierarchy.pointers[self() + 1]);
}

std::string& Node::name() const
{
    if(valid())
        return m_Pointer->Name;

    EMPTY_STRING = "";
    return EMPTY_STRING;
}

std::string& Node::value() const
{
    if(valid())
        return m_Pointer->Value;

    EMPTY_STRING = "";
    return EMPTY_STRING;
}

Node Node::parent() const
{
    return valid() ? Node(m_Pointer->Parent) : Node();
}

int Node::self() const
{
    return valid() ? m_Pointer->Self : 0; 
}

bool Node::valid() const
{
    return m_Pointer != nullptr && m_Pointer->Doc != nullptr;
}

bool Node::empty() const
{
    return Iterator::distance(begin(), end()) <= 0;
}

int Node::size() const
{
    return Iterator::distance(begin(), end());
}

void Node::clear()
{
    if(m_Pointer == nullptr) 
        return;

    delete m_Pointer;
    m_Pointer = nullptr;
}

Node Node::append_child(const char* _Name, const char* _Value)
{
    return valid() ? m_Pointer->Doc->append_child(_Name, _Value, *this) : Node();
}

// Hierarchy
Hierarchy::Hierarchy(const std::vector<Node>& nodes)
{
    build(nodes);
}

Hierarchy::~Hierarchy(){}

bool Hierarchy::is_dirty() const
{
    return m_is_dirty || items.empty() || pointers.empty();
}

void Hierarchy::set_dirty()
{
    m_is_dirty = true;
}

void Hierarchy::build(const std::vector<Node>& nodes)
{
    if(nodes.empty()) 
        return;

    items.resize(nodes.size());
    pointers.resize(nodes.size() + 1);
    std::vector<int> workspace(nodes.size() + 1);

    for (size_t i = 0; i < nodes.size(); i++)
    {
        items[i]     = nodes[i];
        pointers [i] = 0;
        workspace[i] = 0;
    }

    pointers[nodes.size()] = 0;
    workspace[nodes.size()] = 0;

    for(auto&& item : nodes) 
    {
        if(!item.parent().valid()) 
            continue;

        pointers[item.parent().self()]++;
        workspace[item.parent().self()]++;
    }

    // cumulative sum
    for( int i = 0, j = 0, k = 0 ; i < nodes.size() + 1; i++ )
    {
        k += workspace[i];
        workspace[i] = j;
        pointers[i] = j;
        j = k;
    }

    // count sort
    for(int i = 0; i < nodes.size(); i++ )
    {
        if(!nodes[i].parent().valid()) 
            continue;

        int index    = workspace[nodes[i].parent().self()]++;
        items[index] = nodes[i];
    }

    m_is_dirty = false;
}

// Document
Document::Document(){}

Document::~Document()
{
    reset();
}

Node Document::root() const
{
    return m_Nodes.empty() ? Node() : m_Nodes[0];
}

Node Document::append_child(const char* _Name, const char* _Value, Node& _Parent) const
{
    // setup dirty flag
    m_Hierarchy.set_dirty();

    // append child
    auto item       = new Pointer(this);
    item->Name      = _Name;
    item->Value     = _Value;
    item->Self      = std::max<int>((int)m_Nodes.size(), 0);
    item->Parent    = _Parent.data();
    
    m_Nodes.push_back(Node(item));
    return m_Nodes.back();
}

void Document::remove_child(std::function<bool(Node&)> _Predicate, Node& _Parent) const
{
    if(_Predicate == nullptr) 
        return;

    // setup dirty flag
    m_Hierarchy.set_dirty();
}

void Document::reset()
{
    // clear
    for(auto&& node : m_Nodes) 
        node.clear();
    m_Nodes.clear();

    // setup dirty flag
    m_Hierarchy.set_dirty();
}

Hierarchy& Document::hierarchy() const
{
    if(m_Hierarchy.is_dirty()) 
        m_Hierarchy.build(m_Nodes);

    return m_Hierarchy;
}