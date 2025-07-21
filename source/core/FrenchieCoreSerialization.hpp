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
            template<typename Type> inline std::string get_type_id(){ return "unsupported"; }
            template<> inline std::string get_type_id<bool>(){return STRINGIFY(bool);}
            template<> inline std::string get_type_id<char>(){return STRINGIFY(char);}
            template<> inline std::string get_type_id<short>(){return STRINGIFY(short);}
            template<> inline std::string get_type_id<unsigned short>(){return STRINGIFY(unsigned short);}
            template<> inline std::string get_type_id<int>(){return STRINGIFY(int);}
            template<> inline std::string get_type_id<unsigned int>(){return STRINGIFY(unsigned int);}
            template<> inline std::string get_type_id<long>(){return STRINGIFY(long);}
            template<> inline std::string get_type_id<long long>(){return STRINGIFY(long long);}
            template<> inline std::string get_type_id<unsigned long long>(){return STRINGIFY(unsigned long long);}
            template<> inline std::string get_type_id<float>(){return STRINGIFY(float);}
            template<> inline std::string get_type_id<double>(){return STRINGIFY(double);}
            template<> inline std::string get_type_id<std::string>(){return STRINGIFY(std::string);}

            class Node
            {
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
                    std::string> Value;

            public:

                Node(const std::string& _Name, const Value& _Value = Value()) : 
                    m_Name(_Name), 
                    m_Value(Value(_Value)){}
                
                virtual ~Node(){}

                // getters
                std::string get_name() const;
                std::vector<Reference<Node>> get_children_references() const;
                const std::vector<std::shared_ptr<Node>>& get_children() const;

                template<typename Type> 
                Type get_value() const
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

                // setters
                template<typename Type> 
                void set_value(const Type& _Value) const
                {
                    m_Value = Value(_Value);
                }

                void set_name(const std::string& _Name);
                std::string to_string() const;

                // API
                Reference<Node> append_child(
                    const std::string& _Name, 
                    const Value&       _Value = Value());

                template<bool _Recursive = true>
                Reference<Node> find_child(const std::function<bool(Reference<Node>)>& _Predicate) const
                {
                    if(_Predicate == nullptr) 
                        return Reference<Node>();

                    for(auto&& child : m_Children)
                    {
                        if(_Predicate(child)) 
                            return Reference<Node>(child);

                        child->find_child(_Predicate);
                    }

                    return Reference<Node>();
                }

            protected:

                mutable std::string m_Name = std::string();

                std::vector<std::shared_ptr<Node>> m_Children = 
                    std::vector<std::shared_ptr<Node>>();
                
                mutable Value m_Value;

                friend class Format;
            };

            class Format
            {
            public:
                Format(){}
                virtual ~Format(){}
                virtual std::shared_ptr<Node> read(const std::filesystem::path& _Path) = 0;
                virtual bool write(const Reference<Node>& _Node, const std::filesystem::path& _Path) = 0;
            };
        }
    }
}