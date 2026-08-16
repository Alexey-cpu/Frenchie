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

                        // auxiliary lambdas
                        auto parsePairName = [](const char* _Begin, const int& _Size)->std::string_view
                        {
                            int nameBegin = 0;
                            while (nameBegin < _Size && _Begin[nameBegin] != '"')++nameBegin;
                            ++nameBegin;
                            int nameEnd = nameBegin;

                            while (nameEnd < _Size && _Begin[nameEnd] != '"')
                            {
                                if(_Begin[nameEnd] == '\\')
                                {
                                    ++nameEnd;
                                    ++nameEnd;
                                }
                                else
                                {
                                    ++nameEnd;
                                }
                            }

                            return std::string_view(&_Begin[nameBegin], nameEnd - nameBegin);
                        };

                        auto parsePairValue = [](const char* _Begin, const int& _Size)->std::string_view
                        {
                            int valueBegin = 0;

                            while (valueBegin < _Size && _Begin[valueBegin] != ':')
                            {
                                if(_Begin[valueBegin] == '\\')
                                {
                                    ++valueBegin;
                                    ++valueBegin;
                                }
                                else
                                {
                                    ++valueBegin;
                                }
                            }

                            while (valueBegin < _Size && (Helpers::is_empty_symbol(_Begin[valueBegin]) || _Begin[valueBegin] == ':'))++valueBegin;
                            int valueEnd = valueBegin;

                            while (
                                valueEnd < _Size &&

                                _Begin[valueEnd] != ',' &&
                                _Begin[valueEnd] != '{' &&
                                _Begin[valueEnd] != '[' &&
                                _Begin[valueEnd] != '}' &&
                                _Begin[valueEnd] != ']')
                            {
                                if(_Begin[valueEnd] == '\\')
                                {
                                    ++valueEnd;
                                    ++valueEnd;
                                }
                                else
                                {
                                    ++valueEnd;
                                }
                            }

                            return std::string_view(&_Begin[valueBegin], valueEnd - valueBegin);
                        };

                        // main code
                        const DOMTree*   document = _Object.get_document(); 
                        ElementObj       parent   = _Object;
                        size_t           length   = (size_t)(_End - _Begin);

                        for (int element = 0; element < (int)length; element++)
                        {
                            if(Helpers::is_empty_symbol(_Begin[element])) continue;

                            if(_Begin[element] != '{' && _Begin[element] != '[' && _Begin[element] != '}' && _Begin[element] != ']')
                            {
                                int  contentSequence   = element;
                                bool characterSequence = false;

                                while(
                                    contentSequence < (int)length  &&
                                    _Begin[contentSequence] != '{' &&
                                    _Begin[contentSequence] != '[' &&
                                    _Begin[contentSequence] != '}' &&
                                    _Begin[contentSequence] != ']')
                                {
                                    if(_Begin[contentSequence] == '"')
                                        characterSequence = !characterSequence;

                                    if(_Begin[contentSequence] == ',' && !characterSequence)
                                        break;

                                    if(_Begin[contentSequence] == '\\')
                                    {
                                        ++contentSequence;
                                        ++contentSequence;
                                    }
                                    else
                                    {
                                        ++contentSequence;
                                    }
                                }

                                if(_Begin[contentSequence] == '{')
                                {
                                    ElementObj newObj = document->create_node(
                                        parsePairName(&_Begin[element], contentSequence - element),
                                        std::string_view(),
                                        ElementAttributes_::ElementAttributes_ElementTypeObject);

                                    if(document->append_node(newObj, parent))
                                        parent = newObj;

                                    std::cout << "object: " << std::string_view(&_Begin[element], contentSequence - element) << "\n";
                                }
                                else if(_Begin[contentSequence] == '[')
                                {
                                    ElementObj newObj = document->create_node(
                                        parsePairName(&_Begin[element], contentSequence - element),
                                        std::string_view(),
                                        ElementAttributes_::ElementAttributes_ElementTypeCollection);

                                    if(document->append_node(newObj, parent))
                                        parent = newObj;

                                    std::cout << "array: " << std::string_view(&_Begin[element], contentSequence - element) << "\n";
                                }
                                else if(_Begin[contentSequence] == ',' || _Begin[contentSequence] == '}' || _Begin[contentSequence] == ']')
                                {
                                    if(([](const char* _Begin, const int& _Size)->bool
                                    {
                                        int keyValueSequence = 0;

                                        while(keyValueSequence < _Size)
                                        {
                                            if(_Begin[keyValueSequence] == ':')
                                                return true;

                                            if(_Begin[keyValueSequence] == '\\')
                                            {
                                                ++keyValueSequence;
                                                ++keyValueSequence;
                                            }
                                            else
                                            {
                                                ++keyValueSequence;
                                            }
                                        }

                                        return false;
                                    })(&_Begin[element], (contentSequence - element)))
                                    {
                                        document->append_node(
                                            document->create_node(
                                                parsePairName(&_Begin[element], contentSequence - element),
                                                parsePairValue(&_Begin[element], contentSequence - element)),
                                                parent);

                                        std::cout << "pair: " << std::string_view(&_Begin[element], contentSequence - element) << "\n";
                                    }
                                    else
                                    {
                                        std::cout << "element: " << std::string_view(&_Begin[element], contentSequence - element) << "\n";
                                    }
                                }

                                element = contentSequence;
                            }
                            else if(_Begin[element] == ']' || _Begin[element] == '}')
                            {
                                parent = parent.get_parent();
                            }

                            // // parse name
                            // {
                            //     int nameSequence = element;

                            //     if(_Begin[nameSequence] == '"')
                            //     {
                            //         int nameBegin = ++nameSequence;
                            //         while(_Begin[nameSequence] != '"' && nameSequence < (int)length)++nameSequence;
                            //         element = nameSequence;
                            //         name = std::string_view(&_Begin[nameBegin], nameSequence - nameBegin);
                            //     }
                            // }

                            // // parse value
                            // {
                            //     int valueSequence = element;

                            //     if(_Begin[valueSequence] == ':')
                            //     {
                            //         while((Helpers::is_empty_symbol(_Begin[valueSequence]) || _Begin[valueSequence] == ':') && valueSequence < (int)length)++valueSequence;
                            //         int valueBegin = valueSequence;
                            //         while(_Begin[valueSequence] != ',' && _Begin[valueSequence] != '{' && _Begin[valueSequence] != '[' && _Begin[valueSequence] != '}' && _Begin[valueSequence] != ']' && valueSequence < (int)length)++valueSequence;
                            //         if(_Begin[valueSequence] == '}' || _Begin[valueSequence] == ']')
                            //             while (Helpers::is_empty_symbol(_Begin[valueSequence-1]))--valueSequence;
                            //         element = valueSequence;

                            //         if(_Begin[valueSequence] != '{' && _Begin[valueSequence] != '[')
                            //         {
                            //             if(_Begin[valueBegin] == '"') ++valueBegin;
                            //             if(_Begin[valueSequence - 1] == '"') --valueSequence;

                            //             document->append_node(
                            //                 document->create_node(
                            //                     name,
                            //                     std::string_view(&_Begin[valueBegin], valueSequence - valueBegin)),
                            //                     parent);
                            //         }
                            //     }
                            // }

                            // // parse object
                            // if(_Begin[element] == '{' || _Begin[element] == '[')
                            // {
                            //     // create object
                            //     int attributes = 0;
                            //     if(_Begin[element] == '{') attributes |= ElementAttributes_::ElementAttributes_ElementTypeObject;
                            //     if(_Begin[element] == '[') attributes |= ElementAttributes_::ElementAttributes_ElementTypeCollection;

                            //     ElementObj newObj = document->create_node(name, std::string_view(), attributes);
                            //     if(document->append_node(newObj, parent))
                            //         parent = newObj;
                            // }
                            // else if(_Begin[element] == ']' || _Begin[element] == '}')
                            // {
                            //     parent = parent.get_parent();
                            // }
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