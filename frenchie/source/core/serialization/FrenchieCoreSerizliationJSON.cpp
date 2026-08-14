// Core
#include <FrenchieCoreSerizliationJSON.hpp>

// STL
#include <string.h>

using namespace Frenchie::Core::Serizliation;
using namespace Frenchie::Core::Serizliation::JSON;

namespace Frenchie
{
    namespace Core
    {
        namespace Serizliation
        {
            namespace JSON
            {
                class Helpers
                {
                public:

                    static bool read_json(const ElementObj& _Object, const char* _Begin, const char* _End)
                    {
                        // check inputs
                        if(_Object.is_null() || _Begin == nullptr || _End == nullptr)
                            return false;

                        const DOMTree*   document = _Object.get_document(); 
                        ElementObj       parent   = _Object;
                        size_t           length   = (size_t)(_End - _Begin);
                        std::string_view name     = std::string_view();

                        for (int element = 0; element < (int)length; element++)
                        {
                            // parse key-value pair
                            int pairSequence = element;

                            // parse name
                            {
                                int nameSequence = element;

                                if(_Begin[nameSequence] == '"')
                                {
                                    int nameBegin = ++nameSequence;
                                    while(_Begin[nameSequence] != '"' && nameSequence < (int)length)++nameSequence;
                                    element = nameSequence;
                                    name = std::string_view(&_Begin[nameBegin], nameSequence - nameBegin);
                                }
                            }

                            // parse value
                            {
                                int valueSequence = element;

                                if(_Begin[valueSequence] == ':')
                                {
                                    while((Helpers::is_empty_symbol(_Begin[valueSequence]) || _Begin[valueSequence] == ':') && valueSequence < (int)length)++valueSequence;
                                    int valueBegin = valueSequence;
                                    while(_Begin[valueSequence] != ',' && _Begin[valueSequence] != '{' && _Begin[valueSequence] != '[' && _Begin[valueSequence] != '}' && _Begin[valueSequence] != ']' && valueSequence < (int)length)++valueSequence;
                                    if(_Begin[valueSequence] == '}' || _Begin[valueSequence] == ']')
                                        while (Helpers::is_empty_symbol(_Begin[valueSequence-1]))--valueSequence;
                                    element = valueSequence;

                                    if(_Begin[valueSequence] != '{' && _Begin[valueSequence] != '[')
                                    {
                                        if(_Begin[valueBegin] == '"') ++valueBegin;
                                        if(_Begin[valueSequence - 1] == '"') --valueSequence;

                                        document->append_node(
                                            document->create_node(
                                                name,
                                                std::string_view(&_Begin[valueBegin], valueSequence - valueBegin)),
                                                parent);
                                    }
                                }
                            }

                            // parse object
                            {
                                if(_Begin[element] == '{' || _Begin[element] == '[')
                                {
                                    int attributes = 0;

                                    if(_Begin[element] == '{') attributes |= ElementAttributes_::ElementAttributes_ElementTypeObject;
                                    if(_Begin[element] == '[') attributes |= ElementAttributes_::ElementAttributes_ElementTypeCollection;

                                    ElementObj newObj = document->create_node(name, std::string_view(), attributes);
                                    if(document->append_node(newObj, parent))
                                        parent = newObj;
                                    name = std::string_view();
                                }
                                else if(_Begin[element] == ']' || _Begin[element] == '}')
                                {
                                    parent = parent.get_parent();
                                    name   = std::string_view();
                                }
                            }
                        }

                        return true;
                    }

                    template<typename Streamer>
                    static bool write_json(const ElementObj& _Object, Streamer& _Streamer, const bool Pretty)
                    {
                        if(_Object.is_null() || !_Streamer.begin())
                        {
                            _Streamer.end();
                            return false;
                        }

                        _Object.traverse(
                            [&_Streamer, &Pretty](const ElementObj& _Node, const int& _Depth)
                            {
                            },
                            [&_Streamer, &Pretty](const ElementObj& _Node, const int& _Depth)
                            {
                            }
                        );

                        _Streamer.end();

                        return true;
                    }

                    static bool is_empty_symbol(const char& _Symbol)
                    {
                        return _Symbol == '\t' || _Symbol == '\n' || _Symbol == '\0' || _Symbol == '\r' || _Symbol == ' ';
                    }
                };
            }
        }
    }
}

// Parser
bool Parser::read_string(const ElementObj& _Object, const char* _Begin, const char* _End)
{
    return Helpers::read_json(_Object, _Begin, _End);
}

// PrettyWriter
bool PrettyWriter::save_file(const ElementObj& _Object, const std::string& _Path)
{
    FileStreamer streamer(_Path);
    return Helpers::write_json(_Object, streamer, true);
}

std::string PrettyWriter::write_string(const ElementObj& _Object)
{
    StringStreamer streamer;
    return Helpers::write_json(_Object, streamer, true) ? streamer.get_stream_string() : std::string();
}

// CompactWriter
bool CompactWriter::save_file(const ElementObj& _Object, const std::string& _Path)
{
    FileStreamer streamer(_Path);
    return Helpers::write_json(_Object, streamer, false);
}

std::string CompactWriter::write_string(const ElementObj& _Object)
{
    StringStreamer streamer;
    return Helpers::write_json(_Object, streamer, false) ? streamer.get_stream_string() : std::string();
}