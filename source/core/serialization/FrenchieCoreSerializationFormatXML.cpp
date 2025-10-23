#include <FrenchieCoreSerializationFormatXML.hpp>

#include <FrenchieCoreLogger.hpp>
#include <FrenchieCoreContainersStack.hpp>
#include <FrenchieCoreContainersQueue.hpp>

// int main(int, char**)
// {
//     auto start = tic();

//     //--------------------------------------------------------------------------------
//     // load a file into a buffer
//     //--------------------------------------------------------------------------------
//     char* path = "C:/SDK/Qt_Projects/OpenGL/logs/TestFile.xml";
//     //char* path = "C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML.xml";

//     std::ifstream t(path);
//     //std::ifstream t("C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML.xml");
//     t.seekg(0, std::ios::end);
//     size_t size = t.tellg();
//     std::string buffer(size, ' ');
//     t.seekg(0);
//     t.read(&buffer[0], size); 

//     //--------------------------------------------------------------------------------
//     // parse buffer
//     //--------------------------------------------------------------------------------    
//     // XML tree
//     char* buff = buffer.data();

//     // stack
//     Stack<char*> tails;

//     // Allocator
//     MemoryChunkAllocator<char> allocator(4096);

//     // constants
//     const char END_OF_FILE               = '\0';
//     const char XML_TAG_START             =  '<';
//     const char XML_TAG_FINISH            =  '>';
//     const char XML_TAG_POP               =  '/';
//     const char XML_ATTRIBUTE_SEPARATOR   =  ' ';
//     const char XML_ATTRIBUTE_VALUE_FRAME =  '"';

//     // walk a tree
//     while(*buff != END_OF_FILE)
//     {
//         if(*buff == XML_TAG_START)
//         {
//             buff++; // next

//             if(*buff == XML_TAG_POP)
//             {
//                 tails.pop();
//             }
//             else
//             {
//                 // push node
//                 tails.push(buff);

//                 // read tag
//                 while(*buff != XML_TAG_FINISH && *buff != XML_TAG_POP) buff++;

//                 // parse tag
//                 // char* tail = tails.top();
//                 // allocator.allocate(buff - tail);

//                 //---------------------------------------------------------------------------------------
//                 // print tag
//                 //---------------------------------------------------------------------------------------
//                 if(buffer.size() < 4096)
//                 {
//                     char* tail = tails.top();

//                     while(tail != buff)
//                     {
//                         std::cout << *tail;
//                         tail++;
//                     }

//                     std::cout << "\n";
//                 }
//                 //---------------------------------------------------------------------------------------
//             }
//         }

//         buff++; // next
//     }
    
//     std::cout << "elapsed " << elapsed<std::chrono::milliseconds>(start, tic()) << " ms \n";

//     // pugi benchmark
//     start = tic();

//     pugi::xml_document doc;
//     doc.load_file(path);

//     std::cout << "pugi elapsed " << elapsed<std::chrono::milliseconds>(start, tic()) << " ms \n";

//     return 0;
// }

