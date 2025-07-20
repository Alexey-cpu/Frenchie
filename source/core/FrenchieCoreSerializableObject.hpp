#pragma once

#include <FrenchieCoreReference.hpp>
#include <FrenchieCoreHelpers.hpp>

// PUGIXML
#include "pugixml.hpp"

// STL
#include <filesystem>
#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <any>
#include <set>

namespace Frenchie
{
    namespace Core
    {
        class Node
        {
        public:

            // nested types
            struct Property
            {
                Property(const std::string& _Name, const std::any& _Value) : 
                    m_Name(_Name),
                    m_Value(_Value){}

                std::string get_name() const
                {
                    return m_Name;
                }

                template<typename Type> 
                Type get() const
                {
                    try
                    {
                        return std::any_cast<Type>(m_Value)
                    }
                    catch(...)
                    {
                        return Type();
                    } 
                }

                const type_info& get_type() const
                {
                    return m_Value.type();
                }

                template<typename Type> 
                void set(const Type& _Value) const
                {
                    m_Value = _Value;
                }

                std::string to_string() const
                {
                    // bool
                    try
                    {
                        return std::to_string(std::any_cast<bool>(m_Value));
                    }
                    catch(...){}

                    // float
                    try
                    {
                        return std::to_string(std::any_cast<float>(m_Value));
                    }
                    catch(...){}

                    // double
                    try
                    {
                        return std::to_string(std::any_cast<double>(m_Value));
                    }
                    catch(...){}

                    // int
                    try
                    {
                        return std::to_string(std::any_cast<int>(m_Value));
                    }
                    catch(...){}

                    // string
                    try
                    {
                        return std::any_cast<std::string>(m_Value);
                    }
                    catch(...){}

                    return std::string();
                }

                struct TransparentComparator
                {
                    using is_transparent = Property;

                public:

                    bool operator()(const Property& _A, const Property& _B) const
                    {
                        return  _A.m_Name < _B.m_Name;
                    }
                };

            protected:
                const std::string m_Name;
                std::any m_Value;
            };

            Node(const std::string& _Name) : 
                m_Name(_Name){}
            
            virtual ~Node(){}

            std::string get_name() const
            {
                return m_Name;
            }

            std::vector<Reference<Node>> get_children() const
            {
                if(m_Children.empty()) 
                    return std::vector<Reference<Node>>();
                
                std::vector<Reference<Node>> children;
                for(auto child : m_Children) 
                    children.push_back(Reference<Node>(child));

                return children;
            }

            void set_name(const std::string& _Name) 
            {
                m_Name = _Name;
            }

            Reference<Node> append_child(const std::string& _Name)
            {
                m_Children.push_back(std::make_unique<Node>(_Name));
                auto& child = m_Children.back();
                return Reference<Node>(child);
            }

            std::set<Property, Property::TransparentComparator> Properties = 
                std::set<Property, Property::TransparentComparator>();

        protected:

            std::string m_Name = std::string();

            std::vector<std::shared_ptr<Node>> m_Children = 
                std::vector<std::shared_ptr<Node>>();
        };

        template<typename Type>
        class Format
        {
        public:
            Format(){}
            ~Format(){}

            std::shared_ptr<Node> read(const std::filesystem::path& _Path)
            {
                return Type::read(_Path);
            }

            bool write(std::shared_ptr<Node>& _Node, const std::filesystem::path& _Path)
            {
                return Type::write(_Node);
            }
        };

        class XML : public Format<XML>
        {
        public:
            XML(){}
            ~XML(){}

            static std::shared_ptr<Node> read(const std::filesystem::path& _Path)
            {
                // load file
                pugi::xml_document doc;

                if(doc.load_file(_Path.c_str()).status != pugi::xml_parse_status::status_ok)
                {
                    if(doc.load_file(&pugi::as_utf8(_Path.wstring())[0]).status != pugi::xml_parse_status::status_ok) 
                        return std::make_shared<Node>("EMPTY");
                }

                if(doc.empty()) 
                    return std::make_shared<Node>("EMPTY");

                // parse file
                struct Element
                {
                    pugi::xml_node  node   = pugi::xml_node();
                    Reference<Node> object = nullptr;
                };

                std::shared_ptr<Node> root = std::make_shared<Node>(doc.name());
                std::queue<Element>   queue = std::queue<Element>();
                queue.push({doc, root});

                while(!queue.empty())
                {
                    auto top = queue.front();
                    queue.pop();

                    for(auto& element : top.node)
                    {
                        if(std::string(element.attribute("Type").as_string()) == STRINGIFY(bool)) 
                        {
                            top.object->Properties.insert({element.name(), element.text().as_bool()});
                        }
                        else if(std::string(element.attribute("Type").as_string()) == STRINGIFY(double)) 
                        {
                            top.object->Properties.insert({element.name(), element.text().as_double()});
                        }
                        else if(std::string(element.attribute("Type").as_string()) == STRINGIFY(float))
                        {
                            top.object->Properties.insert({element.name(), element.text().as_float()});
                        }
                        else if(std::string(element.attribute("Type").as_string()) == STRINGIFY(int)) 
                        {
                            top.object->Properties.insert({element.name(), element.text().as_int()});
                        }
                        else if(std::string(element.attribute("Type").as_string()) == STRINGIFY(std::string)) 
                        {
                            top.object->Properties.insert({element.name(), std::string(element.text().as_string())});
                        }
                        else if(std::string(element.attribute("Type").as_string()) == "Object")
                        {
                            queue.push(
                                {
                                    element, 
                                    top.object->append_child(element.name())
                                }
                            );
                        }
                    }
                }

                return root;
            }

            bool write(const Reference<Node>& _Node, const std::filesystem::path& _Path)
            {
                pugi::xml_document doc;
                write(_Node, doc);
                return doc.save_file(pugi::as_utf8(_Path.wstring()).c_str());
            }

        protected:

            void write(const Reference<Node>& _Node, pugi::xml_node& _Document)
            {
                if(_Node == nullptr)
                    return;

                auto element = _Document.append_child(_Node->get_name().c_str());
                element.append_attribute("Type").set_value("Object");

                for(auto&& property : _Node->Properties)
                {
                    auto& type = property.get_type();

                    auto child = element.append_child();
                    child.set_name(property.get_name());
                    child.text().set(property.to_string());

                    if(type.hash_code() == typeid(bool).hash_code())
                    {
                        child.append_attribute("Type").set_value(STRINGIFY(bool));
                    }
                    if(type.hash_code() == typeid(double).hash_code())
                    {
                        child.append_attribute("Type").set_value(STRINGIFY(double));
                    }
                    if(type.hash_code() == typeid(float).hash_code())
                    {
                        child.append_attribute("Type").set_value(STRINGIFY(float));
                    }
                    if(type.hash_code() == typeid(int).hash_code())
                    {
                        child.append_attribute("Type").set_value(STRINGIFY(int));
                    }
                    if(type.hash_code() == typeid(std::string).hash_code())
                    {
                        child.append_attribute("Type").set_value(STRINGIFY(std::string));
                    }
                }

                const auto& children = _Node->get_children();
                for(auto&& child : children) 
                    write(child, element);
            }
        };

    }
}