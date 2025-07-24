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
        // class Value final
        // {
        //     public:

        //     typedef std::variant<
        //         bool, 
        //         char,
        //         unsigned char,
        //         short, 
        //         unsigned short, 
        //         int, 
        //         unsigned int, 
        //         long, 
        //         unsigned long,
        //         long long,
        //         unsigned long long, 
        //         float, 
        //         double,
        //         long double,
        //         std::pmr::string> Type;

        //     enum supportedTypes
        //     {
        //         BOOL,
        //         CHAR,
        //         UNSIGNED_CHAR,
        //         SHORT,
        //         UNSIGNED_SHORT,
        //         INT,
        //         UNSIGNED_INT,
        //         LONG,
        //         UNSIGNED_LONG,
        //         LONG_LONG,
        //         UNSIGNED_LONG_LONG,
        //         FLOAT,
        //         DOUBLE,
        //         LONG_DOUBLE,
        //         STRING
        //     };

        //     template<typename T>
        //     Value(const T& _Value = T()) : m_Value(_Value){}

        //     Value(const Type& _Value = Type()) : m_Value(_Value){}
        //     ~Value(){}

        //     template<typename Type> 
        //     Type get() const
        //     {
        //         try
        //         {
        //             return std::get<Type>(m_Value);
        //         }
        //         catch(...)
        //         {
        //             return Type();
        //         } 
        //     }

        //     template<typename Type>
        //     bool is_of_type() const
        //     {
        //         return std::holds_alternative<Type>(m_Value); 
        //     }

        //     template<typename Type> 
        //     void set(const Type& _Value) const
        //     {
        //         m_Value = Value(_Value);
        //     }

        //     std::string as_string() const
        //     {
        //         if(is_of_type<bool>()) 
        //             return Helpers::to_string<bool>(get<bool>());

        //         if(is_of_type<char>()) 
        //             return Helpers::to_string<char>(get<char>());

        //         if(is_of_type<unsigned char>()) 
        //             return Helpers::to_string<unsigned char>(get<unsigned char>());

        //         if(is_of_type<short>()) 
        //             return Helpers::to_string<short>(get<short>());

        //         if(is_of_type<unsigned short>()) 
        //             return Helpers::to_string<unsigned short>(get<unsigned short>());

        //         if(is_of_type<int>()) 
        //             return Helpers::to_string<int>(get<int>());

        //         if(is_of_type<unsigned int>()) 
        //             return Helpers::to_string<unsigned int>(get<unsigned int>());

        //         if(is_of_type<long>()) 
        //             return Helpers::to_string<long>(get<long>());

        //         if(is_of_type<unsigned long>()) 
        //             return Helpers::to_string<unsigned long>(get<unsigned long>());

        //         if(is_of_type<long long>()) 
        //             return Helpers::to_string<long long>(get<long long>());

        //         if(is_of_type<unsigned long long>()) 
        //             return Helpers::to_string<unsigned long long>(get<unsigned long long>());

        //         if(is_of_type<float>()) 
        //             return Helpers::to_string<float>(get<float>());

        //         if(is_of_type<double>()) 
        //             return Helpers::to_string<double>(get<double>());

        //         if(is_of_type<long double>()) 
        //             return Helpers::to_string<long double>(get<long double>());

        //         if(is_of_type<std::pmr::string>()) 
        //             return Helpers::to_string<std::pmr::string>(get<std::pmr::string>());

        //         return std::string();
        //     }

        // protected:
        //     Type m_Value;
        // };

        namespace Serialization
        {
            class Document;
            class Iterator;
            class Node;

            // nested types
            class Pointer final
            {
            public:

                Pointer(const Document* _Document, std::pmr::polymorphic_allocator<char>& _Allocator);
                ~Pointer();

                mutable std::pmr::string Name;
                mutable std::pmr::string Value;
                mutable int              Self   = 0;
                mutable Pointer*         Parent = nullptr;
                mutable const Document*  Doc    = nullptr;

                friend class Document;
            };

            class Iterator final
            {
            public:
                Iterator(const Document* _Document, int _Index);
                ~Iterator();

                // access
                Node operator*() const;
                const Node* operator->() const;
                
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

            class Node final
            {
            public:

                Node(Pointer* _Pointer = nullptr);
                ~Node();

                Pointer* const data() const;
                const Iterator begin() const;
                const Iterator end() const;

                std::pmr::string& name() const;
                std::pmr::string& value() const;
                Node parent() const;
                int  self() const;
                bool valid() const;
                bool empty() const;
                int  size() const;

                Node append_child(const char* _Name, const char* _Value);
                void remove_child(std::function<bool(Node&)> _Predicate);

            private:
                inline static std::pmr::string EMPTY_STRING = "";

                Pointer* m_Pointer = nullptr;

                friend class Document;
            };

            struct Hierarchy final
            {
                Hierarchy(const std::vector<Node>& nodes = std::vector<Node>());
                ~Hierarchy();

                bool is_dirty() const;
                void set_dirty();
                void build(const std::vector<Node>& nodes = std::vector<Node>());

                // info
                std::vector<Node> items      = std::vector<Node>();
                std::vector<int>  pointers   = std::vector<int>();
                bool              m_is_dirty = true;
            };

            class Document final
            {
            public:

                Document();
                ~Document();

                Node root() const;
                Node append_child(const char* _Name, const char* _Value, Node& _Parent = Node()) const;
                void remove_child(std::function<bool(Node&)> _Predicate, Node& _Parent = Node()) const;
                void reset();
                Hierarchy& hierarchy() const;

            protected:

                // info
                mutable std::vector<Node> m_Nodes;
                mutable Hierarchy         m_Hierarchy;

                struct StringsAllocator
                {
                    std::array<std::byte, 4096>           Buffer;
                    std::pmr::monotonic_buffer_resource   MonotonicBufferResource{Buffer.data(), Buffer.size()};
                    std::pmr::polymorphic_allocator<char> PolymorphicAllocator{&MonotonicBufferResource};
                } mutable m_StringsAllocatorAllocator;

                struct NodesAllocator
                {
                    std::array<std::byte, 4096>              Buffer;
                    std::pmr::monotonic_buffer_resource      MonotonicBufferResource{Buffer.data(), Buffer.size()};
                    std::pmr::polymorphic_allocator<Pointer> PolymorphicAllocator{&MonotonicBufferResource};

                    Pointer* construct(
                        const Document*                        _Document, 
                        std::pmr::polymorphic_allocator<char>& _Allocator)
                    {
                        Pointer* pointer = 
                            PolymorphicAllocator.allocate(1);

                        pointer = new(pointer)Pointer(_Document, _Allocator);

                        return pointer;
                    }

                    void destroy(Pointer* const _Object)
                    {
                        if(_Object == nullptr) 
                            return;

                        _Object->~Pointer();
                        PolymorphicAllocator.deallocate(_Object, 1);
                    }

                } mutable m_NodesAllocator;
            };

            template<typename T>
            class Format final
            {
            public:
                
                static bool read(Document& _Document, const std::filesystem::path& _Path)
                {
                    return T::read(_Document, _Path);
                }

                static bool write(Document& _Document, const std::filesystem::path& _Path)
                {
                    return T::write(_Document, _Path);
                }
            };
        }
    }
}