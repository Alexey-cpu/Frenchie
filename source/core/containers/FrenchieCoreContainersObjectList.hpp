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
            class ObjectListNodeIterator final
            {
            public:
                ObjectListNodeIterator(Node* _Node) : m_Node(_Node){}
                ~ObjectListNodeIterator(){}

                Node operator*() const
                {
                    return *m_Node;
                }
                
                const Node* operator->() const
                {
                    return m_Node;
                }
                
                ObjectListNodeIterator& operator++()
                {
                    if(m_Node != nullptr)
                        m_Node = m_Node->Next;
                    return *this;
                }
                
                ObjectListNodeIterator& operator--()
                {
                    if(m_Node != nullptr)
                        m_Node = m_Node->Prev;
                    return *this;
                }
                
                ObjectListNodeIterator  operator++(int)
                {
                    ObjectListNodeIterator tmp = *this; 
                    ++(*this); 
                    return tmp; 
                }
                
                ObjectListNodeIterator  operator--(int)
                {
                    ObjectListNodeIterator tmp = *this; 
                    --(*this); 
                    return tmp; 
                }

                friend bool operator==(const ObjectListNodeIterator& _First, const ObjectListNodeIterator& _Second)
                { 
                    return _First.m_Node == _Second.m_Node; 
                }

                friend bool operator!=(const ObjectListNodeIterator& _First, const ObjectListNodeIterator& _Second)
                { 
                    return _First.m_Node != _Second.m_Node; 
                }

                Node* m_Node{nullptr};
            };

            template<typename Node, typename Allocator = DefaultAllocator<Node>>
            class ObjectList : public Frenchie::Core::NonCopyable
            {
            public:
                ObjectList(){}

                ~ObjectList()
                {
                    clear();
                }

                ObjectListNodeIterator<Node> begin() const
                {
                    return ObjectListNodeIterator<Node>(m_Tail);
                }

                ObjectListNodeIterator<Node> end() const
                {
                    return ObjectListNodeIterator<Node>(nullptr);
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
                ObjectListNodeIterator<Node> insert_after(const ObjectListNodeIterator<Node>& _Iterator, Args ... _Args)
                {
                    return ObjectListNodeIterator<Node>(insert_after(_Iterator.m_Node, _Args ...));
                }

                template<typename ...Args>
                ObjectListNodeIterator<Node> insert_before(const ObjectListNodeIterator<Node>& _Iterator, Args ... _Args)
                {
                    return ObjectListNodeIterator<Node>(insert_before(_Iterator.m_Node, _Args ...));
                }

                void remove(const ObjectListNodeIterator<Node>& _Iterator)
                {
                    remove(_Iterator.m_Node);
                }

            protected:

                template<typename ...Args>
                Node* insert_after(Node* _Node, Args ... _Args)
                {
                    // create node
                    Node* _What = create_node(_Args ...);

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
                Node* insert_before(Node* _Node, Args ... _Args)
                {
                    // create node
                    Node* _What = create_node(_Args ...);

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

                void remove(Node* _Node) const
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
                    return m_Allocator.construct(_Args ...);
                }

                void remove_node(Node* _Node)
                {
                    if(_Node != nullptr)
                        m_Allocator.destroy(_Node);
                }

            protected:
                Node*     m_Tail     {nullptr};
                Node*     m_Head     {nullptr};
                Allocator m_Allocator{Allocator()};
            };
        }
    }
}