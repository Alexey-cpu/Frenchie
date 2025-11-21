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
            // nested types
            template<typename NodeType>
            class ObjectTreeIterator final
            {
            public:
                ObjectTreeIterator(NodeType* _Node) : m_Info(_Node){}
                ~ObjectTreeIterator(){}

                NodeType* operator*() const
                {
                    return m_Info;
                }

                const NodeType** operator->() const
                {
                    return &m_Info;
                }
                
                ObjectTreeIterator& operator++()
                {
                    if(m_Info != nullptr) 
                        m_Info = m_Info->NextSibling;
                    return *this;
                }
                
                ObjectTreeIterator& operator--()
                {
                    if(m_Info != nullptr) 
                        m_Info = m_Info->PrevSibling;
                    return *this;
                }
                
                ObjectTreeIterator  operator++(int)
                {
                    auto tmp = *this; 
                    ++(*this); 
                    return tmp;
                }
                
                ObjectTreeIterator  operator--(int)
                {
                    auto tmp = *this; 
                    --(*this); 
                    return tmp;
                }

                friend bool operator==(const ObjectTreeIterator& _First, const ObjectTreeIterator& _Second)
                { 
                    return _First.m_Info == _Second.m_Info; 
                }

                friend bool operator!=(const ObjectTreeIterator& _First, const ObjectTreeIterator& _Second)
                { 
                    return _First.m_Info != _Second.m_Info; 
                }

                NodeType* m_Info;
            };

            template<typename NodeType, typename AllocatorType = Frenchie::Core::Memory::DefaultAllocator<NodeType>>
            class ObjectTreeLeaf : public Frenchie::Core::NonCopyable
            {
            public:

                ObjectTreeLeaf(){}

                virtual ~ObjectTreeLeaf()
                {
                    // remove allocator
                    if(Allocator != nullptr)
                        delete Allocator;
                }

                // info
                mutable const ObjectTreeLeaf<NodeType, AllocatorType>* Root       {this   };
                mutable const ObjectTreeLeaf<NodeType, AllocatorType>* Parent     {nullptr};
                mutable NodeType*                                      NextSibling{nullptr};
                mutable NodeType*                                      PrevSibling{nullptr};
                mutable NodeType*                                      FirstChild {nullptr};
                mutable NodeType*                                      LastChild  {nullptr};
                mutable AllocatorType*                                 Allocator  {nullptr};
            };

            template<typename NodeType, typename AllocatorType = Frenchie::Core::Memory::DefaultAllocator<NodeType>>
            class ObjectTreeNode : public ObjectTreeLeaf<NodeType, AllocatorType>
            {
            public:

                ObjectTreeNode(){}

                virtual ~ObjectTreeNode()
                {
                    // remove all children
                    clear();
                }

                ObjectTreeIterator<NodeType> to_iterator(NodeType* _Raw) const
                {
                    return ObjectTreeIterator(_Raw);
                }

                template<typename ...Args>
                ObjectTreeIterator<NodeType> insert_after(const ObjectTreeIterator<NodeType>& _After, Args ... _Args) const
                {
                    return ObjectTreeIterator(raw_memory_insert_after(_After.m_Info, _Args ...));
                }

                template<typename ...Args>
                ObjectTreeIterator<NodeType> insert_before(const ObjectTreeIterator<NodeType>& _Before, Args ... _Args) const
                {
                    return ObjectTreeIterator(raw_memory_insert_before(_Before.m_Info, _Args ...));
                }

                void remove(const ObjectTreeIterator<NodeType>& _What) const
                {
                    raw_memory_remove(_What.m_Info);
                }

                ObjectTreeIterator<NodeType> first() const
                {
                    return ObjectTreeIterator<NodeType>(this->FirstChild);
                }             

                ObjectTreeIterator<NodeType> last() const
                {
                    return ObjectTreeIterator<NodeType>(this->LastChild);
                }

                ObjectTreeIterator<NodeType> begin() const
                {
                    return ObjectTreeIterator<NodeType>(this->FirstChild);
                }

                ObjectTreeIterator<NodeType> end() const
                {
                    return ObjectTreeIterator<NodeType>(nullptr);
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
                    if(Root == this)
                    {
                    }

                    // remove all
                    auto NextSibling = this->FirstChild;

                    while (NextSibling)
                    {
                        auto current = NextSibling;
                        NextSibling  = NextSibling->NextSibling;

                        if(current != nullptr)
                            remove_node(current);
                    }

                    // clean up LastChild and FirstChild
                    this->FirstChild = nullptr;
                    this->LastChild  = nullptr;
                }

                bool empty() const
                {
                    return this->LastChild == nullptr && this->FirstChild == nullptr;
                }

            protected:

                NodeType* raw_memory_begin() const
                {
                    return this->FirstChild;
                }

                NodeType* raw_memory_end() const
                {
                    return nullptr;
                }

                NodeType* raw_memory_next(NodeType* _Node) const
                {
                    return _Node != nullptr ? _Node->NextSibling : nullptr;
                }

                NodeType* raw_memory_prev(NodeType* _Node) const
                {
                    return _Node != nullptr ? _Node->PrevSibling : nullptr;
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
                    raw_memory_attach_after(_Node, _What);
                    return _What;
                }

                template<typename ...Args>
                NodeType* raw_memory_insert_before(NodeType* _Node, Args ... _Args) const
                {
                    // create node
                    NodeType* _What = create_node(_Args ...);
                    raw_memory_attach_before(_Node, _What);
                    return _What;
                }

                void raw_memory_remove(NodeType* _Node) const
                {
                    raw_memory_detach(_Node);
                    remove_node(_Node);
                }

                void raw_memory_attach_after(NodeType* _Where, NodeType* _What) const
                {
                    if(_What == nullptr)
                        return;

                    // insert into an empty list
                    if(this->LastChild == nullptr && this->FirstChild == nullptr)
                    {
                        this->LastChild  = _What;
                        this->FirstChild = _What;
                        return;
                    }

                    // insert into not empty list
                    if(_Where == nullptr)
                        _Where = this->LastChild;

                    if(_Where->NextSibling != nullptr)
                        _Where->NextSibling->PrevSibling = _What;

                    _What->NextSibling = _Where->NextSibling;
                    _Where->NextSibling = _What;
                    _What->PrevSibling = _Where;

                    if(_What->NextSibling == nullptr)
                        this->LastChild = _What;

                    _What->Parent = this;
                }

                void raw_memory_attach_before(NodeType* _Where, NodeType* _What) const
                {
                    if(_What == nullptr)
                        return;

                    // insert into an empty list
                    if(this->LastChild == nullptr && this->FirstChild == nullptr)
                    {
                        this->LastChild  = _What;
                        this->FirstChild = _What;
                        return;
                    }

                    // insert into not empty list
                    if(_Where == nullptr)
                        _Where = this->FirstChild;

                    if(_Where->PrevSibling != nullptr)
                        _Where->PrevSibling->NextSibling = _What;

                    _What->NextSibling = _Where;
                    _What->PrevSibling = _Where->PrevSibling;
                    _Where->PrevSibling = _What;

                    if(_What->PrevSibling == nullptr)
                        this->FirstChild = _What;

                    _What->Parent = this;
                }

                void raw_memory_detach(NodeType* _Node) const
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

                    _Node->Parent = nullptr;
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
                    if(_Node != nullptr)
                        this->Root->Allocator->destroy(_Node);
                }
            };

            template<typename NodeType, typename AllocatorType = Frenchie::Core::Memory::DefaultAllocator<NodeType>>
            class ObjectTreeRoot : public ObjectTreeNode<NodeType, AllocatorType>
            {
            public:
                ObjectTreeRoot()
                {
                    this->Allocator = new AllocatorType();
                }

                virtual ~ObjectTreeRoot()
                {
                }
            };
        }
    }
}