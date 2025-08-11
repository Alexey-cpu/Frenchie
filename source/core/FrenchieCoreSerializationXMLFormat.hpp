#pragma once

#include <FrenchieCoreSerializationNode.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            template<bool Compact>
            class XML final
            {
            public:

                struct Element
                {
                    pugi::xml_node document;
                    NodeInfo*      data;
                };

                static bool read(Document* _Document, const std::filesystem::path& _Path)
                {
                    // check document pointer
                    if(_Document == nullptr) 
                        return false;

                    // check path
                    if(!std::filesystem::exists(_Path)) 
                        return false;

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
                            return false;
                    }

                    if(document.empty()) 
                        return false;

                    _Document->m_NodeConstructor.reset();

                    // parse in depth
                    Helpers::Stack<Element> stack;
                    stack.push({document, nullptr});

                    while(!stack.empty())
                    {
                        auto top = stack.top();
                        stack.pop();

                        for(auto&& element : top.document)
                        {
                            if(std::strlen(element.name()) <= 0) // don't read nameless (anonymous) tags
                                continue;

                            // append node
                            auto data = _Document->m_NodeConstructor.append_node(
                                element.name(), 
                                element.text().get(), 
                                top.data
                            );

                            // read node attributes
                            if(!element.attributes().empty())
                            {
                                for(auto it = element.attributes_begin(); it != element.attributes_end(); it++)
                                {
                                    _Document->m_NodeConstructor.append_node(
                                        (*it).name(), 
                                        (*it).value(), 
                                        data
                                    )->Attribute = true;
                                }
                            }

                            stack.push({element, data});
                        }
                    }

                    return true;
                }

                static bool write(Document* _Document, const std::filesystem::path& _Path)
                {
                    // check document pointer
                    if(_Document == nullptr) 
                        return false;

                    // check path
                    if(!std::filesystem::exists(_Path.parent_path())) 
                        return false;

                    std::vector<NodeInfo*> singletons = _Document->m_NodeConstructor.singletons();

                    pugi::xml_document main;

                    const auto& tree = _Document->m_NodeConstructor.hierarchy();

                    for(auto&& singleton : singletons)
                    {
                        Helpers::Queue<Element> queue;
                        queue.push({main, singleton});

                        while (!queue.empty())
                        {
                            auto data = queue.front().data;
                            auto xml  = queue.front().document;
                            queue.pop();

                            auto node = xml.append_child(data->Name);
                            node.text().set(data->Value);

                            for (size_t i = tree.m_Pointers[data->Self]; i < tree.m_Pointers[data->Self + 1]; i++)
                            {
                                if(tree.m_Items[i]->Attribute)
                                {
                                    node.append_attribute(tree.m_Items[i]->Name).set_value(tree.m_Items[i]->Value);
                                }
                                else
                                {
                                    queue.push({node, tree.m_Items[i]});
                                }
                            }
                        }
                    }

                    return Compact ? 
                            main.save_file(pugi::as_utf8(_Path.wstring()).c_str(), "\t", pugi::format_raw) : 
                            main.save_file(pugi::as_utf8(_Path.wstring()).c_str());
                }
            };

            typedef XML<false> XML_BEAUTIFUL;
            typedef XML<true>  XML_COMPACT;
        }
    }
}