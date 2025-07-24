#pragma once

#include <FrenchieCoreSerializationNode.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            template<bool Compact = false>
            class XML final
            {
            public:

                // write a file
                struct Element
                {
                    pugi::xml_node document;
                    Node data;
                };

                static bool read(Document& _Document, const std::filesystem::path& _Path)
                {
                    // load file
                    pugi::xml_document doc;
                    auto status = doc.load_file(_Path.c_str()).status;

                    if(status != pugi::xml_parse_status::status_ok)
                    {
                        status = doc.load_file(&pugi::as_utf8(_Path.wstring())[0]).status;

                        if(status != pugi::xml_parse_status::status_ok) 
                            return false;
                    }

                    if(doc.empty()) 
                        return false;

                    // clear self
                    _Document.reset();

                    // parse in depth
                    std::stack<Element> stack;
                    stack.push({doc, nullptr});

                    while(!stack.empty())
                    {
                        auto top = stack.top();
                        stack.pop();

                        if(top.document.children().empty()) 
                            continue;

                        for(auto&& element : top.document)
                        {
                            if(element.children().empty()) 
                                continue;

                            stack.push(
                                {
                                    element, 
                                    _Document.append_child(element.name(), element.text().get(), top.data)
                                }
                            );
                        }
                    }

                    return true;
                }

                static bool write(Document& _Document, const std::filesystem::path& _Path)
                {
                    pugi::xml_document main;
                    Helpers::Queue<Element> queue;
                    queue.push({main, _Document.root()});

                    while (!queue.empty())
                    {
                        auto data = queue.front().data;
                        auto xml  = queue.front().document;
                        queue.pop();

                        auto node = xml.append_child(data.name());
                        node.text().set(data.value());

                        for(auto item : data) 
                            queue.push({node, item});
                    }

                    return Compact ? 
                            main.save_file(pugi::as_utf8(_Path.wstring()).c_str(), "\t", pugi::format_raw) : 
                            main.save_file(pugi::as_utf8(_Path.wstring()).c_str());
                }
            };
        }
    }
}