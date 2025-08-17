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
            class  Node;
            class  Document;
            class  NodeIterator;
            struct NodeInfo;

            enum NodeType
            {
                ATTRIBUTE = 0,
                NULLPTR   = 1,
                OBJECT    = 2,
                STRING    = 3,
                NUMBER    = 4,
                ARRAY     = 5,
                BOOL      = 6
            };

            class NodeIterator final
            {
            public:
                NodeIterator(const Node& _Node);
                ~NodeIterator();

                Node operator*() const;
                const NodeInfo* operator->() const;
                
                NodeIterator& operator++();
                NodeIterator& operator--();
                NodeIterator  operator++(int);
                NodeIterator  operator--(int);

                friend bool operator==(const NodeIterator& _First, const NodeIterator& _Second)
                { 
                    return _First.m_Info == _Second.m_Info; 
                }

                friend bool operator!=(const NodeIterator& _First, const NodeIterator& _Second)
                { 
                    return _First.m_Info != _Second.m_Info; 
                }

            protected:
                NodeInfo* m_Info;
            };

            class Node
            {
            public:

                friend class NodeIterator;
                friend class Document;

                Node(NodeInfo* _Info = nullptr);
                ~Node() = default;
                
                // checkers
                bool is_valid() const;
                bool has_value() const;

                // getters
                size_t get_type() const;
                const char* get_name() const;
                const char* get_value() const;

                // setters
                void set_name(const char* _Value);
                void set_value(const char* _Value);

                const NodeIterator begin() const;
                const NodeIterator end() const;
                bool empty() const;

                Node append_node(
                    const char*   _Name, 
                    const char*   _Value = "", 
                    const size_t& _Type  = NodeType::OBJECT) const;

                template<typename T> 
                Node append_node(const char* _Name, const T& _Value) const;
                
                void remove_node(std::function<bool(const Node& _Node)> _Predicate) const;
                void remove_node(const char* _Name) const;
                Node find_node(std::function<bool(const Node& _Node)> _Predicate) const;
                Node find_node(const char* _Name) const;

            private:
                NodeInfo* m_Info = nullptr;
            };

            class Document : public Node, public NonCopyable
            {
            public:

                friend class  Node;
                friend class  NodeIterator;
                friend class  DocumentIterator;
                friend struct NodeInfo;

                Document();
                ~Document() = default;

                size_t get_total_memory_size() const;
                size_t get_free_memory_amount() const;
                size_t get_busy_memory_amount() const;

                void reset();

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

            private:
                MemoryChunkAllocator<char>     m_StringAllocator{128};
                MemoryChunkAllocator<NodeInfo> m_NodeAllocator  {8};
            };
        }
    }
}