#include <FrenchieCoreSerialization.hpp>

using namespace Frenchie::Core;

// DocumentNode
DocumentNode::DocumentNode(const std::string& _Name) : 
    m_Name(_Name){}

DocumentNode::~DocumentNode(){}

std::string DocumentNode::get_name() const
{
    return m_Name;
}

std::vector<Reference<DocumentNode>> DocumentNode::get_children() const
{
    if(m_Children.empty()) 
        return std::vector<Reference<DocumentNode>>();
    
    std::vector<Reference<DocumentNode>> children;
    for(auto child : m_Children) 
        children.push_back(Reference<DocumentNode>(child));

    return children;
}

void DocumentNode::set_name(const std::string& _Name) 
{
    m_Name = _Name;
}

Reference<DocumentNode> DocumentNode::append_child(const std::string& _Name)
{
    m_Children.push_back(std::make_unique<DocumentNode>(_Name));
    auto& child = m_Children.back();
    return Reference<DocumentNode>(child);
}

// XMLDocumentWriter
XMLDocumentWriter::XMLDocumentWriter(){}
XMLDocumentWriter::~XMLDocumentWriter(){}

std::shared_ptr<DocumentNode> XMLDocumentWriter::read(const std::filesystem::path& _Path)
{
    // load file
    pugi::xml_document doc;

    if(doc.load_file(_Path.c_str()).status != pugi::xml_parse_status::status_ok)
    {
        if(doc.load_file(&pugi::as_utf8(_Path.wstring())[0]).status != pugi::xml_parse_status::status_ok) 
            return std::make_shared<DocumentNode>("EMPTY");
    }

    if(doc.empty()) 
        return std::make_shared<DocumentNode>("EMPTY");

    // parse file
    struct Element
    {
        pugi::xml_node  node   = pugi::xml_node();
        Reference<DocumentNode> object = nullptr;
    };

    std::shared_ptr<DocumentNode> root = std::make_shared<DocumentNode>(doc.name());
    std::queue<Element>   queue = std::queue<Element>();
    queue.push({doc, root});

    while(!queue.empty())
    {
        auto top = queue.front();
        queue.pop();

        for(auto& element : top.node)
        {
            // bool
            if(std::string(element.attribute("Type").as_string()) == STRINGIFY(bool)) 
            {
                top.object->Properties.insert({element.name(), element.text().as_bool()});
                continue;
            }
            
            // double
            if(std::string(element.attribute("Type").as_string()) == STRINGIFY(double)) 
            {
                top.object->Properties.insert({element.name(), element.text().as_double()});
                continue;
            }
            
            // float
            if(std::string(element.attribute("Type").as_string()) == STRINGIFY(float))
            {
                top.object->Properties.insert({element.name(), element.text().as_float()});
                continue;
            }
            
            // int
            if(std::string(element.attribute("Type").as_string()) == STRINGIFY(int)) 
            {
                top.object->Properties.insert({element.name(), element.text().as_int()});
                continue;
            }
            
            // unsigned int
            if(std::string(element.attribute("Type").as_string()) == STRINGIFY(unsigned int)) 
            {
                top.object->Properties.insert({element.name(), element.text().as_uint()});
                continue;
            }
            
            // std::string
            if(std::string(element.attribute("Type").as_string()) == STRINGIFY(std::string)) 
            {
                top.object->Properties.insert({element.name(), std::string(element.text().as_string())});
                continue;
            }
            
            // Object
            if(std::string(element.attribute("Type").as_string()) == "Object")
            {
                queue.push(
                    {
                        element, 
                        top.object->append_child(element.name())
                    }
                );

                continue;
            }
        }
    }

    return root;
}

bool XMLDocumentWriter::write(const Reference<DocumentNode>& _Node, const std::filesystem::path& _Path)
{
    pugi::xml_document doc;
    write(_Node, doc);
    return doc.save_file(pugi::as_utf8(_Path.wstring()).c_str());
}

void XMLDocumentWriter::write(const Reference<DocumentNode>& _Node, pugi::xml_node& _Document)
{
    if(_Node == nullptr)
        return;

    auto element = _Document.append_child(_Node->get_name().c_str());
    element.append_attribute("Type").set_value("Object");

    for(auto&& property : _Node->Properties)
    {
        auto& type  = property.get_type();
        auto  child = element.append_child();
        child.set_name(property.get_name());

        // bool
        if(type.hash_code() == typeid(bool).hash_code())
        {
            child.append_attribute("Type").set_value(STRINGIFY(bool));
            child.text().set(std::to_string(property.get<bool>()));
            continue;
        }

        // double
        if(type.hash_code() == typeid(double).hash_code())
        {
            child.append_attribute("Type").set_value(STRINGIFY(double));
            child.text().set(std::to_string(property.get<double>()));
            continue;
        }

        // float
        if(type.hash_code() == typeid(float).hash_code())
        {
            child.append_attribute("Type").set_value(STRINGIFY(float));
            child.text().set(std::to_string(property.get<float>()));
            continue;
        }

        // int
        if(type.hash_code() == typeid(int).hash_code())
        {
            child.append_attribute("Type").set_value(STRINGIFY(int));
            child.text().set(std::to_string(property.get<int>()));
            continue;
        }

        // unsigned int
        if(type.hash_code() == typeid(unsigned int).hash_code()) 
        {
            child.append_attribute("Type").set_value(STRINGIFY(unsigned int));
            child.text().set(std::to_string(property.get<unsigned int>()));
            continue;
        }

        // std::string
        if(type.hash_code() == typeid(std::string).hash_code())
        {
            child.append_attribute("Type").set_value(STRINGIFY(std::string));
            child.text().set(property.get<std::string>());
            continue;
        }
    }

    const auto& children = _Node->get_children();

    for(auto&& child : children) 
        write(child, element);
}