// Core
#include <FrenchieCoreSerizliationXML.hpp>

// STL
#include <string.h>
#include <string>

using namespace Frenchie::Core::Serizliation;
using namespace Frenchie::Core::Serizliation::XML;

namespace Frenchie
{
    namespace Core
    {
        namespace Serizliation
        {
            namespace XML
            {
                class Helpers
                {
                public:

                    template<typename Streamer>
                    static void write_xml_name(const std::string_view& _Input, Streamer& _Streamer)
                    {
                        for (int i = 0; i < (int)_Input.size(); i++)
                        {
                            if(i == 0)
                            {
                                if(_Input[i] < '0' || _Input[i] > '9')
                                    _Streamer.write(&_Input[i], 1);
                                continue;
                            }

                            switch (_Input[i])
                            {
                                case '!' :
                                case '"' :
                                case '#' :
                                case '$' :
                                case '%' :
                                case '&' :
                                case '\'':
                                case '\\':
                                case '/' :
                                case '(' :
                                case ')' :
                                case '*' :
                                case '+' :
                                case '-' :
                                case '.' :
                                case ',' :
                                case ';' :
                                case '<' :
                                case '>' :
                                case '=' :
                                case '?' :
                                case '@' :
                                case '[' :
                                case ']' :
                                case '^' :
                                case '{' :
                                case '}' :
                                case '|' :
                                case '~' :
                                    continue;
                            default:
                                    _Streamer.write(&_Input[i], 1);
                                break;
                            }
                        }
                    }

                    template<typename Streamer>
                    static void write_xml_value(const std::string_view& _Input, Streamer& _Streamer)
                    {
                        for (int i = 0; i < (int)_Input.size(); i++)
                        {
                            switch (_Input[i])
                            {
                            case '<':
                                _Streamer.write("&", 1);
                                _Streamer.write("l", 1);
                                _Streamer.write("t", 1);
                                _Streamer.write(";", 1);
                                break;
                            
                            case '>':
                                _Streamer.write("&", 1);
                                _Streamer.write("g", 1);
                                _Streamer.write("t", 1);
                                _Streamer.write(";", 1);
                                break;

                            case '&':
                                _Streamer.write("&", 1);
                                _Streamer.write("a", 1);
                                _Streamer.write("m", 1);
                                _Streamer.write("p", 1);
                                _Streamer.write(";", 1);
                                break;

                            case '\'':
                                _Streamer.write("&", 1);
                                _Streamer.write("a", 1);
                                _Streamer.write("p", 1);
                                _Streamer.write("o", 1);
                                _Streamer.write("s", 1);
                                _Streamer.write(";", 1);
                                break;

                            case '"':
                                _Streamer.write("&", 1);
                                _Streamer.write("q", 1);
                                _Streamer.write("u", 1);
                                _Streamer.write("o", 1);
                                _Streamer.write("t", 1);
                                _Streamer.write(";", 1);
                                break;

                            default:
                                _Streamer.write(&_Input[i], 1);
                                break;
                            }
                        }
                    }

