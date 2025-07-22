#include <FrenchieCoreSerializationXML.hpp>

// STLC
#include <stack>

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
            std::cout << "Could not open file " << _Path << "\n";

            return std::make_shared<Node>("EMPTY");
        }
    }

    if(doc.empty()) 
    {
        // TODO: add log here !!!
        std::cout << "EMPTY file " << _Path << "\n";

        return std::make_shared<Node>("EMPTY");
    }

    std::shared_ptr<Node> root = std::make_shared<Node>(doc.name());
    std::stack<Element, std::vector<Element>> queue;
    queue.push({doc, root.get()});

    while(!queue.empty())
    {
        auto top = queue.top();
        queue.pop();

        for(auto& element : top.node)
        {
            auto object = top.object;
            queue.push({element, object->append_child(element.name())});

            if(std::string(element.attribute("Type").as_string()) == get_type_name<bool>())
            {
                object->value() = element.attribute("Value").as_bool();
                continue;
            }
            
            if(std::string(element.attribute("Type").as_string()) == get_type_name<char>())
            {
                object->value() = (char)element.attribute("Value").as_int();
                continue;
            }
            
            if(std::string(element.attribute("Type").as_string()) == get_type_name<unsigned char>())
            {
                object->value() = (unsigned char)element.attribute("Value").as_uint();
                continue;
            }

            if(std::string(element.attribute("Type").as_string()) == get_type_name<short>())
            {
                object->value() = (short)element.attribute("Value").as_int();
                continue;
            }

            if(std::string(element.attribute("Type").as_string()) == get_type_name<unsigned short>())
            {
                object->value() = (unsigned short)element.attribute("Value").as_int();
                continue;
            }

            if(std::string(element.attribute("Type").as_string()) == get_type_name<int>())
            {
                object->value() = element.attribute("Value").as_int();
                continue;
            }

            if(std::string(element.attribute("Type").as_string()) == get_type_name<unsigned int>())
            {
                object->value() = element.attribute("Value").as_uint();
                continue;
            }

            if(std::string(element.attribute("Type").as_string()) == get_type_name<long>())
            {
                object->value() = (long)element.attribute("Value").as_llong();
                continue;
            }
            
            if(std::string(element.attribute("Type").as_string()) == get_type_name<unsigned long>())
            {
                object->value() = (unsigned long)element.attribute("Value").as_ullong();
                continue;
            }
            
            if(std::string(element.attribute("Type").as_string()) == get_type_name<long long>())
            {
                object->value() = (long)element.attribute("Value").as_llong();
                continue;
            }

            if(std::string(element.attribute("Type").as_string()) == get_type_name<unsigned long long>())
            {
                object->value() = (unsigned long)element.attribute("Value").as_ullong();
                continue;
            }
            
            if(std::string(element.attribute("Type").as_string()) == get_type_name<float>())
            {
                object->value() = (unsigned long)element.attribute("Value").as_float();
                continue;
            }

            if(std::string(element.attribute("Type").as_string()) == get_type_name<double>())
            {
                object->value() = (unsigned long)element.attribute("Value").as_double();
                continue;
            }
            
            if(std::string(element.attribute("Type").as_string()) == get_type_name<long double>())
            {
                object->value() = (long double)element.attribute("Value").as_double();
                continue;
            }
            
            if(std::string(element.attribute("Type").as_string()) == get_type_name<std::string>())
            {
                object->value() = element.attribute("Value").as_string();
                continue;
            }
        }
    }

    return root;
}

bool XML::write(Node* _Node, const std::filesystem::path& _Path)
{
    pugi::xml_document  main;
    std::stack<Element, std::vector<Element>> stack;
    stack.push({main, _Node});

    while (!stack.empty())
    {
        auto object   = stack.top().object;
        auto document = stack.top().node;
        stack.pop();

        auto node = document.append_child(object->name());

        if(object->value().is_of_type<bool>()) 
            node.append_attribute("Type").set_value(get_type_name<bool>());
        else if(object->value().is_of_type<char>()) 
            node.append_attribute("Type").set_value(get_type_name<char>());
        else if(object->value().is_of_type<unsigned char>()) 
            node.append_attribute("Type").set_value(get_type_name<unsigned char>());
        else if(object->value().is_of_type<short>()) 
            node.append_attribute("Type").set_value(get_type_name<short>());
        else if(object->value().is_of_type<unsigned short>()) 
            node.append_attribute("Type").set_value(get_type_name<unsigned short>());
        else if(object->value().is_of_type<int>()) 
            node.append_attribute("Type").set_value(get_type_name<int>());
        else if(object->value().is_of_type<unsigned int>()) 
            node.append_attribute("Type").set_value(get_type_name<unsigned int>());
        else if(object->value().is_of_type<long>()) 
            node.append_attribute("Type").set_value(get_type_name<long>());
        else if(object->value().is_of_type<unsigned long>()) 
            node.append_attribute("Type").set_value(get_type_name<unsigned long>());
        else if(object->value().is_of_type<long long>()) 
            node.append_attribute("Type").set_value(get_type_name<long long>());
        else if(object->value().is_of_type<unsigned long long>()) 
            node.append_attribute("Type").set_value(get_type_name<unsigned long long>());
        else if(object->value().is_of_type<float>()) 
            node.append_attribute("Type").set_value(get_type_name<float>());
        else if(object->value().is_of_type<double>()) 
            node.append_attribute("Type").set_value(get_type_name<double>());
        else if(object->value().is_of_type<long double>()) 
            node.append_attribute("Type").set_value(get_type_name<long double>());
        else if(object->value().is_of_type<std::string>()) 
            node.append_attribute("Type").set_value(get_type_name<std::string>());

        node.append_attribute("Value").set_value(object->value().as_string());

        const auto& children = object->children();

        for(auto&& child : children) 
            stack.push({node, child});
    }

    return main.save_file(pugi::as_utf8(_Path.wstring()).c_str());
}