using namespace Frenchie::Core;
using namespace Frenchie::Core::Containers;
using namespace Frenchie::Core::FileSystem;
using namespace Frenchie::Core::Serialization;

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class XML final
            {
            public:

                struct Element
                {
                    pugi::xml_node document;
                    Node           data;
                };

                static bool read(Document* _Document, const std::filesystem::path& _Path)
                {
                    // check document pointer
                    if(_Document == nullptr) 
                        return false;

                    // check path
                    if(!std::filesystem::exists(_Path)) 
                    {
                        Frenchie::Core::Logger::instance()->error(
                            fmt::format("{} does not exist", _Path.string())
                        );

                        return false;
                    }

                    auto parse_options = 
                        pugi::parse_embed_pcdata    | 
                        pugi::parse_trim_pcdata     | 
                        pugi::parse_cdata           | 
                        pugi::parse_escapes         | 
                        pugi::parse_wconv_attribute | 
                        pugi::parse_eol;

                    pugi::xml_document document;
                    auto status = document.load_file(_Path.c_str(), parse_options).status;

                    if(status != pugi::xml_parse_status::status_ok)
                    {
                        status = document.load_file(&pugi::as_utf8(_Path.wstring())[0], parse_options).status;

                        if(status != pugi::xml_parse_status::status_ok) 
                        {
                            Frenchie::Core::Logger::instance()->error(
                                fmt::format("{}\n XML parse error '{}'", 
                                    Frenchie::Core::String::as_utf8(_Path.wstring()), 
                                    descript_status(status))
                            );

                            return false;
                        }
                    }

                    if(document.empty()) 
                    {
                        Frenchie::Core::Logger::instance()->error(
                            fmt::format("{}\n empty document", 
                                Frenchie::Core::String::as_utf8(_Path.wstring()))
                            );
                        return false;
                    }

                    // reset document
                    _Document->reset();

                    // parse in depth
                    Stack<Element> stack;
                    stack.push({document, *_Document});

                    while(!stack.empty())
                    {
                        auto top = stack.top();
                        stack.pop();

                        for(auto&& element : top.document)
                        {
                            if(std::strlen(element.name()) <= 0) // don't read nameless (anonymous) tags
                                continue;

                            // append node
                            auto data = top.data.append_node(
                                element.name(), 
                                element.text().get()
                            );

                            // read node attributes
                            if(!element.attributes().empty())
                            {
                                for(auto it = element.attributes_begin(); it != element.attributes_end(); it++)
                                {
                                    data.append_node(
                                        (*it).name(), 
                                        (*it).value(),
                                        NodeType::ATTRIBUTE
                                    );
                                }
                            }

                            stack.push({element, data});
                        }
                    }

                    return true;
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

                    pugi::xml_document main;

                    for(auto&& singleton : *_Document)
                    {
                        Queue<Element> queue;
                        queue.push({main, singleton});

                        while (!queue.empty())
                        {
                            auto data = queue.front().data;
                            auto xml  = queue.front().document;
                            queue.pop();

                            auto node = xml.append_child(data.get_name());
                            node.text().set(data.get_value());

                            for(auto&& child : data)
                            {
                                if(child.get_type() == NodeType::ATTRIBUTE)
                                {
                                    node.append_attribute(child.get_name()).set_value(child.get_value());
                                }
                                else
                                {
                                    queue.push({node, child});
                                }
                            }
                        }
                    }

                    return Compact ? 
                            main.save_file(pugi::as_utf8(_Path.wstring()).c_str(), "\t", pugi::format_raw) : 
                            main.save_file(pugi::as_utf8(_Path.wstring()).c_str());
                }
            
            protected:

                static std::string descript_status(pugi::xml_parse_status _Status)
                {
                    switch (_Status)
                    {
                    case pugi::xml_parse_status::status_file_not_found:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);
                    
                    case pugi::xml_parse_status::status_io_error:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_out_of_memory:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_internal_error:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_unrecognized_tag:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_bad_pi:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_bad_comment:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_bad_cdata:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_bad_doctype:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_bad_pcdata:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_bad_start_element:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_bad_attribute:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_bad_end_element:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_end_element_mismatch:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_append_invalid_root:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);

                    case pugi::xml_parse_status::status_no_document_element:
                        return STRINGIFY(pugi::xml_parse_status::status_file_not_found);
                    }

                    return STRINGIFY(pugi::xml_parse_status::status_ok);
                }
            };
        }
    }
}

bool XMLReader::read(Document* _Document, const std::filesystem::path& _Path)
{
    return XML::read(_Document, _Path);
}

bool XMLCompactWriter::write(Document* _Document, const std::filesystem::path& _Path)
{
    return XML::write<true>(_Document, _Path);
}

bool XMLBeautifulWriter::write(Document* _Document, const std::filesystem::path& _Path)
{
    return XML::write<false>(_Document, _Path);
}