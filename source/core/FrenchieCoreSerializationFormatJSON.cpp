#include <FrenchieCoreSerializationFormatJSON.hpp>

// rapid JSON
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <rapidjson/writer.h>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class JSON final
            {
            public:

                struct Element
                {
                    rapidjson::Value::MemberIterator document;
                    Node                             data;
                };

                static rapidjson::Value write_value(const Node& _Node)
                {
                    switch ((NodeValueType)_Node.type())
                    {
                        case NodeValueType::ATTRIBUTE:
                        {

                        }
                        break;

                        case NodeValueType::NUMBER: // write number
                        {
                            rapidjson::Value value(rapidjson::kNumberType);
                            value.SetDouble(Helpers::from_string<double>(_Node.value()));
                            return value;
                        }
                        break;
                    }

                    rapidjson::Value value(rapidjson::kStringType);
                    value.SetString(_Node.value(), (int)strlen(_Node.value()));
                    return value;
                }

                static bool read(Document* _Document, const std::filesystem::path& _Path)
                {
                    // check document pointer
                    if(_Document == nullptr) 
                        return false;

                    // check path
                    if(!std::filesystem::exists(_Path)) 
                        return false;

                    // open JSON file
                    FILE* fp = Helpers::open_file(_Path.string(), "rb");

                    // check that file has been opened
                    if(fp == nullptr) 
                        return false;

                    // read file
                    char readBuffer[65536];
                    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
                    
                    // parse file
                    rapidjson::Document document;
                    document.ParseStream(is);
                    fclose(fp);

                    // parse in depth
                    if(!document.IsObject())
                    {
                        // TODO: write here another logic
                        return true;
                    }

                    for(auto it = document.MemberBegin(); it != document.MemberEnd(); it++)
                    {
                        Helpers::Stack<Element> stack;
                        stack.push({it, nullptr});

                        while(!stack.empty())
                        {
                            auto top = stack.top();
                            stack.pop();

                            auto& name  = top.document->name;
                            auto& value = top.document->value;

                            // parse value
                            if(value.IsNull())
                            {
                                _Document->append_node<nullptr_t>(name.GetString(), nullptr, top.data);
                            }
                            if(value.IsBool())
                            {
                                _Document->append_node<bool>(name.GetString(), value.GetBool(), top.data);
                            }
                            else if(value.IsNumber())
                            {
                                _Document->append_node<double>(name.GetString(), value.GetDouble(), top.data);
                            }
                            else if(value.IsArray())
                            {
                            }
                            else if(value.IsString())
                            {
                                _Document->append_node(name.GetString(), value.GetString(), top.data, NodeValueType::STRING);
                            }
                            else if(value.IsObject())
                            {
                                auto object = 
                                    _Document->append_node(name.GetString(), "", top.data, NodeValueType::OBJECT);

                                for (rapidjson::Value::MemberIterator it = value.GetObject().MemberBegin();
                                    it != value.GetObject().MemberEnd(); ++it)
                                {
                                    stack.push({it, object});
                                }
                            }
                        }
                    }

                    return true;
                }

                template<bool Compact>
                static bool write(Document* _Document, const std::filesystem::path& _Path)
                {
                    //return true;

                    // check document pointer
                    if(_Document == nullptr) 
                        return false;

                    // check path
                    if(!std::filesystem::exists(_Path.parent_path())) 
                        return false;

                    rapidjson::Document doc;

                    auto& root = doc.SetObject();

                    for(auto&& singleton : *_Document)
                    {
                        rapidjson::Value name(rapidjson::kStringType);
                        name.SetString(singleton.name(), (int)strlen(singleton.name()));
                        root.AddMember(name, rapidjson::Value(rapidjson::kObjectType), doc.GetAllocator());

                        Helpers::Stack<Element> queue;

                        for(auto&& child : singleton)
                        {
                            queue.push({std::prev(root.MemberEnd()), child});
                        }

                        while (!queue.empty())
                        {
                            auto data   = queue.top().data;
                            auto parent = queue.top().document->value.GetObject();
                            queue.pop();

                            rapidjson::Value name(rapidjson::kStringType);
                            name.SetString(data.name(), (int)strlen(data.name()));

                            if(data.has_value())
                            {
                                if(data.empty())
                                {
                                    parent.AddMember(name, write_value(data), doc.GetAllocator());
                                }
                                else
                                {
                                    rapidjson::Value object = rapidjson::Value(rapidjson::kObjectType);
                                    object.AddMember("Value", write_value(data), doc.GetAllocator());
                                    parent.AddMember(name, object, doc.GetAllocator());

                                    for(auto&& child : data) 
                                        queue.push({std::prev(parent.MemberEnd()), child});
                                }
                            }
                            else
                            {
                                parent.AddMember(name, rapidjson::Value(rapidjson::kObjectType), doc.GetAllocator());

                                for(auto&& child : data) 
                                    queue.push({std::prev(parent.MemberEnd()), child});
                            }
                        }
                    }

                    FILE* file = Helpers::open_file(_Path.string(), "wb");
 
                    if(file == nullptr) 
                        return false;

                    char writeBuffer[65536];
                    rapidjson::FileWriteStream ofstream(file, writeBuffer, sizeof(writeBuffer));
                    
                    if(!Compact)
                    {
                        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(ofstream);
                        doc.Accept(writer);
                    }
                    else
                    {
                        rapidjson::Writer<rapidjson::FileWriteStream> writer(ofstream);
                        doc.Accept(writer);
                    }
                    
                    // don't forget to close the file
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

bool JSONReader::read(Document* _Document, const std::filesystem::path& _Path)
{
    return JSON::read(_Document, _Path);
}

bool JSONCompactWriter::write(Document* _Document, const std::filesystem::path& _Path)
{
    return JSON::write<true>(_Document, _Path);
}

bool JSONBeautifulWriter::write(Document* _Document, const std::filesystem::path& _Path)
{
    return JSON::write<false>(_Document, _Path);
}