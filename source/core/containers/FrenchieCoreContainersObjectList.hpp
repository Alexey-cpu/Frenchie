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
            class ObjectListNodeInfo : public Frenchie::Core::NonCopyable
            {
            public:
                ObjectListNodeInfo() : Next(nullptr), Prev(nullptr){}
                virtual ~ObjectListNodeInfo(){}

                Type* Next{nullptr};
                Type* Prev{nullptr};
            };

            template<typename Node, typename Allocator = Frenchie::Core::Memory::DefaultAllocator<Node>>
            class ObjectList : public Frenchie::Core::NonCopyable
            {
            public:
                ObjectList(){}

                virtual ~ObjectList()
                {
                    clear();
                }

                bool empty() const
                {
                    return m_Head == nullptr && m_Tail == nullptr;
                }

                int size() const
                {
                    auto next = m_Tail;
                    while (next) ++count;
                    return count;
                }

                void clear() const
                {
                    // remove all
                    auto next = m_Tail;

                    while (next)
                    {
                        auto current = next;
                        next = next->Next;

                        if(current != nullptr)
                            remove_node(current);
                    }

                    // clean up head and tail
                    m_Tail = nullptr;
                    m_Head = nullptr;
                }

                Node* raw_memory_first() const
                {
                    return m_Tail;
                }

                Node* raw_memory_last() const
                {
                    return m_Head;
                }

                template<typename ...Args>
                Node* raw_memory_insert_after(Node* _Node, Args ... _Args) const
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
                Node* raw_memory_insert_before(Node* _Node, Args ... _Args) const
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

                void raw_memory_remove(Node* _Node) const
                {
                    if(_Node == nullptr || empty())
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

                    // remove node
                    remove_node(_Node);
                }

                template<typename ...Args>
                Node* create_node(Args ... _Args) const
                {
                    //++m_Counter;
                    return m_Allocator.construct(_Args ...);
                }

                void remove_node(Node* _Node) const
                {
                    //--m_Counter;
                    if(_Node != nullptr)
                        m_Allocator.destroy(_Node);
                }

                mutable Node*     m_Tail     {nullptr};
                mutable Node*     m_Head     {nullptr};
                mutable Allocator m_Allocator{Allocator()};
                mutable int m_Counter = 0;
            };
        }
    }
}