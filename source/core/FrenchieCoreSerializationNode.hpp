#pragma once

#include <FrenchieCoreReference.hpp>
#include <FrenchieCoreSingleton.hpp>
#include <FrenchieCoreNonCopyable.hpp>
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
            class NodeIterator;
            class Node;

            struct NodeInfo final
            {
                char*           Name      = nullptr;
                char*           Value     = nullptr;
                size_t          Self      = 0;
                const NodeInfo* Parent    = nullptr;
                const Document* Document  = nullptr;
                bool            Attribute = false;
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

                // access
                Node operator*() const;
                const NodeInfo* operator->() const;
                
                // increments
                NodeIterator& operator++();
                NodeIterator& operator--();
                NodeIterator  operator++(int);
                NodeIterator  operator--(int);

                // comparison
                friend bool operator==(const NodeIterator& _First, const NodeIterator& _Second)
                { 
                    return _First.m_Index == _Second.m_Index; 
                }

                friend bool operator!=(const NodeIterator& _First, const NodeIterator& _Second)
                { 
                    return _First.m_Index != _Second.m_Index; 
                }

                // arithmetics
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

                // access
                Node operator*() const;
                const NodeInfo* operator->() const;
                
                // increments
                DocumentIterator& operator++();
                DocumentIterator& operator--();
                DocumentIterator  operator++(int);
                DocumentIterator  operator--(int);

                // comparison
                friend bool operator==(const DocumentIterator& _First, const DocumentIterator& _Second)
                { 
                    return _First.m_Index == _Second.m_Index; 
                }

                friend bool operator!=(const DocumentIterator& _First, const DocumentIterator& _Second)
                { 
                    return _First.m_Index != _Second.m_Index; 
                }

                // arithmetics
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
                bool is_attribute() const;

                const char* name() const;
                const char* value() const;

                const NodeIterator begin() const;
                const NodeIterator end() const;

                Node append_node(
                    const char* _Name, 
                    const char* _Value, 
                    const bool& _Attribute = false);

                template<typename T>
                Node append_node(const char* _Name, const T& _Value)
                {
                    if(!is_valid()) 
                        return Node();

                    return Node(m_Info->Document->append_node<T>(_Name, _Value, *this));
                }
            };

            class Document : public NonCopyable
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
                } mutable m_StringAllocator;

                class NodeAllocator
                {
                    public:
                    std::array<std::byte, 4096>               Buffer;
                    std::pmr::monotonic_buffer_resource       MonotonicBufferResource{Buffer.data(), Buffer.size()};
                    std::pmr::polymorphic_allocator<NodeInfo> PolymorphicAllocator{&MonotonicBufferResource};
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

            public:

                Document(){}
                ~Document() = default;

                Node append_node(const char* _Name, const char* _Value, const Node& _Parent = Node(), const bool& _Attribute = false) const;

                template<typename T> 
                Node append_node(const char* _Name, const T& _Value, const Node& _Parent = Node()) const;

                // TODO: implement this API
                // void remove_node(std::function<bool(const Node& _Node)> _Predicate);
                // void remove_node(const char* _Name);
                // void find_node(std::function<bool(const Node& _Node)> _Predicate);
                // void find_node(const char* _Name);
                // void find_attribute(const char* _Name);

                DocumentIterator begin() const;
                DocumentIterator end() const;

                bool empty() const;
                void reset(); // TODO: refactor this function when 'remove node' is implemented

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