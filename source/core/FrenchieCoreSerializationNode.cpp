#include <FrenchieCoreSerializationNode.hpp>

#include <stack>
#include <vector>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Serialization;

// Document::Node::Pointer
Document::Node::Pointer::Pointer(const Document* _Document) : Doc(_Document){}
Document::Node::Pointer::~Pointer(){}

// Document::Node::Iterator
Document::Node::Iterator::Iterator(const Document* _Document, int _Index) : m_Document(_Document), m_Index(_Index){}
Document::Node::Iterator::~Iterator(){}

Document::Node Document::Node::Iterator::operator*() const 
{
    if(m_Document == nullptr) 
        return Node();

    const auto& hierarchy = m_Document->hierarchy();

    return hierarchy.items[m_Index];
}

const Document::Node* Document::Node::Iterator::operator->() const
{
    if(m_Document == nullptr) 
        return nullptr;

    const auto& hierarchy = m_Document->hierarchy();

    return &hierarchy.items[m_Index];
}

// prefix
Document::Node::Iterator& Document::Node::Iterator::operator++() 
{ 
    m_Index++; 
    return *this; 
}

Document::Node::Iterator& Document::Node::Iterator::operator--() 
{ 
    m_Index--; 
    return *this; 
}

Document::Node::Iterator Document::Node::Iterator::operator++(int) 
{ 
    Iterator tmp = *this; 
    ++(*this); 
    return tmp; 
}

Document::Node::Iterator Document::Node::Iterator::operator--(int) 
{ 
    Iterator tmp = *this; 
    --(*this); 
    return tmp; 
}

int Document::Node::Iterator::distance(const Document::Node::Iterator& _First, const Document::Node::Iterator& _Last)
{
    return _Last.m_Index - _First.m_Index;
}

// Document::Node
Document::Node::Node(const Document::Node::Pointer* _Pointer) : m_Pointer(_Pointer){}
Document::Node::~Node(){}

const Document::Node::Pointer* Document::Node::data() const
{
    return m_Pointer;
}

const Document::Node::Iterator Document::Node::begin() const
{
    if(!valid()) 
        return Document::Node::Iterator(nullptr, -1);

    const auto& hierarchy = 
        m_Pointer->Doc->hierarchy();

    return Document::Node::Iterator(m_Pointer->Doc, hierarchy.pointers[self()]);
}

const Document::Node::Iterator Document::Node::end() const
{
    if(!valid()) 
        return Document::Node::Iterator(nullptr, -1);

    const auto& hierarchy = 
        m_Pointer->Doc->hierarchy();

    return Document::Node::Iterator(m_Pointer->Doc, hierarchy.pointers[self() + 1]);
}

std::string& Document::Node::name() const
{
    if(valid())
        return m_Pointer->Name;

    Document::Node::EMPTY_STRING = "";
    return Document::Node::EMPTY_STRING;
}

std::string& Document::Node::value() const
{
    if(valid())
        return m_Pointer->Value;

    Document::Node::EMPTY_STRING = "";
    return Document::Node::EMPTY_STRING;
}

Document::Node Document::Node::parent() const
{
    return valid() ? Document::Node(m_Pointer->Parent) : Document::Node();
}

int Document::Node::self() const
{
    return valid() ? m_Pointer->Self : 0; 
}

bool Document::Node::valid() const
{
    return m_Pointer != nullptr && m_Pointer->Doc != nullptr;
}

bool Document::Node::empty() const
{
    return Iterator::distance(begin(), end()) <= 0;
}

int Document::Node::size() const
{
    return Document::Node::Iterator::distance(begin(), end());
}

void Document::Node::clear()
{
    if(m_Pointer == nullptr) 
        return;

    delete m_Pointer;
    m_Pointer = nullptr;
}

Document::Node Document::Node::append_child(const char* _Name, const char* _Value)
{
    return valid() ? m_Pointer->Doc->append_child(_Name, _Value, *this) : Node();
}

// Document::Hierarchy
Document::Hierarchy::Hierarchy(const std::vector<Node>& nodes)
{
    generate(nodes);
}

Document::Hierarchy::~Hierarchy(){}

bool Document::Hierarchy::is_dirty() const
{
    return m_is_dirty || items.empty() || pointers.empty();
}

void Document::Hierarchy::set_dirty()
{
    m_is_dirty = true;
}

void Document::Hierarchy::generate(const std::vector<Document::Node>& nodes)
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

Document::Node Document::root() const
{
    return m_Nodes.empty() ? Document::Node() : m_Nodes[0];
}

Document::Node Document::append_child(const char* _Name, const char* _Value, Document::Node& _Parent) const
{
    // setup dirty flag
    m_Hierarchy.set_dirty();

    // append child
    auto item       = new Node::Pointer(this);
    item->Name      = _Name;
    item->Value     = _Value;
    item->Self      = std::max<int>((int)m_Nodes.size(), 0);
    item->Parent    = _Parent.data();
    
    m_Nodes.push_back(Node(item));
    return m_Nodes.back();
}

void Document::remove_child(std::function<bool(Document::Node&)> _Predicate, Document::Node& _Parent) const
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

Document::Hierarchy& Document::hierarchy() const
{
    if(m_Hierarchy.is_dirty()) 
        m_Hierarchy.generate(m_Nodes);

    return m_Hierarchy;
}