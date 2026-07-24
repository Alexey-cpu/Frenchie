#include <FrenchieCoreSerizliationXML.hpp>

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

    // auxiliry lamdas
    auto increment_untill_char_unequals_all_from_sequence = [](const char _Input[], const char _Characters[], int& _Index, const int& _MaxIndex)->int
    {
        if(_Input == nullptr || _Characters == nullptr)
            return _Index;

        int length = (int)std::strlen(_Characters);

        while (([&length](const char _Input[], const char _Characters[], const int& _Index)->bool
        {
            for (int i = 0; i < length; i++)
            {
                if(_Input[_Index] == _Characters[i])
                    return true;
            }

            return false;
        })(_Input, _Characters, _Index) && _Index < _MaxIndex)        
        {
            ++_Index;
        }

        return _Index;
    };

    auto increment_untill_char_equals_any_from_sequence = [](const char _Input[], const char _Characters[], int& _Index, const int& _MaxIndex)->int
    {
        if(_Input == nullptr || _Characters == nullptr)
            return _Index;

        int length = (int)std::strlen(_Characters);

        while (([&length](const char _Input[], const char _Characters[], const int& _Index)->bool
        {
            for (int i = 0; i < length; i++)
            {
                if(_Input[_Index] != _Characters[i])
                    return true;
            }

            return false;
        })(_Input, _Characters, _Index) && _Index < _MaxIndex)        
        {
            ++_Index;
        }

        return _Index;
    };

    auto increment_if_less_then = [](int& _Index, const int& _MaxIndex)->int
    {
        if(_Index + 1 < _MaxIndex)
            ++_Index;
        return _Index;
    };

    // parse input
    const DOMTree* document = _Object.get_document(); 
    ElementObj     parent   = _Object;
    size_t         length   = (size_t)(_End - _Begin);

    for (int element = 0; element < (int)length;)
    {
        // identify element type
        if(_Begin[element] == '<')
        {
            // parse prolog
            {
                int prologSequence = element;

                if(_Begin[increment_if_less_then(prologSequence, length)] == '?')
                {
                    int prologBegin = increment_untill_char_unequals_all_from_sequence(_Begin, "?", prologSequence, length);
                    int prologEnd   = increment_untill_char_equals_any_from_sequence(_Begin, "?", prologSequence, length);

                    document->append_node(
                        document->create_node(
                            std::string_view(),
                            std::string_view(&_Begin[prologBegin], prologEnd - prologBegin),
                            ElementAttributes_::ElementAttributes_ElementTypeObject
                            | ElementAttributes_::ElementAttributes_ElementValueTypeProlog),
                            parent);

                    element = increment_untill_char_unequals_all_from_sequence(_Begin, "?>", prologSequence, length);
                    continue;
                }
            }

            // parse comment
            {
                int commentSequence = element;

                if(
                    _Begin[increment_if_less_then(commentSequence, length)] == '!' &&
                    _Begin[increment_if_less_then(commentSequence, length)] == '-' &&
                    _Begin[increment_if_less_then(commentSequence, length)] == '-')
                {
                    int commentBegin = increment_untill_char_unequals_all_from_sequence(_Begin, "-", commentSequence, length);
                    int commentEnd   = increment_untill_char_equals_any_from_sequence(_Begin, "-", commentSequence, length);

                    document->append_node(
                        document->create_node(
                            std::string_view(),
                            std::string_view(&_Begin[commentBegin], commentEnd - commentBegin),
                            ElementAttributes_::ElementAttributes_ElementTypeObject
                            | ElementAttributes_::ElementAttributes_ElementValueTypeComment),
                            parent);

                    element = increment_untill_char_unequals_all_from_sequence(_Begin, "->", commentSequence, length);
                    continue;
                }
            }

            // parse element
            {
                // retrieve tag
                int tagBegin = ([_Begin](int _Index)->int
                {
                    while (_Begin[_Index] == '<') ++_Index;
                    return _Index;
                })(element);

                int tagEnd = tagBegin;
                for (;tagEnd < (int)length && _Begin[tagEnd] != '>'; tagEnd++, element = tagEnd);

                // parse attributes and name if this is not a closing tag
                if(_Begin[tagBegin] != '/')
                {
                    // parse name
                    int nameBegin = tagBegin;
                    int nameEnd   = tagBegin;
                    for (;nameEnd < tagEnd && _Begin[nameEnd] != ' ' && _Begin[nameEnd] != '/' && _Begin[nameEnd] != '>'; nameEnd++);

                    // parse value
                    std::string_view valueView;
                    int              valueType = ElementAttributes_::ElementAttributes_ElementValueTypeString;

                    {
                        // parse CDATA value
                        increment_untill_char_equals_any_from_sequence(_Begin, "<", element, length);
                        
                        if(
                            _Begin[increment_if_less_then(element, length)] == '!' &&
                            _Begin[increment_if_less_then(element, length)] == '[' &&
                            _Begin[increment_if_less_then(element, length)] == 'C' &&
                            _Begin[increment_if_less_then(element, length)] == 'D' &&
                            _Begin[increment_if_less_then(element, length)] == 'A' &&
                            _Begin[increment_if_less_then(element, length)] == 'T' &&
                            _Begin[increment_if_less_then(element, length)] == 'A' &&
                            _Begin[increment_if_less_then(element, length)] == '[')
                        {
                            int cdataBegin = increment_untill_char_unequals_all_from_sequence(_Begin, "[", element, length);
                            int cdataEnd   = increment_untill_char_equals_any_from_sequence(_Begin, "]", element, length);
                            increment_untill_char_unequals_all_from_sequence(_Begin, "]>", element, length);
                            valueView = std::string_view(&_Begin[cdataBegin], cdataEnd - cdataBegin);
                            valueType = ElementAttributes_::ElementAttributes_ElementValueTypeCDATA;
                        }

                        // parse default value
                        else
                        {
                            int  valueBegin = tagEnd + 1;
                            int  valueEnd   = valueBegin;
                            bool emptyValue = true;
                            for (;valueEnd < (int)length && _Begin[valueEnd] != '<'; valueEnd++, element = valueEnd)
                                emptyValue = emptyValue && Helpers::is_empty_symbol(_Begin[valueEnd]);

                            // create a new element
                            valueView = !emptyValue ? std::string_view(&_Begin[valueBegin], valueEnd - valueBegin) : std::string_view();
                        }
                    }

                    // if this is a self closing tag, then it cannot have a value, so we move parsed value to a parent object
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

                    // attributes
                    for (int attribute = nameEnd; attribute < tagEnd; attribute++)
                    {
                        int attributeSectionStart = attribute;

                        if(_Begin[attributeSectionStart] == ' ' && _Begin[increment_if_less_then(attributeSectionStart, tagEnd)] != '/')
                        {
                            while(attribute < tagEnd && _Begin[attribute] == ' ') ++attribute;
                            
                            // parse attribute name
                            int attributeNameBegin = attribute;
                            int attributeNameEnd   = attribute;
                            for (;attributeNameEnd < tagEnd && _Begin[attributeNameEnd] != '=' && _Begin[attributeNameEnd] != ' '; attributeNameEnd++);

                            // parse attribute value
                            int attributeValueBegin = [&_Begin, &tagEnd](int _Index)->int
                            {
                                while (_Index < tagEnd && _Begin[_Index] != '"') ++_Index;
                                return (++_Index);
                            }(attributeNameEnd);
                            
                            int attributeValueEnd = attributeValueBegin;
                            for (;attributeValueEnd < tagEnd && _Begin[attributeValueEnd] != '"'; attributeValueEnd++);
                            attribute = attributeValueEnd;

                            // create new attribute element
                            document->append_node(
                                document->create_node(
                                    std::string_view(&_Begin[attributeNameBegin], attributeNameEnd - attributeNameBegin),
                                    std::string_view(&_Begin[attributeValueBegin], attributeValueEnd - attributeValueBegin),
                                    ElementAttributes_::ElementAttributes_ElementTypeAttribute
                                    | ElementAttributes_::ElementAttributes_ElementValueTypeString),
                                parent);
                        }
                    }
                }
                if(_Begin[tagBegin] == '/' || _Begin[tagEnd - 1] == '/')
                {
                    parent = parent.get_parent();
                }
            }
        }
        else
        {
            element++;
        }
    }

    return true;
}