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

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {            
            class Value final
            {
                public:

                typedef std::variant<
                    bool, 
                    char,
                    unsigned char,
                    short, 
                    unsigned short, 
                    int, 
                    unsigned int, 
                    long, 
                    unsigned long,
                    long long,
                    unsigned long long, 
                    float, 
                    double,
                    long double,
                    std::string> Type;

                enum supportedTypes
                {
                    BOOL,
                    CHAR,
                    UNSIGNED_CHAR,
                    SHORT,
                    UNSIGNED_SHORT,
                    INT,
                    UNSIGNED_INT,
                    LONG,
                    UNSIGNED_LONG,
                    LONG_LONG,
                    UNSIGNED_LONG_LONG,
                    FLOAT,
                    DOUBLE,
                    LONG_DOUBLE,
                    STRING
                };

                template<typename T>
                Value(const T& _Value = T()) : m_Value(_Value){}

                Value(const Type& _Value = Type()) : m_Value(_Value){}
                ~Value(){}

                template<typename Type> 
                Type get() const
                {
                    try
                    {
                        return std::get<Type>(m_Value);
                    }
                    catch(...)
                    {
                        return Type();
                    } 
                }

                template<typename Type>
                bool is_of_type() const
                {
                    return std::holds_alternative<Type>(m_Value); 
                }

                template<typename Type> 
                void set(const Type& _Value) const
                {
                    m_Value = Value(_Value);
                }

                std::string as_string() const
                {
                    if(is_of_type<bool>()) 
                        return Helpers::to_string<bool>(get<bool>());

                    if(is_of_type<char>()) 
                        return Helpers::to_string<char>(get<char>());

                    if(is_of_type<unsigned char>()) 
                        return Helpers::to_string<unsigned char>(get<unsigned char>());

                    if(is_of_type<short>()) 
                        return Helpers::to_string<short>(get<short>());

                    if(is_of_type<unsigned short>()) 
                        return Helpers::to_string<unsigned short>(get<unsigned short>());

                    if(is_of_type<int>()) 
                        return Helpers::to_string<int>(get<int>());

                    if(is_of_type<unsigned int>()) 
                        return Helpers::to_string<unsigned int>(get<unsigned int>());

                    if(is_of_type<long>()) 
                        return Helpers::to_string<long>(get<long>());

                    if(is_of_type<unsigned long>()) 
                        return Helpers::to_string<unsigned long>(get<unsigned long>());

                    if(is_of_type<long long>()) 
                        return Helpers::to_string<long long>(get<long long>());

                    if(is_of_type<unsigned long long>()) 
                        return Helpers::to_string<unsigned long long>(get<unsigned long long>());

                    if(is_of_type<float>()) 
                        return Helpers::to_string<float>(get<float>());

                    if(is_of_type<double>()) 
                        return Helpers::to_string<double>(get<double>());

                    if(is_of_type<long double>()) 
                        return Helpers::to_string<long double>(get<long double>());

                    if(is_of_type<std::string>()) 
                        return Helpers::to_string<std::string>(get<std::string>());

                    return std::string();
                }

            protected:
                Type m_Value;
            };

            class Node final
            {
            public:

                Node(const std::string& _Name = std::string(), const Value& _Value = Value());
                ~Node();

                std::string& name() const;
                Value& value();
                const std::vector<Node*>& children() const;
                Node* append_child(const std::string& _Name, const Value& _Value = Value());
                Node* find_child(const std::function<bool(Node*)>& _Predicate, bool _Recursive = true) const;
                size_t size() const;
                bool empty() const;

            protected:

                mutable std::string m_Name   = std::string();
                mutable Value       m_Value  = Value();
                Node*               m_Parent   = nullptr;
                std::vector<Node*>  m_Children = std::vector<Node*>();
            };

            template<typename T>
            class Format
            {
            public:
                Format(){}
                ~Format(){}
                
                static std::shared_ptr<Node> read(const std::filesystem::path& _Path)
                {
                    return T::read(_Path);
                }

                static bool write(Node* _Node, const std::filesystem::path& _Path)
                {
                    return T::write(_Node, _Path);
                }
            };

            template<typename T> 
            size_t get_type_index()
            {
                return Value::Type(T()).index();
            }
        }
    }
}