                    static DOMTree::Status read_xml_string(const ElementObj& _Object, const char* _Begin, const char* _End)
                    {
                        // check inputs
                        if(_Object.is_null() || _Begin == nullptr || _End == nullptr)
                            return DOMTree::Status(false, "input string is null.");

                        // get ready
                        const DOMTree* document = _Object.get_document(); 
                        ElementObj     parent   = _Object;
                        size_t         length   = (size_t)(_End - _Begin);

                        if(length < 2)
                            return {false, "input string is too short."};

                        for (int element = 0; element < (int)length;)
                        {
                            // identify element type
                            if(_Begin[element] != '<')
                            {
                                ++element;
                                continue;
                            }

                            // <? ... ?>
                            {
                                int prologSequence = element;

                                if(prologSequence + 1 < (int)length && _Begin[++prologSequence] == '?')
                                {
                                    int  prologBegin = ++prologSequence;
                                    while (prologSequence < (int)length)
                                    {
                                        if( _Begin[prologSequence] == '?' && prologSequence + 1 < (int)length &&
                                            _Begin[prologSequence + 1] == '>')
                                        {
                                            break;
                                        }

                                        ++prologSequence;
                                    }

                                    if(prologSequence >= (int)length)
                                    {
                                        return DOMTree::Status(
                                            false,
                                            std::string("invalid prolog at ")
                                            .append(std::to_string(element))
                                            .append(":\n")
                                            .append(std::string_view(&_Begin[element], std::min<int>(32, length))));
                                    }

                                    document->append_node(
                                        document->create_node(
                                            std::string_view(),
                                            std::string_view(&_Begin[prologBegin], prologSequence - prologBegin),
                                            ElementAttributes_::ElementAttributes_ElementTypeObject | ElementAttributes_::ElementAttributes_ElementValueTypeProlog),
                                            parent);

                                    while (prologSequence < (int)length && _Begin[prologSequence] != '>')++prologSequence;
                                    element = prologSequence;
                                    continue;
                                }
                            }

                            // <!-- ... -->
                            {
                                int commentSequence = element;

                                if(commentSequence + 1 < (int)length && _Begin[++commentSequence] == '!')
                                {
                                    if( !(commentSequence + 1 < (int)length && _Begin[++commentSequence] == '-')||
                                        !(commentSequence + 1 < (int)length && _Begin[++commentSequence] == '-'))
                                    {
                                        return DOMTree::Status(
                                            false,
                                            std::string("invalid comment at ")
                                            .append(std::to_string(element))
                                            .append(":\n")
                                            .append(std::string_view(&_Begin[element], std::min<int>(32, length))));
                                    }

                                    int commentBegin = ++commentSequence;

                                    while (commentSequence < (int)length)
                                    {
                                        if( _Begin[commentSequence + 0] == '-' && commentSequence + 1 < (int)length &&
                                            _Begin[commentSequence + 1] == '-' && commentSequence + 2 < (int)length &&
                                            _Begin[commentSequence + 2] == '>')
                                        {
                                            break;
                                        }

                                        ++commentSequence;
                                    }

                                    if(commentSequence >= (int)length)
                                    {
                                        return DOMTree::Status(
                                            false,
                                            std::string("invalid comment at ")
                                            .append(std::to_string(element))
                                            .append(":\n")
                                            .append(std::string_view(&_Begin[element], std::min<int>(32, length))));
                                    }

                                    document->append_node(
                                        document->create_node(
                                            std::string_view(),
                                            std::string_view(&_Begin[commentBegin], commentSequence - commentBegin),
                                            ElementAttributes_::ElementAttributes_ElementTypeObject | ElementAttributes_::ElementAttributes_ElementValueTypeComment),
                                            parent);

                                    while (commentSequence < (int)length && _Begin[commentSequence] != '>') ++commentSequence;
                                    element = commentSequence;
                                    continue;
                                }
                            }

                            // <...>...</...>, <.../>
                            {
                                // get ready
                                std::string_view name  = std::string_view();
                                std::string_view value = std::string_view();
                                int              type  = ElementAttributes_::ElementAttributes_ElementTypeObject;

                                // retrieve tag
                                int tagBegin = element;
                                int tagEnd   = element;
                                while (tagEnd < (int)length && _Begin[tagEnd] != '>') ++tagEnd;
                                element = tagEnd;

                                // parse name
                                int nameBegin = tagBegin;
                                while (nameBegin < tagEnd && (_Begin[nameBegin] == '<' || _Begin[nameBegin] == '/'))++nameBegin;
                                
                                int nameEnd   = nameBegin;
                                while (nameEnd < tagEnd && !Helpers::is_empty_symbol(_Begin[nameEnd]) && _Begin[nameEnd] != '/' && _Begin[nameEnd] != '>') ++nameEnd;
                                name = std::string_view(&_Begin[nameBegin], nameEnd - nameBegin);

                                // close tag
                                if(tagBegin + 1 < (int)length && _Begin[tagBegin + 1] == '/')
                                {
                                    if(name != parent.get_name())
                                    {
                                        return DOMTree::Status(
                                            false,
                                            std::string("missing closing tag for element at ")
                                            .append(std::to_string(element))
                                            .append(":\n")
                                            .append(name));
                                    }
                                    
                                    parent = parent.get_parent();
                                    continue;
                                }

                                // <![CDATA[...]]>>
                                int cdataSequence = element;
                                while (cdataSequence < (int)length && _Begin[cdataSequence] != '<')++cdataSequence;
                                
                                if(cdataSequence + 1 < (int)length && _Begin[++cdataSequence] == '!')
                                {
                                    if( !(cdataSequence + 1 < (int)length && _Begin[++cdataSequence] == '[')||
                                        !(cdataSequence + 1 < (int)length && _Begin[++cdataSequence] == 'C')||
                                        !(cdataSequence + 1 < (int)length && _Begin[++cdataSequence] == 'D')||
                                        !(cdataSequence + 1 < (int)length && _Begin[++cdataSequence] == 'A')||
                                        !(cdataSequence + 1 < (int)length && _Begin[++cdataSequence] == 'T')||
                                        !(cdataSequence + 1 < (int)length && _Begin[++cdataSequence] == 'A')||
                                        !(cdataSequence + 1 < (int)length && _Begin[++cdataSequence] == '['))
                                    {
                                        return DOMTree::Status(
                                            false,
                                            std::string("invalid CDATA section at ")
                                            .append(std::to_string(cdataSequence))
                                            .append(":\n")
                                            .append(std::string_view(&_Begin[cdataSequence], std::min<int>(32, length))));
                                    }

                                    int cdataBegin = ++cdataSequence;
                                    while (cdataSequence < (int)length)
                                    {
                                        if( _Begin[cdataSequence + 0] == ']' && cdataSequence + 1 < (int)length &&
                                            _Begin[cdataSequence + 1] == ']' && cdataSequence + 2 < (int)length &&
                                            _Begin[cdataSequence + 2] == '>')
                                        {
                                            break;
                                        }

                                        ++cdataSequence;
                                    }

                                    if(cdataSequence >= (int)length)
                                        return DOMTree::Status(false, "malformed CDATA section,");

                                    value = std::string_view(&_Begin[cdataBegin], cdataSequence - cdataBegin);
                                    type |= ElementAttributes_::ElementAttributes_ElementValueTypeCDATA;
                                    while (cdataSequence < length && _Begin[cdataSequence] != '>') ++cdataSequence;
                                    element = cdataSequence;
                                }

                                // <...>...</...>
                                else
                                {
                                    int  valueBegin = tagEnd + 1;
                                    int  valueEnd   = valueBegin;
                                    while(valueEnd < (int)length && _Begin[valueEnd] != '<')++valueEnd;
                                    while(valueEnd > valueBegin && Helpers::is_empty_symbol(_Begin[std::max<int>(valueEnd - 1, 0)]))--valueEnd;
                                    value = std::string_view(&_Begin[valueBegin], valueEnd - valueBegin);
                                    type |= ElementAttributes_::ElementAttributes_ElementValueTypeString;
                                    element = valueEnd;
                                }

                                // <.../>
                                bool isSelfClosing = _Begin[tagEnd - 1] == '/';

                                if(isSelfClosing)
                                {
                                    if(parent.get_ref()->m_Value.empty())
                                    {
                                        parent.get_ref()->m_Value      = value;
                                        parent.get_ref()->m_Attributes = type;
                                    }

                                    value = std::string_view();
                                    type = ElementAttributes_::ElementAttributes_ElementTypeObject | ElementAttributes_::ElementAttributes_ElementValueTypeString;
                                }

                                ElementObj newObj = document->create_node(name, value, type);
                                if(document->append_node(newObj, parent))
                                    parent = newObj;

                                // parse attributes
                                for (int attribute = nameEnd; attribute < tagEnd; attribute++)
                                {
                                    while (!Helpers::is_empty_symbol(_Begin[attribute]))++attribute;
                                    while (Helpers::is_empty_symbol(_Begin[attribute]))++attribute;
                                    
                                    if(attribute >= tagEnd) break;

                                    // parse name
                                    int attributeNameBegin = attribute;
                                    while (attribute < tagEnd && !Helpers::is_empty_symbol(_Begin[attribute]) && _Begin[attribute] != '=') ++attribute;
                                    std::string_view attributeName(&_Begin[attributeNameBegin], attribute - attributeNameBegin);

                                    // parse value
                                    while (attribute < tagEnd && _Begin[attribute] != '"')++attribute;
                                    int attributeValueBegin = ++attribute;
                                    while (attribute < tagEnd && _Begin[attribute] != '"')++attribute;
                                    std::string_view attributeValue(&_Begin[attributeValueBegin], attribute - attributeValueBegin);

                                    // create new attribute
                                    document->append_node(
                                        document->create_node(
                                            attributeName,
                                            attributeValue,
                                            ElementAttributes_::ElementAttributes_ElementTypeAttribute
                                            | ElementAttributes_::ElementAttributes_ElementValueTypeString),
                                        parent);
                                }

                                // go up
                                if(isSelfClosing)
                                    parent = parent.get_parent();
                            }
                        }

                        return DOMTree::Status(true, "XML parse succeeded.");
                    }

