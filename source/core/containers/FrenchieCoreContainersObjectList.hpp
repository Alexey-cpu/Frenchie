#pragma once

#include <FrenchieCoreMemoryDefaultAllocator.hpp>
#include <FrenchieCoreNonCopyable.hpp>

#include <iostream>

namespace Frenchie
{
    namespace Core
    {
        namespace Containers
        {
            template<typename NodeType, typename AllocatorType = Frenchie::Core::Memory::DefaultAllocator<NodeType>>
            class ObjectTreeLeaf : public Frenchie::Core::NonCopyable
            {
            public:

                ObjectTreeLeaf(AllocatorType* _Allocator = nullptr) : Allocator(_Allocator){}

                virtual ~ObjectTreeLeaf()
                {
                    // remove allocator
                    if(Allocator != nullptr)
                    {
                        std::cout << "removing allocator... \n";
                        delete Allocator;
                    }
                }

                // info
                const ObjectTreeLeaf<NodeType, AllocatorType>* Root       {this   };
                mutable NodeType*                              NextSibling{nullptr};
                mutable NodeType*                              PrevSibling{nullptr};
                mutable NodeType*                              FirstChild {nullptr};
                mutable NodeType*                              LastChild  {nullptr};
                mutable AllocatorType*                         Allocator  {nullptr};

            };

            template<typename NodeType, typename AllocatorType = Frenchie::Core::Memory::DefaultAllocator<NodeType>>
            class ObjectTreeNode : public ObjectTreeLeaf<NodeType, AllocatorType>
            {
            public:

                ObjectTreeNode(AllocatorType* _Allocator = nullptr) : ObjectTreeLeaf<NodeType, AllocatorType>(_Allocator){}
                virtual ~ObjectTreeNode()
                {
                    // remove all children
                    clear();
                }

                // nested types
                class Iterator final
                {
                public:
                    Iterator(NodeType* _Node) : m_Info(_Node){}
                    ~Iterator(){}

                    NodeType* operator*() const
                    {
                        return m_Info;
                    }

                    const NodeType** operator->() const
                    {
                        return &m_Info;
                    }
                    
                    Iterator& operator++()
                    {
                        if(m_Info != nullptr) 
                            m_Info = m_Info->NextSibling;
                        return *this;
                    }
                    
                    Iterator& operator--()
                    {
                        if(m_Info != nullptr) 
                            m_Info = m_Info->PrevSibling;
                        return *this;
                    }
                    
                    Iterator  operator++(int)
                    {
                        auto tmp = *this; 
                        ++(*this); 
                        return tmp;
                    }
                    
                    Iterator  operator--(int)
                    {
                        auto tmp = *this; 
                        --(*this); 
                        return tmp;
                    }

                    friend bool operator==(const Iterator& _First, const Iterator& _Second)
                    { 
                        return _First.m_Info == _Second.m_Info; 
                    }

                    friend bool operator!=(const Iterator& _First, const Iterator& _Second)
                    { 
                        return _First.m_Info != _Second.m_Info; 
                    }

                    NodeType* m_Info;
                };

                template<typename ...Args>
                Iterator insert_after(const Iterator& _After, Args ... _Args)
                {
                    return Iterator(raw_memory_insert_after(_After.m_Info, _Args ...));
                }

                template<typename ...Args>
                Iterator insert_before(const Iterator& _Before, Args ... _Args)
                {
                    return Iterator(raw_memory_insert_before(_Before.m_Info, _Args ...));
                }

                Iterator begin() const
                {
                    return Iterator(this->FirstChild);
                }

                Iterator end() const
                {
                    return Iterator(nullptr);
                }

                // API
                int size() const
                {
                    int count = 0;
                    auto NextSibling = this->FirstChild;
                    while (NextSibling) ++count;
                    return count;
                }

