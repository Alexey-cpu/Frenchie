// Core
#include <FrenchieCoreSerizliationJSON.hpp>

// STL
#include <string.h>
#include <string>

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

                    // nested types
                    struct JSONValue
                    {
                        std::string_view Value     {std::string_view()};
                        int              Attributes{ElementAttributes_::ElementAttributes_ElementTypeObject};
                    };

                    // read API
                    static bool is_it_json_value(const JSONValue& jsonValue)
                    {
                        return  (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeNullptr   ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeBoolean   ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeFloat     ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeDouble    ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeLongDouble) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeInt8      ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeInt16     ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeInt32     ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeInt64     ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeUint8     ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeUint16    ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeUint32    ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeUint64    ) ||
                                (jsonValue.Attributes & ElementAttributes_::ElementAttributes_ElementValueTypeString    );
                    }

                    static bool is_it_json_string_value(const char* _Begin, const int& _Size)
                    {
                        return _Begin != nullptr && _Size >= 2 && _Begin[0] == '"' && _Begin[_Size - 1] == '"';
                    }

                    static bool is_it_json_bool_value(const char* _Begin, const int& _Size)
                    {
                        return _Begin != nullptr &&
                            ((_Size == 4 && std::string_view(_Begin, 4) == "true") ||
                             (_Size == 5 && std::string_view(_Begin, 5) == "false"));
                    }

                    static bool is_it_json_null_value(const char* _Begin, const int& _Size)
                    {
                        return _Begin != nullptr && _Size == 4 && std::string_view(_Begin, 4) == "null";
                    }

                    static bool is_it_json_decimal_number(const char* _Begin, const int& _Size)
                    {
                        if(_Begin == nullptr || _Size <= 0)
                            return false;

                        const int size = 16;
                        char pattern[size]{};
                        int  next = 0;

                        for(int i = 0; i < _Size; i++)
                        {
                            // check allowed symbols
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
                                _Begin[i] != '+' &&
                                _Begin[i] != '-' &&
                                _Begin[i] != '.' &&
                                _Begin[i] != 'e' &&
                                _Begin[i] != 'E')
                            {
                                return false;
                            }

                            // build matching pattern
                            if(next < size)
                            {
                                if( _Begin[i] == '+' ||
                                    _Begin[i] == '-' ||
                                    _Begin[i] == '.' ||
                                    _Begin[i] == 'e' ||
                                    _Begin[i] == 'E')
                                {
                                    pattern[next++] = _Begin[i];
                                }
                                else if(
                                    _Begin[i] == '0' ||
                                    _Begin[i] == '1' ||
                                    _Begin[i] == '2' ||
                                    _Begin[i] == '3' ||
                                    _Begin[i] == '4' ||
                                    _Begin[i] == '5' ||
                                    _Begin[i] == '6' ||
                                    _Begin[i] == '7' ||
                                    _Begin[i] == '8' ||
                                    _Begin[i] == '9')
                                {
                                    if(next <= 0 || pattern[next - 1] != '*')
                                        pattern[next++] = '*';
                                }
                            }
                        }

                        return
                                // integer number possible patterns
                                strcmp(pattern, R"(*)"      ) == 0 ||
                                strcmp(pattern, R"(-*)"     ) == 0 ||
                                strcmp(pattern, R"(*e+*)"   ) == 0 ||
                                strcmp(pattern, R"(-*e+*)"  ) == 0 ||
                                strcmp(pattern, R"(*e-*)"   ) == 0 ||
                                strcmp(pattern, R"(-*e-*)"  ) == 0 ||

                                // floating point number possible patterns
                                strcmp(pattern, R"(*.*)"    ) == 0 ||
                                strcmp(pattern, R"(-*.*)"   ) == 0 ||
                                strcmp(pattern, R"(*.*e+*)" ) == 0 ||
                                strcmp(pattern, R"(-*.*e+*)") == 0 ||
                                strcmp(pattern, R"(*.*e-*)" ) == 0 ||
                                strcmp(pattern, R"(-*.*e-*)") == 0;
                    }

                    static JSONValue read_json_value(const char* _Begin, const int& _Size)
                    {
                        if(_Begin == nullptr || _Size <= 0)
                            return {};

                        int valueBegin = 0;
                        while(valueBegin < _Size && Helpers::is_empty_symbol(_Begin[valueBegin]))++valueBegin;
                        int valueEnd   = _Size;
                        while(valueEnd > valueBegin && Helpers::is_empty_symbol(_Begin[valueEnd - 1]))--valueEnd;

                        if(valueBegin == valueEnd)
                            return {};

                        std::string_view value(&_Begin[valueBegin], valueEnd - valueBegin);
                        int attruibutes = ElementAttributes_::ElementAttributes_ElementTypeObject;

                        if(is_it_json_string_value(&_Begin[valueBegin], valueEnd - valueBegin))
                        {
                            ++valueBegin; --valueEnd;
                            value = std::string_view(&_Begin[valueBegin], valueEnd - valueBegin);
                            attruibutes |= ElementAttributes_::ElementAttributes_ElementValueTypeString;
                        }
                        else if(is_it_json_bool_value(&_Begin[valueBegin], valueEnd - valueBegin))
                        {
                            attruibutes |= ElementAttributes_::ElementAttributes_ElementValueTypeBoolean;
                        }
                        else if(is_it_json_null_value(&_Begin[valueBegin], valueEnd - valueBegin))
                        {
                            attruibutes |= ElementAttributes_::ElementAttributes_ElementValueTypeNullptr;
                        }
                        else if(is_it_json_decimal_number(&_Begin[valueBegin], valueEnd - valueBegin))
                        {
                            attruibutes |= ElementAttributes_::ElementAttributes_ElementValueTypeFloat;
                        }

                        return {value, attruibutes};
                    };

                    static DOMTree::Status read_json_string(const ElementObj& _Object, const char* _Begin, const char* _End)
                    {
                        // check inputs
                        if(_Object.is_null() || _Begin == nullptr || _End == nullptr || _End < _Begin)
                            return DOMTree::Status(false, "input string is null.");

                        // get ready
                        const DOMTree* document = _Object.get_document(); 
                        ElementObj     parent   = _Object;
                        size_t         length   = (size_t)(_End - _Begin);

                        if(length < strlen("{}"))
                            return DOMTree::Status(false, "input string length is less than minimum length of 2 symbols.");

                        // check that JSON document is balanced
                        int bracesCount   = 0;
                        int bracketsCount = 0;

                        int lastBrace     = 0;
                        int lastBracket   = 0;

                        for (int element = 0; element < (int)length;)
                        {                            
                            if(_Begin[element] == '{')
                            {
                                ++bracesCount;
                                lastBrace = element;
                            }

                            if(_Begin[element] == '}')
                            {
                                --bracesCount;
                                lastBrace = element;
                            }

                            if(_Begin[element] == '[')
                            {
                                ++bracketsCount;
                                lastBracket = element;
                            }

                            if(_Begin[element] == ']')
                            {
                                --bracketsCount;
                                lastBracket = element;
                            }

                            if(_Begin[element] == '\\')
                            {
                                ++element;
                                ++element;
                            }
                            else
                            {
                                ++element;
                            }
                        }

                        if(bracesCount != 0)
                        {
                            return DOMTree::Status(
                                false,
                                std::string("unbalanced braces at index ")
                                .append(std::to_string(lastBrace))
                                .append(":\n")
                                .append(std::string_view(
                                    &_Begin[lastBrace],
                                    std::min<int>(32, length))));
                        }

                        if(bracketsCount != 0)
                        {
                            return DOMTree::Status(
                                false,
                                std::string("unbalanced braces at index ")
                                .append(std::to_string(lastBracket))
                                .append(":\n")
                                .append(std::string_view(
                                    &_Begin[lastBracket],
                                    std::min<int>(32, length))));
                        }

                        // parse
                        int linesCount = 0;

                        for (int element = 0; element < (int)length; element++)
                        {
                            if(_Begin[element] == '\n')
                                ++linesCount;

                            if(Helpers::is_empty_symbol(_Begin[element])) continue;

                            // parse key-value pair
                            if(
                                _Begin[element] != '{' &&
                                _Begin[element] != '}' &&
                                _Begin[element] != '[' &&
                                _Begin[element] != ']' &&
                                _Begin[element] != ',' &&
                                _Begin[element] != ':')
                            {
                                int  entryBegin  = element;
                                int  entryEnd    = element;
                                int  nameBegin   = element;
                                int  nameEnd     = element;
                                int  valueBegin  = element;
                                int  valueEnd    = element;
                                int  quotesCount = 0;
                                int  colonsCount = 0;

                                const int size = 32;
                                char pattern[size]{};
                                int  next = 0;

                                while(entryEnd < (int)length)
                                {
                                    // parse name
                                    if(_Begin[entryEnd] == '"')
                                    {
                                        ++quotesCount;
                                        if(quotesCount == 1)
                                            nameBegin = entryEnd < (int)length ? entryEnd + 1 : entryEnd;
                                        if(quotesCount == 2)
                                            nameEnd = entryEnd;
                                    }

                                    bool isCharacterSequence = quotesCount % 2;

                                    // build matching pattern
                                    if(next < size)
                                    {
                                        if(_Begin[entryEnd] == '"')
                                        {
                                            pattern[next++] = _Begin[entryEnd];
                                        }
                                        else if(
                                            !isCharacterSequence &&
                                            (_Begin[entryEnd] == '{' ||
                                             _Begin[entryEnd] == '}' ||
                                             _Begin[entryEnd] == '[' ||
                                             _Begin[entryEnd] == ']' ||
                                             _Begin[entryEnd] == ':' ||
                                             _Begin[entryEnd] == ','))
                                        {
                                            pattern[next++] = _Begin[entryEnd];
                                        }
                                        else if(!isCharacterSequence && !Helpers::is_empty_symbol(_Begin[entryEnd]) && (next <= 0 || pattern[next - 1] != '.'))
                                        {
                                            pattern[next++] = '.';
                                        }
                                    }

                                    // parse value
                                    if(!isCharacterSequence)
                                    {
                                        if(_Begin[entryEnd] == ':')
                                        {
                                            valueBegin = entryEnd < (int)length ? entryEnd + 1 : entryEnd;
                                            ++colonsCount;
                                        }

                                        if( _Begin[entryEnd] == ',' ||
                                            _Begin[entryEnd] == '{' ||
                                            _Begin[entryEnd] == '}' ||
                                            _Begin[entryEnd] == '[' ||
                                            _Begin[entryEnd] == ']')
                                        {
                                            valueEnd = entryEnd;
                                            break;
                                        }
                                    }

                                    if(_Begin[entryEnd] == '\\')
                                    {
                                        ++entryEnd;
                                        ++entryEnd;
                                    }
                                    else
                                    {
                                        ++entryEnd;
                                    }
                                }

                                if(entryEnd >= (int)length)
                                {
                                    return DOMTree::Status(
                                        false,
                                        std::string("unterminated JSON value at line ")
                                        .append(std::to_string(linesCount)));
                                }

                                if( strcmp(pattern, R"("":"",)") != 0 &&
                                    strcmp(pattern, R"("":""})") != 0 &&
                                    strcmp(pattern, R"("":.,)" ) != 0 &&
                                    strcmp(pattern, R"("":.})" ) != 0 &&
                                    strcmp(pattern, R"("":[)"  ) != 0 &&
                                    strcmp(pattern, R"("":{)"  ) != 0 &&
                                    strcmp(pattern, R"("",)"   ) != 0 &&
                                    strcmp(pattern, R"(""])"   ) != 0 &&
                                    strcmp(pattern, R"(.,)"    ) != 0 &&
                                    strcmp(pattern, R"(.])"    ) != 0)
                                {
                                    return DOMTree::Status(
                                        false,
                                        std::string("syntax error at line ")
                                        .append(std::to_string(linesCount))
                                        .append(" ").append(pattern).append(" :\n")
                                        .append(std::string_view(
                                            &_Begin[entryBegin],
                                            std::min<int>(entryEnd - entryBegin + 32, length - entryBegin))));
                                }

                                // create named object or array
                                if(_Begin[entryEnd] == '{' || _Begin[entryEnd] == '[')
                                {
                                    ElementObj newObj = document->create_node(
                                        std::string_view(&_Begin[nameBegin], nameEnd - nameBegin),
                                        std::string_view(),
                                        _Begin[entryEnd] == '{' ?
                                            ElementAttributes_::ElementAttributes_ElementTypeObject :
                                                ElementAttributes_::ElementAttributes_ElementTypeCollection);

                                    if(document->append_node(newObj, parent))
                                        parent = newObj;
                                }

                                // add key-value pair or array entry
                                else if((_Begin[entryEnd] == ',' || _Begin[entryEnd] == '}' || _Begin[entryEnd] == ']'))
                                {
                                    // adjust value
                                    JSONValue jsonValue =
                                        colonsCount ?
                                            read_json_value(&_Begin[valueBegin], valueEnd - valueBegin) :
                                                read_json_value(&_Begin[entryBegin], entryEnd - entryBegin);

                                    // check that we've parsed JSON compatible value
                                    if(!is_it_json_value(jsonValue))
                                    {
                                        return DOMTree::Status(
                                            false,
                                            std::string("unrecognized JSON value at line ")
                                            .append(std::to_string(linesCount))
                                            .append(":\n")
                                            .append(std::string_view(
                                                &_Begin[entryBegin],
                                                std::min<int>(entryEnd - entryBegin + 32, length - entryBegin))));
                                    }

                                    // add key-value-pair
                                    if(colonsCount)
                                    {
                                        document->append_node(
                                            document->create_node(
                                                std::string_view(&_Begin[nameBegin], nameEnd - nameBegin),
                                                    jsonValue.Value,
                                                    jsonValue.Attributes),
                                                parent);
                                    }
                                    // add array entry
                                    else
                                    {
                                        document->append_node(
                                            document->create_node(
                                                std::string_view(),
                                                jsonValue.Value,
                                                jsonValue.Attributes),
                                            parent);
                                    }
                                }

                                element = entryEnd;
                            }

                            // create ananymous object or array
                            else if(_Begin[element] == '{' || _Begin[element] == '[')
                            {
                                ElementObj newObj = document->create_node(
                                    std::string_view(),
                                    std::string_view(),
                                    _Begin[element] == '{' ?
                                        ElementAttributes_::ElementAttributes_ElementTypeObject :
                                            ElementAttributes_::ElementAttributes_ElementTypeCollection);

                                if(document->append_node(newObj, parent))
                                    parent = newObj;
                            }

                            // next parent
                            if(_Begin[element] == ']' || _Begin[element] == '}')
                            {
                                // missing and duplicated comma check
                                {
                                    int index = element;

                                    do
                                    {
                                        ++index;
                                    } while(index < (int)length && Helpers::is_empty_symbol(_Begin[index]));

                                    // missing comma
                                    if(index < (int)length && _Begin[index] != ',' && _Begin[index] != '}' && _Begin[index] != ']')
                                    {
                                        return DOMTree::Status(
                                            false,
                                            std::string("missing comma between objects at line ")
                                            .append(std::to_string(linesCount))
                                            .append(":\n")
                                            .append(std::string_view(
                                                &_Begin[element],
                                                std::min<int>(32, length - element))));
                                    }

                                    // duplicated comma
                                    if(_Begin[index] == ',')
                                    {
                                        do
                                        {
                                            ++index;
                                        } while(index < (int)length && Helpers::is_empty_symbol(_Begin[index]));

                                        if(index < (int)length && _Begin[index] == ',')
                                        {
                                            return DOMTree::Status(
                                                false,
                                                std::string("duplicated comma between objects at line ")
                                                .append(std::to_string(linesCount))
                                                .append(":\n")
                                                .append(std::string_view(&_Begin[element], std::min<int>(32, length))));
                                        }
                                    }
                                }

                                // trailing comma check
                                {
                                    int index = element;

                                    do
                                    {
                                        --index;
                                    } while(index >= 0 && Helpers::is_empty_symbol(_Begin[index]));
                                    
                                    if(_Begin[index] == ',')
                                    {
                                        return DOMTree::Status(
                                            false,
                                            std::string("trailing comma between objects at line ")
                                            .append(std::to_string(linesCount))
                                            .append(":\n")
                                            .append(std::string_view(&_Begin[element], std::min<int>(32, length - element))));
                                    }
                                }

                                parent = parent.get_parent();
                            }
                        }

                        return DOMTree::Status(true, "JSON parse succeeded.");
                    }

                    // write API
                    template<typename Streamer>
                    static void write_json_value(const std::string_view& _Input, Streamer& _Streamer)
                    {
                        for (int i = 0; i < (int)_Input.size(); i++)
                        {
                            int tokensLeft = i - (int)_Input.size();

                            // &lt;
                            if(tokensLeft <= 4 && _Input[i + 0] == '&' && _Input[i + 1] == 'l' && _Input[i + 2] == 't' && _Input[i + 3] == ';')
                            {
                                _Streamer.write("<", 1); i += 3;
                                continue;
                            }

                            // &gt;
                            if(tokensLeft <= 4 && _Input[i + 0] == '&' && _Input[i + 1] == 'g' && _Input[i + 2] == 't' && _Input[i + 3] == ';')
                            {
                                _Streamer.write(">", 1); i += 3;
                                continue;
                            }

                            // &amp;
                            if(tokensLeft <= 5 && _Input[i + 0] == '&' && _Input[i + 1] == 'a' && _Input[i + 2] == 'm' && _Input[i + 3] == 'p' && _Input[i + 4] == ';')
                            {
                                _Streamer.write("&", 1); i += 4;
                                continue;
                            }

                            // &apos;
                            if(tokensLeft <= 6 && _Input[i + 0] == '&' && _Input[i + 1] == 'a' && _Input[i + 2] == 'p' && _Input[i + 3] == 'o' && _Input[i + 4] == 's' && _Input[i + 5] == ';')
                            {
                                _Streamer.write("'", 1); i += 5;
                                continue;
                            }

                            // &quot;
                            if(tokensLeft <= 6 && _Input[i + 0] == '&' && _Input[i + 1] == 'q' && _Input[i + 2] == 'u' && _Input[i + 3] == 'o' && _Input[i + 4] == 't' && _Input[i + 5] == ';')
                            {
                                _Streamer.write("\\\"", 2); i += 5;
                                continue;
                            }

                            _Streamer.write(&_Input[i], 1);
                        }
                    }

                    template<typename Streamer>
                    static bool write_json(const ElementObj& _Object, Streamer& _Streamer, const bool _Pretty)
                    {
                        if(_Object.is_null() || !_Streamer.begin())
                        {
                            _Streamer.end();
                            return false;
                        }

                        int isRoot = _Object.size() <= 1;

                        _Object.traverse(
                            [&_Streamer, &_Pretty, _Object, &isRoot](const ElementObj& _Node, const int& _Depth)
                            {
                                if(_Pretty)
                                {
                                    for (int i = 0; i < _Depth - 1; i++)
                                        _Streamer.write(" ", 1);
                                }

                                if(
                                    (_Node.get_parent() != _Object || !isRoot) &&
                                    !(_Node.get_parent().get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeCollection))
                                {
                                    _Streamer.write("\"", 1);
                                    _Streamer.write(_Node.get_name().data(), (int)_Node.get_name().size());
                                    _Streamer.write("\":", 2);
                                }

                                if(_Node.empty())
                                {
                                    if(
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeCDATA)  ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeProlog) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeComment))
                                    {
                                        _Streamer.write("\"\"", 2);
                                    }
                                    else if((_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeString))
                                    {
                                        _Streamer.write("\"", 1);
                                        write_json_value(_Node.get_value(), _Streamer);
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
                                        write_json_value(_Node.get_value(), _Streamer);
                                    }
                                    else
                                    {
                                        _Streamer.write((_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject) ? "{}" : "[]", 2);
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
DOMTree::Status Parser::read_string(const ElementObj& _Object, const char* _Begin, const char* _End)
{
    return Helpers::read_json_string(_Object, _Begin, _End);
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