                    template<typename Streamer>
                    static bool write_xml(const ElementObj& _Object, Streamer& _Streamer, const bool Pretty)
                    {
                        if(_Object.is_null() || !_Streamer.begin())
                        {
                            _Streamer.end();
                            return false;
                        }

                        _Object.traverse(
                            [&_Streamer, &Pretty](const ElementObj& _Node, const int& _Depth)
                            {
                                if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject)
                                {
                                    // write prolog
                                    if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeProlog)
                                    {
                                        _Streamer.write("<?", 2);

                                        // we suppose, that you setup prolog correctly
                                        _Streamer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                                        
                                        if(Pretty)
                                            _Streamer.write("?>\n", 3);
                                        else
                                            _Streamer.write("?>", 2);
                                    }
                                    // write comment
                                    else if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeComment)
                                    {
                                        _Streamer.write("<!--", 4);

                                        // comment value can contain what ever, so we don't need any fitting
                                        _Streamer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                                        
                                        if(Pretty)
                                            _Streamer.write("-->\n", 4);
                                        else
                                            _Streamer.write("-->", 3);
                                    }
                                    // write default element
                                    else
                                    {
                                        if(Pretty)
                                        {
                                            for (int i = 0; i < _Depth - 1; i++)
                                                _Streamer.write("\t", 1);
                                        }

                                        _Streamer.write("<", 1);
                                        write_xml_name<Streamer>(_Node.get_name(), _Streamer);

                                        // write attributres
                                        int nonAttributicChildrenCount = 0;

                                        for(const auto& child : _Node)
                                        {
                                            if(!(child.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeAttribute))
                                            {
                                                ++nonAttributicChildrenCount;
                                                continue;
                                            }

                                            _Streamer.write(" ", 1);
                                            write_xml_name<Streamer>(child.get_name(), _Streamer);
                                            _Streamer.write("=\"", 2);
                                            write_xml_value<Streamer>(child.get_value(), _Streamer);
                                            _Streamer.write("\"", 1);
                                        }

                                        if(_Node.get_value().empty() && _Node.find_node([](const ElementObj& _Object){return (_Object.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject);}).is_null())
                                            _Streamer.write("/>", 2);
                                        else
                                            _Streamer.write(">", 1);
                                        
                                        // write value
                                        if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeCDATA)
                                        {
                                            _Streamer.write("<![CDATA[", 9);

                                            // CDATA value can contain what ever, so we don't need any fitting
                                            _Streamer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                                            _Streamer.write("]]>", 3);
                                        }
                                        else
                                        {
                                            write_xml_value<Streamer>(_Node.get_value(), _Streamer);
                                        }
                                        
                                        if(Pretty)
                                            _Streamer.write("\n", 1);
                                    }
                                }
                            },
                            [&_Streamer, &Pretty](const ElementObj& _Node, const int& _Depth)
                            {
                                if(_Node.get_value().empty() &&
                                    _Node.find_node([](const ElementObj& _Object){return (_Object.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject);}).is_null()) return;

                                if(
                                     (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject)      &&
                                    !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeAttribute)   &&
                                    !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeProlog) &&
                                    !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeComment))
                                {
                                    if(Pretty)
                                    {
                                        for (int i = 0; i < _Depth - 1; i++)
                                            _Streamer.write("\t", 1);
                                    }
                                    
                                    _Streamer.write("</", 2);
                                    write_xml_name<Streamer>(_Node.get_name(), _Streamer);
                                    
                                    if(Pretty)
                                        _Streamer.write(">\n", 2);
                                    else
                                        _Streamer.write(">", 1);
                                }
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
    return Helpers::read_xml_string(_Object, _Begin, _End);
}

// PrettyWriter
bool PrettyWriter::save_file(const ElementObj& _Object, const std::string& _Path)
{
    FileStreamer streamer(_Path);
    return Helpers::write_xml(_Object, streamer, true);
}

std::string PrettyWriter::write_string(const ElementObj& _Object)
{
    StringStreamer streamer;
    return Helpers::write_xml(_Object, streamer, true) ? streamer.get_stream_string() : std::string();
}

// CompactWriter
bool CompactWriter::save_file(const ElementObj& _Object, const std::string& _Path)
{
    FileStreamer streamer(_Path);
    return Helpers::write_xml(_Object, streamer, false);
}

std::string CompactWriter::write_string(const ElementObj& _Object)
{
    StringStreamer streamer;
    return Helpers::write_xml(_Object, streamer, false) ? streamer.get_stream_string() : std::string();
}