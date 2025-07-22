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
            return std::make_shared<Node>("EMPTY");
        }
    }

    if(doc.empty()) 
    {
        // TODO: add log here !!!
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

            switch (element.attribute("Type").as_int())
            {
            case Value::supportedTypes::BOOL:
            object->value() = element.attribute("Value").as_bool();
                break;
            
            case Value::supportedTypes::DOUBLE:
            object->value() = element.attribute("Value").as_double();
                break;

            case Value::supportedTypes::FLOAT:
            object->value() = element.attribute("Value").as_float();
                break;

            case Value::supportedTypes::INT:
            object->value() = element.attribute("Value").as_int();
                break;

            case Value::supportedTypes::LONG_LONG:
            object->value() = element.attribute("Value").as_llong();
                break;
            
            case Value::supportedTypes::UNSIGNED_INT:
            object->value() = element.attribute("Value").as_uint();
                break;

            case Value::supportedTypes::UNSIGNED_LONG_LONG:
            object->value() = element.attribute("Value").as_ullong();
                break;

            case Value::supportedTypes::STRING:
            object->value() = element.attribute("Value").as_string();
                break;
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
            node.append_attribute("Type").set_value(Value::supportedTypes::BOOL);
        else if(object->value().is_of_type<int>()) 
            node.append_attribute("Type").set_value(Value::supportedTypes::INT);
        else if(object->value().is_of_type<unsigned int>()) 
            node.append_attribute("Type").set_value(Value::supportedTypes::UNSIGNED_INT);
        else if(object->value().is_of_type<long long>()) 
            node.append_attribute("Type").set_value(Value::supportedTypes::LONG_LONG);
        else if(object->value().is_of_type<unsigned long long>()) 
            node.append_attribute("Type").set_value(Value::supportedTypes::UNSIGNED_LONG_LONG);
        else if(object->value().is_of_type<float>()) 
            node.append_attribute("Type").set_value(Value::supportedTypes::FLOAT);
        else if(object->value().is_of_type<double>()) 
            node.append_attribute("Type").set_value(Value::supportedTypes::DOUBLE);
        else if(object->value().is_of_type<std::string>()) 
            node.append_attribute("Type").set_value(Value::supportedTypes::STRING);

        node.append_attribute("Value").set_value(object->value().as_string());

        const auto& children = object->children();

        for(auto&& child : children) 
            stack.push({node, child});
    }

    return main.save_file(pugi::as_utf8(_Path.wstring()).c_str(), "\t", pugi::format_raw);
}