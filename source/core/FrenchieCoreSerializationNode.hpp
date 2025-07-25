#pragma once

#include <FrenchieCoreReference.hpp>
#include <FrenchieCoreSingleton.hpp>
#include <FrenchieCoreHelpers.hpp>

// PUGIXML
#include "pugixml.hpp"

// FMT
#include <fmt/format.h>

// STL
#include <filesystem>
#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <queue>
#include <stack>
#include <any>
#include <set>
#include <variant>

#include <cstddef>
#include <iostream>
#include <memory>
#include <memory_resource>
#include <vector>
#include <array>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class Document;
            class Iterator;
            class Node;

            struct NodeInfo
            {
                char*     Name   = nullptr;
                char*     Value  = nullptr;
                size_t    Self   = 0;
                NodeInfo* Parent = nullptr;

                enum Type {OBJECT, VALUE, ARRAY} Type = Type::OBJECT;
            };

            class NodeCostructor
            {
                class StringAllocator
                {
                public:
                    std::array<std::byte, 4096>           Buffer;
                    std::pmr::monotonic_buffer_resource   MonotonicBufferResource{Buffer.data(), Buffer.size()};
                    std::pmr::polymorphic_allocator<char> PolymorphicAllocator{&MonotonicBufferResource};

                    inline char* copy(const char* _Source)
                    {
                        size_t length  = strlen(_Source);
                        char*  buffer  = PolymorphicAllocator.allocate(length + 1);
                        buffer[length] = '\0';

                        if(!length) 
                            return buffer;

                        std::memcpy(buffer, _Source, length);
                        return buffer;
                    }
                } m_StringAllocator;

                class NodeAllocator
                {
                    public:
                    std::array<std::byte, 4096>               Buffer;
                    std::pmr::monotonic_buffer_resource       MonotonicBufferResource{Buffer.data(), Buffer.size()};
                    std::pmr::polymorphic_allocator<NodeInfo> PolymorphicAllocator{&MonotonicBufferResource};
                } m_NodeAllocator;

                struct Hierarchy final
                {
                    void build(const std::vector<NodeInfo*>& nodes = std::vector<NodeInfo*>())
                    {
                        if(nodes.empty()) 
                            return;

                        m_Items.resize(nodes.size());
                        m_Pointers.resize(nodes.size() + 1);
                        std::vector<int> workspace(nodes.size() + 1);

                        for (size_t i = 0; i < nodes.size(); i++)
                        {
                            m_Items   [i]     = nodes[i];
                            m_Pointers[i] = 0;
                            workspace [i] = 0;
                        }

                        m_Pointers[nodes.size()] = 0;
                        workspace[nodes.size()] = 0;

                        for(auto&& item : nodes) 
                        {
                            if(!item->Parent) 
                                continue;

                            m_Pointers[item->Parent->Self]++;
                            workspace[item->Parent->Self]++;
                        }

                        // cumulative sum
                        for( int i = 0, j = 0, k = 0 ; i < nodes.size() + 1; i++ )
                        {
                            k += workspace[i];
                            workspace[i] = j;
                            m_Pointers[i] = j;
                            j = k;
                        }

                        // count sort
                        for(int i = 0; i < nodes.size(); i++ )
                        {
                            if(!nodes[i]->Parent) 
                                continue;

                            int index    = workspace[nodes[i]->Parent->Self]++;
                            m_Items[index] = nodes[i];
                        }

                        m_IsDirty = false;
                    }

                    // info
                    std::vector<NodeInfo*> m_Items    = std::vector<NodeInfo*>();
                    std::vector<int>       m_Pointers = std::vector<int>();
                    bool                   m_IsDirty  = true;
                } mutable m_Hierarchy;

                mutable std::vector<NodeInfo*> m_Nodes;

            public:

                NodeCostructor(){}

                ~NodeCostructor(){}

                inline NodeInfo* append_node(const char* _Name, const char* _Value, NodeInfo* _Parent = nullptr)
                {
                    NodeInfo* node = m_NodeAllocator.PolymorphicAllocator.allocate(1);
                    node->Name     = m_StringAllocator.copy(_Name);
                    node->Value    = m_StringAllocator.copy(_Value);
                    node->Self     = m_Nodes.size();
                    node->Parent   = _Parent;
                    
                    m_Nodes.push_back(node);
                    m_Hierarchy.m_IsDirty = true;

                    return node;
                }

                Hierarchy& hierarchy() const
                {
                    if(m_Hierarchy.m_IsDirty) 
                        m_Hierarchy.build(m_Nodes);

                    return m_Hierarchy;
                }

                NodeInfo* first_child() const
                {
                    return m_Nodes[0];
                }

                bool empty() const
                {
                    return m_Nodes.empty();
                }

                std::vector<NodeInfo*> singletons() const
                {
                    std::vector<NodeInfo*> singletons{};

                    for(auto&& node : m_Nodes)
                    {
                        if(node->Parent == nullptr) 
                            singletons.push_back(node);
                    }

                    return singletons;
                }

                void reset()
                {
                    m_Nodes.clear();
                    m_NodeAllocator.MonotonicBufferResource.release();
                    m_StringAllocator.MonotonicBufferResource.release();
                    m_Hierarchy.m_IsDirty = true;
                }
            };

            class Iterator final
            {
            public:
                Iterator(const Document* _Document, int _Index);
                ~Iterator();

                // access
                Node operator*() const;
                const NodeInfo* operator->() const;
                
                // increments
                Iterator& operator++();
                Iterator& operator--();
                Iterator  operator++(int);
                Iterator  operator--(int);

                // comparison
                friend bool operator==(const Iterator& _First, const Iterator& _Second)
                { 
                    return _First.m_Index == _Second.m_Index; 
                }

                friend bool operator!=(const Iterator& _First, const Iterator& _Second)
                { 
                    return _First.m_Index != _Second.m_Index; 
                }

                // arithmetics
                static int distance(const Iterator& _First, const Iterator& _Last);

            protected:
                const Document* m_Document;
                int             m_Index;
            };

            class Node
            {
                inline static struct EmptyNode
                {
                    char Name [1] = {'\0'};
                    char Value[1] = {'\0'};
                } m_EmptyNode;

            public:
                Node(NodeInfo* _Info = nullptr, const Document* _Document = nullptr);
                ~Node() = default;
                
                bool valid() const;

                const char* name() const;
                const char* value() const;

                const Iterator begin() const;
                const Iterator end() const;

                // 
                Node append_node(const char* _Name, const char* _Value);

                NodeInfo*       m_Info     = nullptr;
                const Document* m_Document = nullptr;
            };

            class Document
            {
            public:

                Document(){}
                ~Document() = default;

                Node append_node(const char* _Name, const char* _Value, const Node& _Parent = Node()) const
                {
                    return Node(m_NodeConstructor.append_node(_Name, _Value, _Parent.m_Info), this);
                }

                template<typename T>
                Node append_value(const char* _Name, const T& _Value, const Node& _Parent = Node());

                Node first_child() const
                {
                    return !m_NodeConstructor.empty() ? Node(m_NodeConstructor.first_child(), this) : Node();
                }

                template<typename _Format>
                bool read(const std::filesystem::path& _Path)
                {
                    return _Format::read(this, _Path);
                }

                template<typename _Format>
                bool write(const std::filesystem::path& _Path)
                {
                    return _Format::write(this, _Path);
                }

                mutable NodeCostructor m_NodeConstructor;
            };

            template<typename _Format>
            class Format
            {
            public:

                static bool read(Document* _Document, const std::filesystem::path& _Path)
                {
                    return _Format::read(_Document, _Path);
                }

                static bool write(Document* _Document, const std::filesystem::path& _Path)
                {
                    return _Format::write(_Document, _Path);
                }
            };
        }
    }
}