                void clear() const
                {
                    // remove all
                    auto NextSibling = this->FirstChild;

                    while (NextSibling)
                    {
                        auto current = NextSibling;
                        NextSibling = NextSibling->NextSibling;

                        if(current != nullptr)
                            remove_node(current);
                    }

                    // clean up LastChild and FirstChild
                    this->FirstChild = nullptr;
                    this->LastChild = nullptr;
                }

                bool empty() const
                {
                    return this->LastChild == nullptr && this->FirstChild == nullptr;
                }

                NodeType* raw_memory_first() const
                {
                    return this->FirstChild;
                }

                NodeType* raw_memory_last() const
                {
                    return this->LastChild;
                }

                template<typename ...Args>
                NodeType* raw_memory_insert_after(NodeType* _Node, Args ... _Args) const
                {
                    // create node
                    NodeType* _What = create_node(_Args ...);

                    if(_What == nullptr)
                        return nullptr;

                    // insert into an empty list
                    if(this->LastChild == nullptr && this->FirstChild == nullptr)
                    {
                        this->LastChild = _What;
                        this->FirstChild = _What;
                        return _What;
                    }

                    // insert into not empty list
                    if(_Node == nullptr)
                        _Node = this->LastChild;

                    if(_Node->NextSibling != nullptr)
                        _Node->NextSibling->PrevSibling = _What;

                    _What->NextSibling = _Node->NextSibling;
                    _Node->NextSibling = _What;
                    _What->PrevSibling = _Node;

                    if(_What->NextSibling == nullptr)
                        this->LastChild = _What;

                    return _What;
                }

                template<typename ...Args>
                NodeType* raw_memory_insert_before(NodeType* _Node, Args ... _Args) const
                {
                    // create node
                    NodeType* _What = create_node(_Args ...);

                    if(_What == nullptr)
                        return nullptr;

                    // insert into an empty list
                    if(this->LastChild == nullptr && this->FirstChild == nullptr)
                    {
                        this->LastChild = _What;
                        this->FirstChild = _What;
                        return _What;
                    }

                    // insert into not empty list
                    if(_Node == nullptr)
                        _Node = this->FirstChild;

                    if(_Node->PrevSibling != nullptr)
                        _Node->PrevSibling->NextSibling = _What;

                    _What->NextSibling = _Node;
                    _What->PrevSibling = _Node->PrevSibling;
                    _Node->PrevSibling = _What;

                    if(_What->PrevSibling == nullptr)
                        this->FirstChild = _What;

                    return _What;
                }

                void raw_memory_remove(NodeType* _Node) const
                {
                    if(_Node == nullptr || empty())
                        return;

                    // check if FirstChild needs update
                    if(_Node->PrevSibling != nullptr)
                        _Node->PrevSibling->NextSibling = _Node->NextSibling;
                    else
                        this->FirstChild = _Node->NextSibling;
                    
                    // check if LastChild needs update
                    if(_Node->NextSibling != nullptr)
                        _Node->NextSibling->PrevSibling = _Node->PrevSibling;
                    else
                        this->LastChild = _Node->PrevSibling;

                    // remove node
                    remove_node(_Node);
                }

                template<typename ...Args>
                NodeType* create_node(Args ... _Args) const
                {
                    auto node  = this->Root->Allocator->construct(_Args ...);
                    node->Root = this->Root;
                    return node;
                }

                void remove_node(NodeType* _Node) const
                {
                    //--m_Counter;
                    if(_Node != nullptr)
                        this->Root->Allocator->destroy(_Node);
                }
            };

            template<typename NodeType, typename AllocatorType = Frenchie::Core::Memory::DefaultAllocator<NodeType>>
            class ObjectTreeRoot : public ObjectTreeNode<NodeType, AllocatorType>
            {
            public:
                ObjectTreeRoot() : ObjectTreeNode<NodeType, AllocatorType>(new AllocatorType()){}
                virtual ~ObjectTreeRoot(){}
            };
        }
    }
}