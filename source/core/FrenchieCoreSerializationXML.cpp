#include <FrenchieCoreSerializationXML.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class XMLHelpers
            {
            public:

                void write_document_to_xml(const Reference<Node>& _Node, pugi::xml_node& _Document)
                {
                    if(_Node == nullptr)
                        return;

                    auto node = _Document.append_child(STRINGIFY(Node));
                    node.append_child("Name").text().set(_Node->get_name().c_str());
                    auto value = node.append_child("Value");
                    value.text().set(_Node->get_value_as_string());

                    if(_Node->is_of_type<bool>()) 
                        value.append_attribute("Type").set_value(get_type_id<bool>());
                    else if(_Node->is_of_type<char>()) 
                        value.append_attribute("Type").set_value(get_type_id<char>());
                    else if(_Node->is_of_type<unsigned char>()) 
                        value.append_attribute("Type").set_value(get_type_id<unsigned char>());
                    else if(_Node->is_of_type<short>()) 
                        value.append_attribute("Type").set_value(get_type_id<short>());
                    else if(_Node->is_of_type<unsigned short>()) 
                        value.append_attribute("Type").set_value(get_type_id<unsigned short>());
                    else if(_Node->is_of_type<int>()) 
                        value.append_attribute("Type").set_value(get_type_id<int>());
                    else if(_Node->is_of_type<unsigned int>()) 
                        value.append_attribute("Type").set_value(get_type_id<unsigned int>());
                    else if(_Node->is_of_type<long>()) 
                        value.append_attribute("Type").set_value(get_type_id<long>());
                    else if(_Node->is_of_type<unsigned long>()) 
                        value.append_attribute("Type").set_value(get_type_id<unsigned long>());
                    else if(_Node->is_of_type<long long>()) 
                        value.append_attribute("Type").set_value(get_type_id<long long>());
                    else if(_Node->is_of_type<unsigned long long>()) 
                        value.append_attribute("Type").set_value(get_type_id<unsigned long long>());
                    else if(_Node->is_of_type<float>()) 
                        value.append_attribute("Type").set_value(get_type_id<float>());
                    else if(_Node->is_of_type<double>()) 
                        value.append_attribute("Type").set_value(get_type_id<double>());
                    else if(_Node->is_of_type<long double>()) 
                        value.append_attribute("Type").set_value(get_type_id<long double>());
                    else if(_Node->is_of_type<std::string>()) 
                        value.append_attribute("Type").set_value(get_type_id<std::string>());

                    const auto& children = _Node->get_children();

                    for(auto&& child : children) 
                        write_document_to_xml(child, node);
                }
            };
        }
    }
}

using namespace Frenchie::Core;
using namespace Frenchie::Core::Serialization;

// XML
XML::XML(){}
XML::~XML(){}

std::shared_ptr<Node> XML::read(const std::filesystem::path& _Path)
{
    // load file
    pugi::xml_document doc;

    auto status = doc.load_file(_Path.c_str()).status;

    if(status != pugi::xml_parse_status::status_ok)
    {
        status = doc.load_file(&pugi::as_utf8(_Path.wstring())[0]).status;

        if(status != pugi::xml_parse_status::status_ok) 
        {
            // TODO: add log here !!!
            return std::make_shared<Node>("EMPTY");
        }
    }

    if(doc.empty()) 
    {
        // TODO: add log here !!!
        return std::make_shared<Node>("EMPTY");
    }

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
            if(std::string(element.name()) == STRINGIFY(Node))
            {
            }

            if(std::string(element.attribute("Type").as_string()) == get_type_id<bool>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<char>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<unsigned char>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<short>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<unsigned short>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<int>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<unsigned int>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<long>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<unsigned long>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<long long>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<unsigned long long>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<float>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<double>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<long double>())
            {
            }
            else if(std::string(element.attribute("Type").as_string()) == get_type_id<std::string>())
            {
            }
            else
            {
                auto object = top.object->append_child(element.name());
                queue.push({element, object});
            }
        }
    }

    return root;
}

bool XML::write(const Reference<Node>& _Node, const std::filesystem::path& _Path)
{
    pugi::xml_document doc;
    XMLHelpers().write_document_to_xml(_Node, doc);
    return doc.save_file(pugi::as_utf8(_Path.wstring()).c_str());
}