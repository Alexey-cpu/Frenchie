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

                template<typename Type> 
                void write_value_property(const std::string& _Name, const Type& _Value, pugi::xml_node& _Document)
                {
                    auto child_Document = _Document.append_child();
                    child_Document.set_name(_Name);
                    child_Document.append_attribute("Type").set_value(type_name<Type>());
                    child_Document.text().set(Helpers::to_string(_Value));
                }

                template<typename Value, template<typename> typename Container> 
                void write_vector_property(const std::string& _Name, const Container<Value>& _Vector, pugi::xml_node& _Document)
                {
                    if(_Vector.empty())
                        return;

                    auto child_Document = _Document.append_child();
                    child_Document.set_name(_Name);
                    child_Document.append_attribute("Type").set_value(type_name<Container<Value>>());

                    int  index  = 0;
                    for(auto&& _Document : _Vector)
                    {
                        auto child = child_Document.append_child();
                        child.set_name(fmt::format("item_{}", index++));
                        child.text().set(Helpers::to_string(_Document));
                    }
                }

                template<typename Type>
                void write_property(const Property& _Property, pugi::xml_node& _Document)
                {
                    if(_Property.is_of_type<Type>())
                        write_value_property<Type>(_Property.get_name(), _Property.get<Type>(), _Document);
                    
                    if(_Property.is_of_type<std::vector<Type>>())
                        write_vector_property<Type, std::vector>(_Property.get_name(), _Property.get<std::vector<Type>>(), _Document);
                    
                    if(_Property.is_of_type<std::list<Type>>())
                        write_vector_property<Type, std::list>(_Property.get_name(), _Property.get<std::list<Type>>(), _Document);
                    
                    if(_Property.is_of_type<std::set<Type>>())
                        write_vector_property<Type, std::set>(_Property.get_name(), _Property.get<std::set<Type>>(), _Document);
                }

                template<typename Type> 
                void read_property(pugi::xml_node& _Document, const Reference<Node>& _Node)
                {
                    // read vector
                    if(std::string(_Document.attribute("Type").as_string()) == type_name<std::vector<Type>>())
                    {
                        std::vector<Type> vector;
                        for(auto&& element : _Document) 
                            vector.push_back(Helpers::from_string<Type>(std::string(_Document.text().get())));
                        _Node->Properties.push_back({std::string(_Document.name()), vector});
                        return;
                    }

                    // read list
                    if(std::string(_Document.attribute("Type").as_string()) == type_name<std::list<Type>>())
                    {
                        std::list<Type> vector;
                        for(auto&& element : _Document) 
                            vector.push_back(Helpers::from_string<Type>(std::string(_Document.text().get())));
                        _Node->Properties.push_back({std::string(_Document.name()), vector});
                        return;
                    }

                    // read set
                    if(std::string(_Document.attribute("Type").as_string()) == type_name<std::set<Type>>())
                    {
                        std::set<Type> vector;
                        for(auto&& element : _Document) 
                            vector.insert(Helpers::from_string<Type>(std::string(_Document.text().get())));
                        _Node->Properties.push_back({std::string(_Document.name()), vector});
                        return;
                    }

                    // read value
                    if(std::string(_Document.attribute("Type").as_string()) == type_name<Type>()) 
                    {
                        _Node->Properties.push_back(
                            {
                                std::string(_Document.name()), 
                                Helpers::from_string<Type>(std::string(_Document.text().get()))
                            }
                        );
                    }
                }

                void write_document_to_xml(const Reference<Node>& _Node, pugi::xml_node& _Document)
                {
                    if(_Node == nullptr)
                        return;

                    auto element = _Document.append_child(_Node->get_name().c_str());
                    element.append_attribute("Type").set_value("Object");

                    for(auto&& property : _Node->Properties)
                    {
                        write_property<bool>(property, element);
                        write_property<float>(property, element);
                        write_property<double>(property, element);
                        write_property<short>(property, element);
                        write_property<unsigned short>(property, element);
                        write_property<int>(property, element);
                        write_property<unsigned int>(property, element);
                        write_property<long>(property, element);
                        write_property<unsigned long>(property, element);
                        write_property<long long>(property, element);
                        write_property<unsigned long long>(property, element);
                    }

                    const auto& children = _Node->get_children();

                    for(auto&& child : children) 
                        write_document_to_xml(child, element);
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
            // read property
            XMLHelpers().read_property<bool>(element, top.object);
            
            // go to the next object
            if(std::string(element.attribute("Type").as_string()) == "Object")
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

bool XML::write(const Reference<Node>& _Node, const std::filesystem::path& _Path)
{
    pugi::xml_document doc;
    XMLHelpers().write_document_to_xml(_Node, doc);
    return doc.save_file(pugi::as_utf8(_Path.wstring()).c_str());
}