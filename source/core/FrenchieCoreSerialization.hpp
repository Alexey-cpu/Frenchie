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
            // Utility
            template <typename T> inline const char* type_name() { return "unsupported"; }
            template <typename T> inline const bool  is_vector() { return false; }
            template <typename T> inline const bool  is_list() { return false; }
            template <typename T> inline const bool  is_set() { return false; }

            #define __frenchie_core_support_serialization_of__(__type) \
            template <> inline const char* type_name<__type>() {return STRINGIFY(__type);} \
            template <> inline const char* type_name<std::vector<__type>>() {return STRINGIFY(std::vector<__type>);} \
            template <> inline const bool  is_vector<std::vector<__type>>() { return true; } \
            template <> inline const char* type_name<std::list<__type>>() {return STRINGIFY(std::list<__type>);} \
            template <> inline const bool  is_list<std::list<__type>>() { return true; } \
            template <> inline const char* type_name<std::set<__type>>() {return STRINGIFY(std::set<__type>);} \
            template <> inline const bool  is_set<std::set<__type>>() { return true; } \

            __frenchie_core_support_serialization_of__(bool)
            __frenchie_core_support_serialization_of__(char)
            __frenchie_core_support_serialization_of__(short)
            __frenchie_core_support_serialization_of__(unsigned short)
            __frenchie_core_support_serialization_of__(int)
            __frenchie_core_support_serialization_of__(unsigned int)
            __frenchie_core_support_serialization_of__(long)
            __frenchie_core_support_serialization_of__(unsigned long)
            __frenchie_core_support_serialization_of__(long long)
            __frenchie_core_support_serialization_of__(unsigned long long)
            __frenchie_core_support_serialization_of__(float)
            __frenchie_core_support_serialization_of__(double)
            __frenchie_core_support_serialization_of__(std::string)

            #undef __frenchie_core_support_serialization_of__

            // Property
            struct Property
            {
                Property(const std::string& _Name, const std::any& _Value) :  
                    m_Name(_Name), m_Value(_Value){}

                std::string get_name() const
                {
                    return m_Name;
                }

                template<typename Type> 
                Type get() const
                {
                    try
                    {
                        return std::any_cast<Type>(m_Value);
                    }
                    catch(...)
                    {
                        return Type();
                    } 
                }

                template<typename Type> 
                void set(const Type& _Value) const
                {
                    m_Value = _Value;
                }

                template<typename Type>
                bool is_of_type() const
                {
                    return m_Value.type().hash_code() == typeid(Type).hash_code(); 
                }

            private:
                const   std::string m_Name;
                mutable std::any    m_Value;
            };

            class Node
            {
            public:

                Node(const std::string& _Name);
                virtual ~Node();

                // getters
                std::string get_name() const;
                std::vector<Reference<Node>> get_children() const;

                // setters
                void set_name(const std::string& _Name);

                // API
                Reference<Node> append_child(const std::string& _Name);

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

                std::vector<Property> Properties = std::vector<Property>();

            protected:

                std::string m_Name = std::string();

                std::vector<std::shared_ptr<Node>> m_Children = 
                    std::vector<std::shared_ptr<Node>>();
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