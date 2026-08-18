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

                    static bool is_supported_dom_tree_node_value(const ElementObj& _Node)
                    {
                        return !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeCDATA  ) &&
                               !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeProlog ) &&
                               !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeComment);
                    };

                    static bool read_json(const ElementObj& _Object, const char* _Begin, const char* _End)
                    {
                        // check inputs
                        if(_Object.is_null() || _Begin == nullptr || _End == nullptr)
                            return false;

                        // nested types
                        struct JSONValue
                        {
                            std::string_view Value     {std::string_view()};
                            int              Attributes{ElementAttributes_::ElementAttributes_ElementTypeObject};
                        };

                        // auxiliary lambdas
                        auto typedPairValue = [](const char* _Begin, const int& _Size)->JSONValue
                        {
                            int valueBegin = 0;
                            int valueEnd   = _Size;
                            while (Helpers::is_empty_symbol(_Begin[valueEnd - 1]))--valueEnd;

                            std::string_view value(&_Begin[valueBegin], valueEnd - valueBegin);
                            int attruibutes = ElementAttributes_::ElementAttributes_ElementTypeObject;

                            if(_Begin[valueBegin] == '"' && _Begin[valueEnd - 1] == '"')
                            {
                                ++valueBegin; --valueEnd;
                                value = std::string_view(&_Begin[valueBegin], valueEnd - valueBegin);
                                attruibutes |= ElementAttributes_::ElementAttributes_ElementValueTypeString;
                            }
                            else if(value == "true" || value == "false")
                            {
                                attruibutes |= ElementAttributes_::ElementAttributes_ElementValueTypeBoolean;
                            }
                            else if(value == "null")
                            {
                                attruibutes |= ElementAttributes_::ElementAttributes_ElementValueTypeNullptr;
                            }
                            else if(([](const char* _Begin, const int& _Size)->bool
                            {
                                int floatingDelimitersCount = 0;

                                for(int i = 0; i < _Size; i++)
                                {
                                    if(_Begin[i] == '.')
                                    {
                                        ++floatingDelimitersCount;
                                        if(i == 0 || floatingDelimitersCount > 1)
                                            return false;
                                    }

                                    if(i == 0 && _Begin[i] == '.')
                                        return false;

                                    if(i > 0 && (_Begin[i] == '+' || _Begin[i] == '-'))
                                        return false;

                                    if(
                                        _Begin[i] != '0' &&
                                        _Begin[i] != '1' &&
                                        _Begin[i] != '2' &&
                                        _Begin[i] != '3' &&
                                        _Begin[i] != '4' &&
                                        _Begin[i] != '5' &&
                                        _Begin[i] != '6' &&
                                        _Begin[i] != '7' &&
                                        _Begin[i] != '8' &&
                                        _Begin[i] != '9' &&
                                        _Begin[i] != '.' &&
                                        _Begin[i] != '+' &&
                                        _Begin[i] != '-')
                                    {
                                        return false;
                                    }
                                }

                                return true;
                            })(value.data(), (int)value.size()))
                            {
                                attruibutes |= ElementAttributes_::ElementAttributes_ElementValueTypeFloat;
                            }
                            else
                            {
                                attruibutes |= ElementAttributes_::ElementAttributes_ElementValueTypeString;
                            }

                            return {value, attruibutes};
                        };

                        auto parsePairName  = [](const char* _Begin, const int& _Size)->std::string_view
                        {
                            int nameBegin = 0;
                            while (nameBegin < _Size && _Begin[nameBegin] != '"')++nameBegin;
                            int nameEnd = ++nameBegin;

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

                        auto parsePairValue = [&typedPairValue](const char* _Begin, const int& _Size)->JSONValue
                        {
                            int valueBegin = 0;

                            {
                                bool isCharacterSequence = false;

                                while (valueBegin < _Size)
                                {
                                    if(_Begin[valueBegin] == '"')
                                        isCharacterSequence = !isCharacterSequence;

                                    if(_Begin[valueBegin] == ':' && !isCharacterSequence)
                                        break;

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
                            }
                            
                            int valueEnd = valueBegin;

                            {
                                bool isCharacterSequence = false;

                                while(valueEnd < _Size)
                                {
                                    if(_Begin[valueEnd] == '"')
                                        isCharacterSequence = !isCharacterSequence;

                                    if(
                                        (_Begin[valueEnd] == ',' ||
                                         _Begin[valueEnd] == '{' ||
                                         _Begin[valueEnd] == '[' ||
                                         _Begin[valueEnd] == '}' ||
                                         _Begin[valueEnd] == ']') &&
                                        !isCharacterSequence)
                                    {
                                        break;
                                    }

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
                            }

                            return typedPairValue(&_Begin[valueBegin], valueEnd - valueBegin);
                        };

                        // main code
                        const DOMTree*   document = _Object.get_document(); 
                        ElementObj       parent   = _Object;
                        size_t           length   = (size_t)(_End - _Begin);

                        for (int element = 0; element < (int)length; element++)
                        {
                            if(Helpers::is_empty_symbol(_Begin[element])) continue;

                            // parse key-value pair
                            if(
                                _Begin[element] != '{' &&
                                _Begin[element] != '[' &&
                                _Begin[element] != '}' &&
                                _Begin[element] != ']' &&
                                _Begin[element] != ',')
                            {
                                int  contentSequence     = element;
                                bool isCharacterSequence = false;

                                while(contentSequence < (int)length)
                                {
                                    if(_Begin[contentSequence] == '"')
                                        isCharacterSequence = !isCharacterSequence;

                                    if(
                                        (_Begin[contentSequence] == ',' ||
                                         _Begin[contentSequence] == '{' ||
                                         _Begin[contentSequence] == '[' ||
                                         _Begin[contentSequence] == '}' ||
                                         _Begin[contentSequence] == ']') &&
                                        !isCharacterSequence)
                                    {
                                        break;
                                    }

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
                                }
                                else if(_Begin[contentSequence] == '[')
                                {
                                    ElementObj newObj = document->create_node(
                                        parsePairName(&_Begin[element], contentSequence - element),
                                        std::string_view(),
                                        ElementAttributes_::ElementAttributes_ElementTypeCollection);

                                    if(document->append_node(newObj, parent))
                                        parent = newObj;
                                }
                                else if(_Begin[contentSequence] == ',' || _Begin[contentSequence] == '}' || _Begin[contentSequence] == ']')
                                {
                                    if(([](const char* _Begin, const int& _Size)->bool
                                    {
                                        int  keyValueSequence    = 0;
                                        bool isCharacterSequence = false;

                                        while(keyValueSequence < _Size)
                                        {
                                            if(_Begin[keyValueSequence] == '"')
                                                isCharacterSequence = !isCharacterSequence;

                                            if(_Begin[keyValueSequence] == ':' && !isCharacterSequence)
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
                                        JSONValue jsonValue = parsePairValue(&_Begin[element], contentSequence - element);

                                        document->append_node(
                                            document->create_node(
                                                parsePairName(
                                                    &_Begin[element],
                                                    contentSequence - element),
                                                    jsonValue.Value,
                                                    jsonValue.Attributes),
                                                parent);
                                    }
                                    else
                                    {
                                        JSONValue jsonValue = typedPairValue(&_Begin[element], contentSequence - element);
                                        document->append_node(document->create_node(std::string_view(), jsonValue.Value, jsonValue.Attributes), parent);
                                    }
                                }

                                element = contentSequence;
                            }

                            // create new object
                            else if(_Begin[element] == '{')
                            {
                                ElementObj newObj = document->create_node(
                                    std::string_view(),
                                    std::string_view(),
                                    ElementAttributes_::ElementAttributes_ElementTypeObject);

                                if(document->append_node(newObj, parent))
                                    parent = newObj;
                            }

                            // create new array (collection)
                            else if(_Begin[element] == '[')
                            {
                                ElementObj newObj = document->create_node(
                                    std::string_view(),
                                    std::string_view(),
                                    ElementAttributes_::ElementAttributes_ElementTypeCollection);

                                if(document->append_node(newObj, parent))
                                    parent = newObj;
                            }

                            // go up the tree
                            else if(_Begin[element] == ']' || _Begin[element] == '}')
                            {
                                parent = parent.get_parent();
                            }
                        }

                        return true;
                    }

                    template<typename Streamer>
                    static bool write_json(const ElementObj& _Object, Streamer& _Streamer, const bool _Pretty)
                    {
                        if(_Object.is_null() || !_Streamer.begin())
                        {
                            _Streamer.end();
                            return false;
                        }

                        _Object.traverse(
                            [&_Streamer, &_Pretty, _Object](const ElementObj& _Node, const int& _Depth)
                            {
                                if(_Pretty)
                                {
                                    for (int i = 0; i < _Depth - 1; i++)
                                        _Streamer.write(" ", 1);
                                }

                                if(
                                    _Node.get_parent() != _Object &&
                                    !(_Node.get_parent().get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeCollection))
                                {
                                    _Streamer.write("\"", 1);
                                    _Streamer.write(_Node.get_name().data(), (int)_Node.get_name().size());
                                    _Streamer.write("\":", 2);
                                }

                                if(_Node.empty())
                                {
                                    if(!is_supported_dom_tree_node_value(_Node))
                                    {
                                        _Streamer.write("\"\"", 2);
                                    }
                                    else if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeString)
                                    {
                                        _Streamer.write("\"", 1);
                                        _Streamer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                                        _Streamer.write("\"", 1);
                                    }
                                    else if(
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeNullptr   ) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeBoolean   ) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeFloat     ) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeDouble    ) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeLongDouble) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeInt8      ) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeInt16     ) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeInt32     ) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeInt64     ) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeUint8     ) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeUint16    ) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeUint32    ) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeUint64    ))
                                    {
                                        _Streamer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                                    }
                                }
                                else
                                {
                                    _Streamer.write((_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject) ? "{" : "[", 1);

                                    if(_Pretty)
                                        _Streamer.write("\n", 1);
                                }
                            },
                            [&_Streamer, &_Pretty](const ElementObj& _Node, const int& _Depth)
                            {
                                if(!_Node.empty())
                                {
                                    if(_Pretty)
                                    {
                                        for (int i = 0; i < _Depth - 1; i++)
                                            _Streamer.write(" ", 1);
                                    }

                                    _Streamer.write((_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject) ? "}" : "]", 1);
                                }

                                if(_Node.get_next().is_not_null())
                                    _Streamer.write(",", 1);

                                if(_Pretty)
                                    _Streamer.write("\n", 1);
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