// Core
#include <FrenchieCoreSerizliationXML.hpp>

// STL
#include <string.h>

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
                    static bool is_empty_symbol(const char& _Symbol)
                    {
                        return _Symbol == '\t' || _Symbol == '\n' || _Symbol == '\0' || _Symbol == '\r' || _Symbol == ' ';
                    }
                };
            }
        }
    }
}

bool Parser::read_string(const ElementObj& _Object, const char* _Begin, const char* _End)
{
    // check inputs
    if(_Object.is_null() || _Begin == nullptr || _End == nullptr)
        return false;

    const int minimumElementSequenceLength = strlen("</>");
    const int minimumCommentSequenceLength = strlen("<!---->");
    const int minimumPrologSequenceLength  = strlen("<\?\?>");
    const int minimumCDATASequenceLength   = strlen("<![CDATA[]]>");

    const DOMTree* document = _Object.get_document(); 
    ElementObj     parent   = _Object;
    size_t         length   = (size_t)(_End - _Begin);

    for (int element = 0; element < (int)length && length >= minimumElementSequenceLength;)
    {
        // identify element type
        if(_Begin[element] != '<')
        {
            ++element;
            continue;
        }

        // parse prolog
        {
            int prologSequence = element;

            if(length >= minimumPrologSequenceLength && _Begin[++prologSequence] == '?')
            {
                int prologBegin = ++prologSequence;
                while (prologSequence < length && !(_Begin[prologSequence] == '?' && _Begin[prologSequence + 1] == '>'))
                    ++prologSequence;

                document->append_node(
                    document->create_node(
                        std::string_view(),
                        std::string_view(&_Begin[prologBegin], prologSequence - prologBegin),
                        ElementAttributes_::ElementAttributes_ElementTypeObject | ElementAttributes_::ElementAttributes_ElementValueTypeProlog),
                        parent);

                while (_Begin[prologSequence] != '>')++prologSequence;
                element = prologSequence;

                continue;
            }
        }

        // parse comment
        {
            int commentSequence = element;

            if(length >= minimumCommentSequenceLength && _Begin[++commentSequence] == '!' && _Begin[++commentSequence] == '-' && _Begin[++commentSequence] == '-')
            {
                int commentBegin = ++commentSequence;
                while (commentSequence < length && !(_Begin[commentSequence] == '-' && _Begin[commentSequence + 1] == '-' && _Begin[commentSequence + 2] == '>'))
                    ++commentSequence;

                document->append_node(
                    document->create_node(
                        std::string_view(),
                        std::string_view(&_Begin[commentBegin], commentSequence - commentBegin),
                        ElementAttributes_::ElementAttributes_ElementTypeObject | ElementAttributes_::ElementAttributes_ElementValueTypeComment),
                        parent);

                while (_Begin[commentSequence] != '>') ++commentSequence;
                element = commentSequence;

                continue;
            }
        }

        // parse tag
        {
            // retrieve tag
            int tagBegin = ++element;
            while (element < (int)length && _Begin[element] != '>') ++element;
            int tagEnd = element;

            // parse attributes and name if this is not a closing tag
            if(_Begin[tagBegin] != '/')
            {
                // parse name
                int nameBegin = tagBegin;
                int nameEnd   = tagBegin;
                while (nameEnd < tagEnd && !Helpers::is_empty_symbol(_Begin[nameEnd]) && _Begin[nameEnd] != '/' && _Begin[nameEnd] != '>') ++nameEnd;

                // parse value
                std::string_view valueView;
                int              valueType = ElementAttributes_::ElementAttributes_ElementValueTypeString;

                {
                    // parse CDATA value
                    int cdataSequence = element;
                    while (_Begin[cdataSequence] != '<')++cdataSequence;
                    
                    if(
                        length >= minimumCDATASequenceLength &&

                        _Begin[++cdataSequence] == '!' &&
                        _Begin[++cdataSequence] == '[' &&
                        _Begin[++cdataSequence] == 'C' &&
                        _Begin[++cdataSequence] == 'D' &&
                        _Begin[++cdataSequence] == 'A' &&
                        _Begin[++cdataSequence] == 'T' &&
                        _Begin[++cdataSequence] == 'A' &&
                        _Begin[++cdataSequence] == '[')
                    {
                        int cdataBegin = ++cdataSequence;
                        while (cdataSequence < length && !(_Begin[cdataSequence] == ']' && _Begin[cdataSequence + 1] == ']' && _Begin[cdataSequence + 2] == '>'))
                            ++cdataSequence;

                        valueView = std::string_view(&_Begin[cdataBegin], cdataSequence - cdataBegin);
                        valueType = ElementAttributes_::ElementAttributes_ElementValueTypeCDATA;
                        while (_Begin[cdataSequence] != '>') ++cdataSequence;
                        element = cdataSequence;
                    }

                    // parse default value
                    else
                    {
                        int  valueBegin = tagEnd + 1;
                        int  valueEnd   = valueBegin;
                        while(valueEnd < (int)length && _Begin[valueEnd] != '<')++valueEnd;
                        while(valueEnd > valueBegin && Helpers::is_empty_symbol(_Begin[valueEnd - 1]))--valueEnd;
                        valueView = std::string_view(&_Begin[valueBegin], valueEnd - valueBegin);
                        element   = valueEnd;
                    }
                }

                // if this is a self closing tag we set a value to it's parent
                // otherwise, we set value to it
                if(_Begin[tagEnd - 1] == '/')
                {
                    if(parent.get_ref()->m_Value.empty())
                    {
                        parent.get_ref()->m_Value      = valueView;
                        parent.get_ref()->m_Attributes = parent.get_ref()->m_Attributes | valueType;
                    }

                    ElementObj newObj = document->create_node(
                        std::string_view(&_Begin[nameBegin], nameEnd - nameBegin),
                        std::string_view(),
                        ElementAttributes_::ElementAttributes_ElementTypeObject | ElementAttributes_::ElementAttributes_ElementValueTypeString);

                    if(document->append_node(newObj, parent))
                        parent = newObj;
                }
                else
                {
                    ElementObj newObj = document->create_node(
                        std::string_view(&_Begin[nameBegin], nameEnd - nameBegin),
                        valueView,
                        ElementAttributes_::ElementAttributes_ElementTypeObject | valueType);

                    if(document->append_node(newObj, parent))
                        parent = newObj;
                }

                // parse attributes
                for (int attribute = nameEnd; attribute < tagEnd; attribute++)
                {
                    if(!Helpers::is_empty_symbol(_Begin[attribute]))
                        continue;

                    while(attribute < tagEnd && (Helpers::is_empty_symbol(_Begin[attribute]) || _Begin[attribute] == '/')) ++attribute;
                    
                    if(attribute >= tagEnd)
                        continue;

                    // parse name
                    int attributeNameBegin = attribute;
                    while (attribute < tagEnd && _Begin[attribute] != '=' && !Helpers::is_empty_symbol(_Begin[attribute])) ++attribute;
                    std::string_view attributeName(&_Begin[attributeNameBegin], attribute - attributeNameBegin);

                    // parse value
                    while (attribute < tagEnd && _Begin[attribute] != '"') ++attribute;
                    int attributeValueBegin = ++attribute;
                    while (attribute < tagEnd && _Begin[attribute] != '"')++attribute;
                    std::string_view attributeValue(&_Begin[attributeValueBegin], attribute - attributeValueBegin);

                    // create new attribute
                    document->append_node(
                        document->create_node(
                            attributeName,
                            attributeValue,
                            ElementAttributes_::ElementAttributes_ElementTypeAttribute | ElementAttributes_::ElementAttributes_ElementValueTypeString),
                        parent);
                }
            }

            // go up
            if(_Begin[tagBegin] == '/' || _Begin[tagEnd - 1] == '/')
                parent = parent.get_parent();
        }
    }

    return true;
}