#pragma once

#include <FrenchieCoreMemoryDefaultAllocator.hpp>
#include <FrenchieCoreNonCopyable.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Containers
        {
            template<typename Type>
            class ObjectListNode
            {
            public:
                ObjectListNode(){}
                virtual ~ObjectListNode(){}

                Type* Next{nullptr};
                Type* Prev{nullptr};
            };

            template<typename Node>
            class ObjectListIterator final
            {
            public:
                ObjectListIterator(Node* _Node) : m_Node(_Node){}
                ~ObjectListIterator(){}

                Node operator*() const
                {
                    return *m_Node;
                }
                
                const Node* operator->() const
                {
                    return m_Node;
                }
                
                ObjectListIterator& operator++()
                {
                    if(m_Node != nullptr)
                        m_Node = m_Node->Next;
                    return *this;
                }
                
                ObjectListIterator& operator--()
                {
                    if(m_Node != nullptr)
                        m_Node = m_Node->Prev;
                    return *this;
                }
                
                ObjectListIterator  operator++(int)
                {
                    ObjectListIterator tmp = *this; 
                    ++(*this); 
                    return tmp; 
                }
                
                ObjectListIterator  operator--(int)
                {
                    ObjectListIterator tmp = *this; 
                    --(*this); 
                    return tmp; 
                }

                friend bool operator==(const ObjectListIterator& _First, const ObjectListIterator& _Second)
                { 
                    return _First.m_Node == _Second.m_Node; 
                }

                friend bool operator!=(const ObjectListIterator& _First, const ObjectListIterator& _Second)
                { 
                    return _First.m_Node != _Second.m_Node; 
                }

                Node* m_Node{nullptr};
            };

            template<typename Node, typename Allocator = Frenchie::Core::Memory::DefaultAllocator<Node>>
            class ObjectList : public Frenchie::Core::NonCopyable
            {
            public:
                ObjectList(const Allocator* _Allocator) : m_Allocator(_Allocator){}

                ~ObjectList()
                {
                    clear();
                }

                ObjectListIterator<Node> begin() const
                {
                    return ObjectListIterator<Node>(m_Tail);
                }

                ObjectListIterator<Node> end() const
                {
                    return ObjectListIterator<Node>(nullptr);
                }

                bool empty() const
                {
                    return begin() == end();
                }

                int size() const
                {
                    int count = 0;
                    for (auto&& node : *this)
                        ++count;
                    return count;
                }

                void clear()
                {
                    for(auto it = begin(); it != end();)
                    {
                        auto rm = it;
                        ++it;
                        remove_node(rm.m_Node);

                        if(it == end())
                            break;
                    }
                }

                template<typename ...Args>
                ObjectListIterator<Node> insert_after(const ObjectListIterator<Node>& _Iterator, Args ... _Args)
                {
                    return ObjectListIterator<Node>(raw_memory_insert_after(_Iterator.m_Node, _Args ...));
                }

                template<typename ...Args>
                ObjectListIterator<Node> insert_before(const ObjectListIterator<Node>& _Iterator, Args ... _Args)
                {
                    return ObjectListIterator<Node>(raw_memory_insert_before(_Iterator.m_Node, _Args ...));
                }

                void remove(const ObjectListIterator<Node>& _Iterator)
                {
                    raw_memory_remove(_Iterator.m_Node);
                }

            protected:

                template<typename ...Args>
                Node* raw_memory_insert_after(Node* _Node, Args ... _Args)
                {
                    // create node
                    Node* _What = create_node(_Args ...);

                    if(_What == nullptr)
                        return nullptr;

                    // insert into an empty list
                    if(m_Head == nullptr && m_Tail == nullptr)
                    {
                        m_Head = _What;
                        m_Tail = _What;
                        return _What;
                    }

                    // insert into not empty list
                    if(_Node == nullptr)
                        _Node = m_Head;

                    if(_Node->Next != nullptr)
                        _Node->Next->Prev = _What;

                    _What->Next = _Node->Next;
                    _Node->Next = _What;
                    _What->Prev = _Node;

                    if(_What->Next == nullptr)
                        m_Head = _What;

                    return _What;
                }

                template<typename ...Args>
                Node* raw_memory_insert_before(Node* _Node, Args ... _Args)
                {
                    // create node
                    Node* _What = create_node(_Args ...);

                    if(_What == nullptr)
                        return nullptr;

                    // insert into an empty list
                    if(m_Head == nullptr && m_Tail == nullptr)
                    {
                        m_Head = _What;
                        m_Tail = _What;
                        return _What;
                    }

                    // insert into not empty list
                    if(_Node == nullptr)
                        _Node = m_Tail;

                    if(_Node->Prev != nullptr)
                        _Node->Prev->Next = _What;

                    _What->Next = _Node;
                    _What->Prev = _Node->Prev;
                    _Node->Prev = _What;

                    if(_What->Prev == nullptr)
                        m_Tail = _What;

                    return _What;
                }

                void raw_memory_remove(Node* _Node)
                {
                    if(_Node == nullptr)
                        return;

                    // check if tail needs update
                    if(_Node->Prev != nullptr)
                        _Node->Prev->Next = _Node->Next;
                    else
                        m_Tail = _Node->Next;
                    
                    // check if head needs update
                    if(_Node->Next != nullptr)
                        _Node->Next->Prev = _Node->Prev;
                    else
                        m_Head = _Node->Prev;

                    // remove chunk
                    delete _Node;
                }

                template<typename ...Args>
                Node* create_node(Args ... _Args)
                {
                    return m_Allocator != nullptr ? m_Allocator->construct(_Args ...) : nullptr;
                }

                void remove_node(Node* _Node)
                {
                    if(_Node != nullptr && m_Allocator != nullptr)
                        m_Allocator->destroy(_Node);
                }

            protected:
                Node*            m_Tail     {nullptr};
                Node*            m_Head     {nullptr};
                const Allocator* m_Allocator{nullptr};
            };
        }
    }
}