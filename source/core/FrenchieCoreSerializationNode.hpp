#pragma once

#include <FrenchieCoreReference.hpp>
#include <FrenchieCoreSingleton.hpp>
#include <FrenchieCoreNonCopyable.hpp>
#include <FrenchieCoreHelpers.hpp>

#include <FrenchieCoreChunkAllocator.hpp>

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
            class NodeIterator;
            class Node;

            enum NodeValueType
            {
                ATTRIBUTE,
                NULLPTR,
                OBJECT,
                STRING,
                NUMBER,
                ARRAY,
                BOOL
            };

            struct NodeInfo final
            {
                char*           Name      = nullptr;
                char*           Value     = nullptr;
                size_t          Self      = 0;
                const NodeInfo* Parent    = nullptr;
                const Document* Document  = nullptr;
                size_t          Type      = NodeValueType::OBJECT;
            };

            struct NodeHierarchy final
            {
                void build(const std::vector<NodeInfo*>& = std::vector<NodeInfo*>());

                // info
                std::vector<NodeInfo*> m_Items       = std::vector<NodeInfo*>();
                std::vector<NodeInfo*> m_Singletones = std::vector<NodeInfo*>();
                std::vector<int>       m_Pointers    = std::vector<int>();
                bool                   m_IsDirty     = true;
            };

            class NodeIterator final
            {
            public:
                NodeIterator(const Node& _Node, int _Index);
                ~NodeIterator();

                Node operator*() const;
                const NodeInfo* operator->() const;
                
                NodeIterator& operator++();
                NodeIterator& operator--();
                NodeIterator  operator++(int);
                NodeIterator  operator--(int);

                friend bool operator==(const NodeIterator& _First, const NodeIterator& _Second)
                { 
                    return _First.m_Index == _Second.m_Index; 
                }

                friend bool operator!=(const NodeIterator& _First, const NodeIterator& _Second)
                { 
                    return _First.m_Index != _Second.m_Index; 
                }

                static int distance(const NodeIterator& _First, const NodeIterator& _Last);

            protected:
                const Document* m_Document;
                int             m_Index;
            };

            class DocumentIterator
            {
            public:
                DocumentIterator(const Document* _Document, int _Index);
                ~DocumentIterator();

                Node operator*() const;
                const NodeInfo* operator->() const;
                
                DocumentIterator& operator++();
                DocumentIterator& operator--();
                DocumentIterator  operator++(int);
                DocumentIterator  operator--(int);

                friend bool operator==(const DocumentIterator& _First, const DocumentIterator& _Second)
                { 
                    return _First.m_Index == _Second.m_Index; 
                }

                friend bool operator!=(const DocumentIterator& _First, const DocumentIterator& _Second)
                { 
                    return _First.m_Index != _Second.m_Index; 
                }

                static int distance(const DocumentIterator& _First, const DocumentIterator& _Last);

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

                NodeInfo* m_Info = nullptr;

                friend class NodeIterator;
                friend class Document;

            public:
                Node(NodeInfo* _Info = nullptr);
                ~Node() = default;
                
                bool is_valid() const;
                bool has_value() const;

                size_t type() const;
                const char* name() const;
                const char* value() const;

                const NodeIterator begin() const;
                const NodeIterator end() const;
                bool empty() const;

                Node append_node(const char* _Name, const char* _Value = "", const size_t& _Type = NodeValueType::OBJECT);

                template<typename T>
                Node append_node(const char* _Name, const T& _Value)
                {
                    if(!is_valid()) 
                        return Node();

                    return Node(m_Info->Document->append_node<T>(_Name, _Value, *this));
                }

                void remove_node(std::function<bool(const Node& _Node)> _Predicate);
                void remove_node(const char* _Name);
                Node find_node(std::function<bool(const Node& _Node)> _Predicate);
                Node find_node(const char* _Name);
            };

            class Document : public NonCopyable
            {
                template<typename T>
                class Allocator
                {
                public:
                    MemoryChunkAllocator<T> ChunkAllocator{4096};

                    T* allocate(const size_t _Count)
                    {
                        return ChunkAllocator.allocate(_Count);
                    }

                    void deallocate(T* _Pointer)
                    {
                        ChunkAllocator.deallocate(_Pointer);
                    }

                    void release()
                    {
                        ChunkAllocator.release();
                    }
                };

                class StringAllocator
                {
                public:
                    Allocator<char> PolymorphicAllocator;

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
                } mutable m_StringAllocator;

            public:

                class NodeAllocator
                {
                public:
                    Allocator<NodeInfo> PolymorphicAllocator;
                } mutable m_NodeAllocator;

                mutable NodeHierarchy          m_Hierarchy;
                mutable std::vector<NodeInfo*> m_Nodes;

                friend class  Node;
                friend class  NodeIterator;
                friend class  DocumentIterator;
                friend struct NodeInfo;

                NodeHierarchy& hierarchy() const
                {
                    if(m_Hierarchy.m_IsDirty) 
                        m_Hierarchy.build(m_Nodes);

                    return m_Hierarchy;
                }

                Document(){}
                ~Document() = default;

                Node append_node(const char* _Name, const char* _Value = "", const Node& _Parent = Node(), const size_t& _Type = NodeValueType::OBJECT)const;

                template<typename T> 
                Node append_node(const char* _Name, const T& _Value, const Node& _Parent = Node()) const;
                
                void remove_node(std::function<bool(const Node& _Node)> _Predicate, const Node& _Parent = Node()) const;
                void remove_node(const char* _Name, const Node& _Parent = Node()) const;
                Node find_node(std::function<bool(const Node& _Node)> _Predicate, const Node& _Parent = Node()) const;
                Node find_node(const char* _Name, const Node& _Parent = Node()) const;

                DocumentIterator begin() const;
                DocumentIterator end() const;

                bool empty() const;
                void reset();

                size_t get_free_memory_amount() const
                {
                    return m_NodeAllocator.PolymorphicAllocator.ChunkAllocator.get_free_memory_amount() + 
                           m_StringAllocator.PolymorphicAllocator.ChunkAllocator.get_free_memory_amount();
                }

                template<typename Reader>
                bool read(const std::filesystem::path& _Path)
                {
                    return Reader::read(this, _Path);
                }

                template<typename Writer>
                bool write(const std::filesystem::path& _Path)
                {
                    return Writer::write(this, _Path);
                }
            };
        }
    }
}