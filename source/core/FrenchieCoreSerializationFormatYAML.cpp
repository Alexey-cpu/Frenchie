#include <FrenchieCoreSerializationFormatYAML.hpp>

// ryml can be used as a single header, or as a simple library:
#if defined(RYML_SINGLE_HEADER) // using the single header directly in the executable
    #define RYML_SINGLE_HDR_DEFINE_NOW
    #include <ryml_all.hpp>
#elif defined(RYML_SINGLE_HEADER_LIB) // using the single header from a library
    #include <ryml_all.hpp>
#else
    #include <ryml.hpp>
    // <ryml_std.hpp> is needed if interop with std containers is
    // desired; ryml itself does not use any STL container.
    // For this sample, we will be using std interop, so...
    #include <ryml_std.hpp> // optional header, provided for std:: interop
    #include <c4/format.hpp> // needed for the examples below
#endif

#include <fstream>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class YAML final
            {
            public:

                struct Element
                {
                    ryml::NodeRef document;
                    Node          data;
                };

                static bool read(Document* _Document, const std::filesystem::path& _Path)
                {
                    // check document pointer
                    if(_Document == nullptr) 
                        return false;

                    // check path
                    if(!std::filesystem::exists(_Path)) 
                        return false;

                    // open YAML file
                    std::ifstream t(_Path);

                    if(!t.is_open()) 
                        return false;

                    t.seekg(0, std::ios::end);
                    size_t size = t.tellg();
                    std::string buffer(size, ' ');
                    t.seekg(0);
                    t.read(&buffer[0], size);

                    ryml::Tree tree;

                    try
                    {
                        tree = ryml::parse_in_place(ryml::to_substr(buffer));
                    }
                    catch(...)
                    {
                        return false;
                    }

                    if(!tree.rootref().is_map() && !tree.rootref().is_seq())
                    {
                        return false;
                    }

                    // reset document
                    _Document->reset();

                    for(auto&& yaml : tree.rootref().children())
                    {
                        // parse in depth
                        Helpers::Stack<Element> stack;
                        stack.push({yaml, *_Document});

                        while(!stack.empty())
                        {
                            auto document = stack.top().document;
                            auto data     = stack.top().data;
                            stack.pop();

                            if(document.is_val())
                            {
                                auto node = data.append_node(
                                    (document.has_key() && document.key().len > 0 ? &document.key()[0] : ""),
                                    (document.has_val() && document.val().len > 0 ? &document.val()[0] : ""));
                            }
                            if(document.is_map() || document.is_seq())
                            {
                                auto node = data.append_node(
                                    (document.has_key() && document.key().len > 0 ? &document.key()[0] : ""));

                                for(auto&& child : document.children()) 
                                    stack.push({child, node});
                            }
                        }
                    }

                    return true;
                }

                static void write_array(const Node& _Node, ryml::NodeRef& _YAML)
                {
                    if(!_YAML.is_map()) 
                        return;

                    auto array = _YAML.append_child();
                    array |= ryml::KEYSEQ;
                    array.set_key(_Node.get_name());

                    for(auto&& child : _Node)
                    {
                        if(child.get_type() == NodeType::ARRAY)
                        {
                            auto yml = array.append_child();

                            YAML::write_array(child, array);
                        }
                        if(child.get_type() == NodeType::OBJECT)
                        {
                            YAML::write_object(child, array);
                        }
                        else
                        {
                            auto yml = array.append_child();
                            yml |= ryml::VAL;
                            yml.set_val(child.get_value());
                        }
                    }
                }

                static void write_object(const Node& _Node, ryml::NodeRef& _YAML)
                {
                    if(!_YAML.is_map() && !_YAML.is_seq()) 
                        return;

                    ryml::NodeRef root;

                    if(_YAML.is_root())
                    {
                        root = _YAML;
                    }
                    else
                    {
                        root  = _YAML.append_child();
                        root |= ryml::KEYMAP;
                        root.set_key(_Node.get_name());
                    }

                    Helpers::Queue<Element> stack;
                    stack.push({root, _Node});

                    while(!stack.empty())
                    {
                        auto document = stack.front().document;
                        auto data     = stack.front().data;
                        stack.pop();

                        if(!document.is_map()) 
                            continue;

                        if(data.empty())
                        {
                            auto ymlNode = document.append_child();
                            ymlNode |= ryml::KEYVAL;
                            ymlNode.set_key(data.get_name());
                            ymlNode.set_val(data.get_value());
                        }
                        else // data has children
                        {
                            if(data.get_type() == NodeType::ARRAY) // it might be array
                            {
                                write_array(data, document);
                            }
                            else
                            {
                                auto ymlNode = document.append_child();
                                ymlNode |= ryml::KEYMAP;
                                ymlNode.set_key(data.get_name());

                                for(auto&& child : data) 
                                    stack.push({ymlNode, child});
                            }
                        }
                    }
                }

                template<bool Compact>
                static bool write(Document* _Document, const std::filesystem::path& _Path)
                {
                    // check document pointer
                    if(_Document == nullptr) 
                        return false;

                    // check path
                    if(!std::filesystem::exists(_Path.parent_path())) 
                        return false;

                    ryml::Tree tree;
                    ryml::NodeRef root = tree.rootref();
                    root |= ryml::DOCMAP; // mark root as a map

                    auto start = Helpers::tic();

                    YAML::write_object(*_Document, root);

                    // open YAML file
                    FILE* file = Helpers::open_file(_Path.string(), "wb");

                    // check that file has been opened
                    if(file == nullptr) 
                        return false;

                    ryml::emit_yaml(tree, file);
                    fclose(file);

                    return true;
                }
            };
        }
    }
}

using namespace Frenchie::Core;
using namespace Frenchie::Core::Helpers;
using namespace Frenchie::Core::Serialization;

bool YAMLReader::read(Document* _Document, const std::filesystem::path& _Path)
{
    return YAML::read(_Document, _Path);
}

bool YAMLCompactWriter::write(Document* _Document, const std::filesystem::path& _Path)
{
    return YAML::write<true>(_Document, _Path);
}

bool YAMLBeautifulWriter::write(Document* _Document, const std::filesystem::path& _Path)
{
    return YAML::write<false>(_Document, _Path);
}