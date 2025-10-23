#include <FrenchieCoreSerializationFormatJSON.hpp>

// rapid JSON
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <rapidjson/writer.h>

#include <FrenchieCoreContainersStack.hpp>
#include <FrenchieCoreContainersQueue.hpp>

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
            class JSON final
            {
            public:

                static bool read(Document* _Document, const std::filesystem::path& _Path)
                {
                    // check document pointer
                    if(_Document == nullptr) 
                        return false;

                    // check path
                    if(!std::filesystem::exists(_Path)) 
                        return false;

                    // open JSON file
                    FILE* file = open_file(_Path.string(), "rb");

                    // check that file has been opened
                    if(file == nullptr) 
                        return false;

                    rapidjson::Document document;

                    // try to read file
                    try
                    {
                        // read file
                        char readBuffer[65536];                    
                        rapidjson::FileReadStream fileStream(file, readBuffer, sizeof(readBuffer));

                        // parse file
                        document.ParseStream(fileStream);
                        fclose(file);
                    }
                    catch(...)
                    {
                        return false;
                    }

                    if(!document.IsObject())
                    {
                        return true;
                    }

                    struct Element
                    {
                        rapidjson::Value* name  = nullptr;
                        rapidjson::Value* value = nullptr;
                        Node              data;
                    };

                    // reset document
                    _Document->reset();

                    // parse in depth
                    for(auto it = document.MemberBegin(); it != document.MemberEnd(); it++) // --> this is object
                    {
                        Stack<Element> stack;
                        stack.push({&it->name, &it->value, *_Document});

                        while(!stack.empty())
                        {
                            auto top = stack.top();
                            stack.pop();

                            rapidjson::Value* name  = top.name;
                            rapidjson::Value* value = top.value;

                            if(value->IsNull())
                            {
                                top.data.append_node<nullptr_t>(name != nullptr ? name->GetString() : "", nullptr);
                            }
                            if(value->IsBool())
                            {
                                top.data.append_node<bool>(name != nullptr ? name->GetString() : "", value->GetBool());
                            }
                            else if(value->IsNumber())
                            {
                                top.data.append_node<double>(name != nullptr ? name->GetString() : "", value->GetDouble());
                            }
                            else if(value->IsArray())
                            {
                                auto object = top.data.append_node(name != nullptr ? name->GetString() : "", "", NodeType::ARRAY);

                                for(auto i = value->Begin(); i != value->End(); ++i) 
                                    stack.push({nullptr, i, object});
                            }
                            else if(value->IsString())
                            {
                                top.data.append_node(name != nullptr ? name->GetString() : "", value->GetString(), NodeType::STRING);
                            }
                            if(value->IsObject())
                            {
                                auto object = top.data.append_node(name != nullptr ? name->GetString() : "", "", NodeType::OBJECT);

                                for(auto i = value->GetObject().MemberBegin(); i != value->GetObject().MemberEnd(); ++i) 
                                    stack.push({&i->name, &i->value, object});
                            }
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

                    FILE* file = open_file(_Path.string(), "wb");
 
                    if(file == nullptr) 
                        return false;

                    rapidjson::Document json;
                    JSON::write_object(*_Document, json.SetObject(), json);

                    char writeBuffer[65536];
                    rapidjson::FileWriteStream ofstream(file, writeBuffer, sizeof(writeBuffer));
                    
                    if(!Compact)
                    {
                        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(ofstream);
                        json.Accept(writer);
                    }
                    else
                    {
                        rapidjson::Writer<rapidjson::FileWriteStream> writer(ofstream);
                        json.Accept(writer);
                    }
                    
                    // don't forget to close the file
                    fclose(file);

                    return true;
                }

            protected:

                static void write_array(const Node& _Node, rapidjson::Value& _Parent, rapidjson::Document& _JSON)
                {
                    rapidjson::Value array(rapidjson::kArrayType);

                    for(auto&& data : _Node)
                    {
                        // write object
                        if(data.get_type() == NodeType::BOOL)
                        {
                            array.PushBack(
                                rapidjson::Value().Set<bool>(String::from_string<bool>(data.get_value())), 
                                _JSON.GetAllocator());
                        }
                        else if(data.get_type() == NodeType::NUMBER)
                        {
                            array.PushBack(
                                rapidjson::Value().Set<double>(String::from_string<double>(data.get_value())), 
                                _JSON.GetAllocator());
                        }
                        else if(data.get_type() == NodeType::STRING || 
                                data.get_type() == NodeType::ATTRIBUTE)
                        {
                            array.PushBack(
                                rapidjson::Value().SetString(data.get_value(), (int)strlen(data.get_value())), 
                                _JSON.GetAllocator());
                        }
                        else if(data.get_type() == NodeType::ARRAY)
                        {
                            // write array
                            rapidjson::Value object(rapidjson::kArrayType);
                            JSON::write_array(data, object, _JSON);

                            // push array into array
                            array.PushBack(object, _JSON.GetAllocator());
                        }
                        else if(data.get_type() == NodeType::OBJECT)
                        {
                            // write object
                            rapidjson::Value object(rapidjson::kObjectType);
                            JSON::write_object(data, object, _JSON);
                            
                            // push object into array
                            array.PushBack(object, _JSON.GetAllocator());
                        }
                    }                    
                    
                    if(_Parent.IsObject())
                    {
                        _Parent.AddMember(
                            rapidjson::Value().SetString(_Node.get_name(), (int)strlen(_Node.get_name())), 
                            array, 
                            _JSON.GetAllocator());
                    }
                    else
                    {
                        _Parent.PushBack(array, _JSON.GetAllocator());
                    }
                }

                static void write_object(const Node& _Node, rapidjson::Value& _Parent, rapidjson::Document& _JSON)
                {
                    struct Element
                    {
                        rapidjson::Value::MemberIterator member;
                        Node                             data;
                    };

                    for(auto&& node : _Node)
                    {
                        // write bool
                        if(node.get_type() == NodeType::BOOL)
                        {                                
                            _Parent.AddMember(
                                rapidjson::Value().SetString(node.get_name(), (int)strlen(node.get_name())), 
                                rapidjson::Value().Set<bool>(String::from_string<bool>(node.get_value())), 
                                _JSON.GetAllocator());
                            continue;
                        }
                        
                        // write number
                        if(node.get_type() == NodeType::NUMBER)
                        {
                            _Parent.AddMember(
                                rapidjson::Value().SetString(node.get_name(), (int)strlen(node.get_name())), 
                                rapidjson::Value().Set<double>(String::from_string<double>(node.get_value())), 
                                _JSON.GetAllocator());
                            continue;
                        }
                        
                        // write string/attribute
                        if(node.get_type() == NodeType::STRING || 
                            node.get_type() == NodeType::ATTRIBUTE)
                        {
                            _Parent.AddMember(
                                rapidjson::Value().SetString(node.get_name(), (int)strlen(node.get_name())), 
                                rapidjson::Value().SetString(node.get_value(), (int)strlen(node.get_value())), 
                                _JSON.GetAllocator());
                            continue;
                        }

                        // write array
                        if(node.get_type() == NodeType::ARRAY)
                        {
                            JSON::write_array(node, _Parent, _JSON);
                            continue;
                        }

                        // write object
                        if(node.get_type() != NodeType::OBJECT) 
                            continue;

                        // get ready
                        rapidjson::Value name;
                        name.SetString(node.get_name(), (int)strlen(node.get_name()));
                        _Parent.AddMember(name, rapidjson::Value(rapidjson::kObjectType), _JSON.GetAllocator());

                        // push children onto the stack
                        Stack<Element> stack;

                        for(auto&& child : node)
                            stack.push({std::prev(_Parent.MemberEnd()), child});

                        // write recursivelly
                        while (!stack.empty())
                        {
                            auto& parent = stack.top().member->value;
                            auto  data   = stack.top().data;
                            stack.pop();

                            // write object
                            if(data.get_type() == NodeType::BOOL)
                            {                                
                                parent.AddMember(
                                    rapidjson::Value().SetString(data.get_name(), (int)strlen(data.get_name())), 
                                    rapidjson::Value().Set<bool>(String::from_string<bool>(data.get_value())), 
                                    _JSON.GetAllocator());
                            }
                            else if(data.get_type() == NodeType::NUMBER)
                            {
                                parent.AddMember(
                                    rapidjson::Value().SetString(data.get_name(), (int)strlen(data.get_name())), 
                                    rapidjson::Value().Set<double>(String::from_string<double>(data.get_value())), 
                                    _JSON.GetAllocator());
                            }
                            else if(data.get_type() == NodeType::STRING || 
                                    data.get_type() == NodeType::ATTRIBUTE)
                            {
                                parent.AddMember(
                                    rapidjson::Value().SetString(data.get_name(), (int)strlen(data.get_name())), 
                                    rapidjson::Value().SetString(data.get_value(), (int)strlen(data.get_value())), 
                                    _JSON.GetAllocator());
                            }
                            else if(data.get_type() == NodeType::ARRAY)
                            {
                                JSON::write_array(data, parent, _JSON);
                            }
                            else if(data.get_type() == NodeType::OBJECT)
                            {
                                if(data.has_value())
                                {
                                    if(!data.empty())
                                    {
                                        rapidjson::Value object(rapidjson::kObjectType);
                                        object.AddMember(
                                            rapidjson::Value(rapidjson::kStringType).SetString("Value"), 
                                            rapidjson::Value(rapidjson::kStringType).SetString(data.get_value(), (int)strlen(data.get_value())),
                                            _JSON.GetAllocator()
                                        );

                                        parent.AddMember(
                                            rapidjson::Value(rapidjson::kStringType).SetString(data.get_name(), (int)strlen(data.get_name())), 
                                            object, 
                                            _JSON.GetAllocator());

                                        for(auto&& child : data) 
                                            stack.push({std::prev(parent.MemberEnd()), child});
                                    }
                                    else
                                    {
                                        parent.AddMember(
                                            rapidjson::Value(rapidjson::kStringType).SetString(data.get_name(), (int)strlen(data.get_name())), 
                                            rapidjson::Value(rapidjson::kStringType).SetString(data.get_value(), (int)strlen(data.get_value())),
                                            _JSON.GetAllocator());
                                    }
                                }
                                else
                                {                                    
                                    parent.AddMember(
                                        rapidjson::Value(rapidjson::kStringType).SetString(data.get_name(), (int)strlen(data.get_name())), 
                                        rapidjson::Value(rapidjson::kObjectType), 
                                        _JSON.GetAllocator());

                                    for(auto&& child : data) 
                                        stack.push({std::prev(parent.MemberEnd()), child});
                                }
                            }
                        }
                    }
                }
            };
        }
    }
}

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