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

                        // nested types
                        struct JSONValue
                        {
                            std::string_view Value     {std::string_view()};
                            int              Attributes{ElementAttributes_::ElementAttributes_ElementTypeObject};
                        };

                        // auxiliary lambdas
                        auto adjustValue = [](const char* _Begin, const int& _Size)->JSONValue
                        {
                            int valueBegin = 0;
                            while (Helpers::is_empty_symbol(_Begin[valueBegin]))++valueBegin;
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

                            return {value, attruibutes};
                        };

                        // get ready
                        const DOMTree* document = _Object.get_document(); 
                        ElementObj     parent   = _Object;
                        size_t         length   = (size_t)(_End - _Begin);

                        if(length < 2)
                            return false;

                        // JSON structure validation
                        int quotes   = 0;
                        int braces   = 0;
                        int brackets = 0;

                        for (int element = 0; element < (int)length;)
                        {
                            if(_Begin[element] == '{')++braces;
                            if(_Begin[element] == '}')--braces;
                            if(_Begin[element] == '[')++brackets;
                            if(_Begin[element] == ']')--brackets;
                            if(_Begin[element] == '"') quotes = !quotes;

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

                        if(braces != 0 || brackets != 0 || quotes != 0)
                            return false;

                        // parse
                        for (int element = 0; element < (int)length; element++)
                        {
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
                                int  entryBegin = element;
                                int  entryEnd   = element;
                                int  nameBegin  = element;
                                int  nameEnd    = element;
                                int  valueBegin = element;
                                int  valueEnd   = element;
                                
                                bool isCharacterSequence = false;
                                bool isKeyValueSequence  = false;

                                while(entryEnd < (int)length)
                                {
                                    if(_Begin[entryEnd] == '"')
                                    {
                                        isCharacterSequence = !isCharacterSequence;

                                        if(!isKeyValueSequence)
                                        {
                                            if(isCharacterSequence)
                                                nameBegin = entryEnd < (int)length ? entryEnd + 1 : entryEnd;
                                            else
                                                nameEnd = entryEnd;
                                        }
                                    }

                                    if(_Begin[entryEnd] == ':' && !isCharacterSequence && !isKeyValueSequence)
                                    {
                                        valueBegin = entryEnd < (int)length ? entryEnd + 1 : entryEnd;
                                        isKeyValueSequence = true;
                                    }

                                    if(
                                        (_Begin[entryEnd] == ',' ||
                                         _Begin[entryEnd] == '{' ||
                                         _Begin[entryEnd] == '[' ||
                                         _Begin[entryEnd] == '}' ||
                                         _Begin[entryEnd] == ']') &&
                                        !isCharacterSequence)
                                    {
                                        valueEnd = entryEnd;
                                        break;
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

                                // create named object
                                if(_Begin[entryEnd] == '{')
                                {
                                    ElementObj newObj = document->create_node(
                                        std::string_view(&_Begin[nameBegin], nameEnd - nameBegin),
                                        std::string_view(),
                                        ElementAttributes_::ElementAttributes_ElementTypeObject);

                                    if(document->append_node(newObj, parent))
                                        parent = newObj;
                                }

                                // create named array
                                else if(_Begin[entryEnd] == '[')
                                {
                                    ElementObj newObj = document->create_node(
                                        std::string_view(&_Begin[nameBegin], nameEnd - nameBegin),
                                        std::string_view(),
                                        ElementAttributes_::ElementAttributes_ElementTypeCollection);

                                    if(document->append_node(newObj, parent))
                                        parent = newObj;
                                }

                                // add key-value pair or array entry
                                else if((_Begin[entryEnd] == ',' || _Begin[entryEnd] == '}' || _Begin[entryEnd] == ']'))
                                {
                                    // adjust value
                                    JSONValue jsonValue =
                                        isKeyValueSequence ?
                                            adjustValue(&_Begin[valueBegin], valueEnd - valueBegin) :
                                                adjustValue(&_Begin[entryBegin], entryEnd - entryBegin);


                                    // add key-value-pair
                                    if(isKeyValueSequence)
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

                            // create ananymous object
                            else if(_Begin[element] == '{')
                            {
                                ElementObj newObj = document->create_node(
                                    std::string_view(),
                                    std::string_view(),
                                    ElementAttributes_::ElementAttributes_ElementTypeObject);

                                if(document->append_node(newObj, parent))
                                    parent = newObj;
                            }

                            // create ananymous array
                            else if(_Begin[element] == '[')
                            {
                                ElementObj newObj = document->create_node(
                                    std::string_view(),
                                    std::string_view(),
                                    ElementAttributes_::ElementAttributes_ElementTypeCollection);

                                if(document->append_node(newObj, parent))
                                    parent = newObj;
                            }

                            // next parent
                            if(_Begin[element] == ']' || _Begin[element] == '}')
                                parent = parent.get_parent();
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
                                    if(
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeCDATA)  ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeProlog) ||
                                        (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeComment)
                                    )
                                    {
                                        _Streamer.write("\"\"", 2);
                                    }
                                    else if((_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeString))
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