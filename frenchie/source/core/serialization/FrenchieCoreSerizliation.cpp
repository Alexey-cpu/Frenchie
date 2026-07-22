#include <FrenchieCoreSerizliation.hpp>
#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Core::Serizliation;

namespace Frenchie
{
    namespace Core
    {
        namespace Serizliation
        {
            // helpers
            class Helpers
            {
            public:

                static std::string normalize_value(const std::string& _Input)
                {
                    if(_Input.empty())
                        return std::string();

                    std::string normalized;

                    for (size_t i = 0; i < _Input.size(); i++)
                    {
                        switch (_Input[i])
                        {
                        case '<':
                            normalized.push_back('&');
                            normalized.push_back('l');
                            normalized.push_back('t');
                            normalized.push_back(';');
                            break;
                        
                        case '>':
                            normalized.push_back('&');
                            normalized.push_back('g');
                            normalized.push_back('t');
                            normalized.push_back(';');
                            break;

                        case '&':
                            normalized.push_back('&');
                            normalized.push_back('a');
                            normalized.push_back('m');
                            normalized.push_back('p');
                            normalized.push_back(';');
                            break;

                        case '\'':
                            normalized.push_back('&');
                            normalized.push_back('a');
                            normalized.push_back('p');
                            normalized.push_back('o');
                            normalized.push_back('s');
                            normalized.push_back(';');
                            break;

                        case '"':
                            normalized.push_back('&');
                            normalized.push_back('q');
                            normalized.push_back('u');
                            normalized.push_back('o');
                            normalized.push_back('t');
                            normalized.push_back(';');
                            break;

                        default:
                            normalized.push_back(_Input[i]);
                            break;
                        }
                    }

                    return normalized;
                }

                static std::string normalize_name(const std::string& _Input)
                {
                    if(_Input.empty())
                        return std::string();

                    std::string normalized;

                    for (size_t i = 0; i < _Input.size(); i++)
                    {
                        if(
                            (i == 0 &&
                                _Input[i] > '0' &&
                                _Input[i] < '9') ||

                            (_Input[i] != '!'  &&
                             _Input[i] != '"'  &&
                             _Input[i] != '#'  &&
                             _Input[i] != '$'  &&
                             _Input[i] != '%'  &&
                             _Input[i] != '&'  &&
                             _Input[i] != '\'' &&
                             _Input[i] != '\\' &&
                             _Input[i] != '/'  &&
                             _Input[i] != '('  &&
                             _Input[i] != ')'  &&
                             _Input[i] != '*'  &&
                             _Input[i] != '+'  &&
                             _Input[i] != '-'  &&
                             _Input[i] != '.'  &&
                             _Input[i] != ','  &&
                             _Input[i] != ';'  &&
                             _Input[i] != '<'  &&
                             _Input[i] != '>'  &&
                             _Input[i] != '='  &&
                             _Input[i] != '?'  &&
                             _Input[i] != '@'  &&
                             _Input[i] != '['  &&
                             _Input[i] != ']'  &&
                             _Input[i] != '^'  &&
                             _Input[i] != '{'  &&
                             _Input[i] != '}'  &&
                             _Input[i] != '|'  &&
                             _Input[i] != '~'))
                        {
                            normalized.push_back(_Input[i]);
                        }
                    }

                    return normalized;
                }
            };
        }
    }
}

ElementRef::ElementRef(const DOMTree* _Document) : m_Document(_Document){}

// ElementObj
ElementObj::ElementObj(ElementRef* _Ref) : m_Ref(_Ref){}

int ElementObj::get_type() const
{
    return m_Ref != nullptr ? m_Ref->m_Type : -1;
}

std::string_view ElementObj::get_name() const
{
    return m_Ref != nullptr ? m_Ref->m_Name : std::string_view();
}

std::string_view ElementObj::get_value() const
{
    return m_Ref != nullptr ? m_Ref->m_Value : std::string_view();
} 

ElementObj ElementObj::get_next() const
{
    return m_Ref != nullptr ? ElementObj(m_Ref->m_NextSibling) : ElementObj();
}

ElementObj ElementObj::get_prev() const
{
    return m_Ref != nullptr ? ElementObj(m_Ref->m_PrevSibling) : ElementObj();
}

ElementObj ElementObj::get_first() const
{
    return m_Ref != nullptr ? ElementObj(m_Ref->m_FirstChild) : ElementObj();
}

ElementObj ElementObj::get_last() const
{
    return m_Ref != nullptr ? ElementObj(m_Ref->m_LastChild) : ElementObj();
}

ElementObj ElementObj::get_parent() const
{
    return m_Ref != nullptr ? ElementObj(m_Ref->m_Parent) : ElementObj();
}

void ElementObj::set_name(const std::string& _Value)
{
    if(m_Ref == nullptr) return;

    // generate normalized value
    std::string normalized = Helpers::normalize_name(_Value);
    char* value = !normalized.empty() ? m_Ref->m_Document->m_StringAllocator.allocate(normalized.size() + 1) : nullptr;
    std::strcpy(value, &normalized[0]);
    value[normalized.size()] = '\0';

    // generate string view
    m_Ref->m_Name = std::string_view(value, normalized.size());
}

void ElementObj::set_value(const std::string& _Value)
{
    if(m_Ref == nullptr) return;

    std::string normalized = Helpers::normalize_value(_Value);
    char* value = !normalized.empty() ? m_Ref->m_Document->m_StringAllocator.allocate(normalized.size() + 1) : nullptr;
    std::strcpy(value, &normalized[0]);
    value[normalized.size()] = '\0';

    m_Ref->m_Value = std::string_view(value, normalized.size());
}

void ElementObj::set_type(const int& _Attributes)
{
    if(m_Ref != nullptr)
        m_Ref->m_Type = _Attributes;
}

const ElementItr ElementObj::begin() const
{
    return ElementItr(get_first());
}

const ElementItr ElementObj::end() const
{
    return ElementItr(ElementObj(nullptr));
}

bool ElementObj::is_null() const
{
    return m_Ref == nullptr;
}

bool ElementObj::is_not_null() const
{
    return m_Ref != nullptr;
}

ElementObj ElementObj::append_node(const std::string& _Name, const std::string& _Value)
{
    if(m_Ref == nullptr || m_Ref->m_Document == nullptr)
        return ElementObj();

    // generate new element
    ElementObj obj = m_Ref->m_Document->create_element(std::string_view(), std::string_view(), *this);
    obj.set_name(_Name);
    obj.set_value(_Value);
    return obj;
}

void ElementObj::remove()
{
    ElementObj::detach_child(m_Ref);
}

bool ElementObj::operator ==(const ElementObj& _Other)
{
    return m_Ref == _Other.m_Ref;
}

bool ElementObj::operator !=(const ElementObj& _Other)
{
    return m_Ref != _Other.m_Ref;
}

void ElementObj::attach_child(ElementRef* _Who, ElementRef* _Where)
{
    if(_Who == nullptr || _Where == nullptr || _Who->m_Document != _Where->m_Document)
        return;

    // detach from parent
    if(_Who->m_Parent != nullptr)
        ElementObj::detach_child(_Who);

    // this is the first child
    if(_Where->m_FirstChild == nullptr)
    {
        _Where->m_FirstChild = _Who;
        _Who->m_Parent = _Where;
        return;
    }

    // this is the second child
    if(_Where->m_LastChild == nullptr)
    {
        _Where->m_LastChild = _Who;
        _Where->m_FirstChild->m_NextSibling = _Where->m_LastChild;
        _Where->m_LastChild->m_PrevSibling = _Where->m_FirstChild;
        _Who->m_Parent = _Where;
        return;
    }

    // this is the next child
    _Where->m_LastChild->m_NextSibling = _Who;
    _Who->m_PrevSibling = _Where->m_LastChild;
    _Where->m_LastChild = _Who;
    _Who->m_Parent = _Where;
}

void ElementObj::detach_child(ElementRef* _This)
{
    if(_This == nullptr)
        return;

    // unlink child
    if(_This->m_NextSibling != nullptr)
        _This->m_NextSibling->m_PrevSibling = _This->m_PrevSibling;

    if(_This->m_PrevSibling != nullptr)
        _This->m_PrevSibling->m_NextSibling = _This->m_NextSibling;

    _This->m_Parent = nullptr;
}

// ElementItr
ElementItr::ElementItr(const ElementObj& _Object) : m_Object(_Object){}
ElementItr::~ElementItr(){}

ElementObj  ElementItr::operator*() const
{
    return m_Object;
}

ElementObj* ElementItr::operator->()
{
    return &m_Object;
}

ElementItr& ElementItr::operator++()
{
    m_Object = m_Object.get_next();
    return *this;
}

ElementItr& ElementItr::operator--()
{
    m_Object = m_Object.get_prev();
    return *this;
}

ElementItr  ElementItr::operator++(int)
{
    ++(*this);
    return *this;
}

ElementItr  ElementItr::operator--(int)
{
    --(*this);
    return *this;
}

bool ElementItr::operator ==(const ElementItr& _Other)
{
    return m_Object == _Other.m_Object;
}

bool ElementItr::operator !=(const ElementItr& _Other)
{
    return m_Object != _Other.m_Object;
}

// Document
DOMTree::DOMTree() : m_DocumentObj(ElementObj(create_element())){}
DOMTree::~DOMTree(){}

ElementObj DOMTree::get_root() const
{
    return m_DocumentObj;
}

ElementObj DOMTree::create_element(const std::string_view& _Name, const std::string_view& _Value, const ElementObj& _Parent, const int& _Type) const
{
    // allocate and construct element
    ElementRef* newElement = m_ElementsAllocator.allocate(1);
    m_ElementsAllocator.construct<ElementRef>(newElement, this);

    // creat object
    ElementObj::attach_child(newElement, _Parent.m_Ref);
    newElement->m_Name  = _Name;
    newElement->m_Value = _Value;
    newElement->m_Type  = _Type;

    return ElementObj(newElement);
}

bool XML::Parser::parse_string(const DOMTree* _Document, const char* _Begin, const char* _End)
{
    // check inputs
    if(_Document == nullptr || _Begin == nullptr || _End == nullptr)
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
    ElementObj parent = _Document->get_root();
    size_t     length = (size_t)(_End - _Begin);

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

                    _Document->create_element(
                        std::string_view(),
                        std::string_view(&_Begin[prologBegin], prologEnd - prologBegin),
                        parent,
                        XML::Types::Prolog);

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

                    _Document->create_element(
                        std::string_view(),
                        std::string_view(&_Begin[commentBegin], commentEnd - commentBegin),
                        parent,
                        XML::Types::Comment);

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

                // parse attributes and name
                if(_Begin[tagBegin] != '/')
                {
                    // parse name
                    int nameBegin = tagBegin;
                    int nameEnd   = tagBegin;
                    for (;nameEnd < tagEnd && _Begin[nameEnd] != ' ' && _Begin[nameEnd] != '/' && _Begin[nameEnd] != '>'; nameEnd++);

                    // parse CDATA
                    std::string_view valueView;

                    {
                        int cdataSequence      = tagEnd;
                        int cdataSequenceBegin = increment_untill_char_equals_any_from_sequence(_Begin, "<", cdataSequence, length);
                        
                        if(
                            _Begin[increment_if_less_then(cdataSequence, length)] == '!' &&
                            _Begin[increment_if_less_then(cdataSequence, length)] == '[' &&
                            _Begin[increment_if_less_then(cdataSequence, length)] == 'C' &&
                            _Begin[increment_if_less_then(cdataSequence, length)] == 'D' &&
                            _Begin[increment_if_less_then(cdataSequence, length)] == 'A' &&
                            _Begin[increment_if_less_then(cdataSequence, length)] == 'T' &&
                            _Begin[increment_if_less_then(cdataSequence, length)] == 'A' &&
                            _Begin[increment_if_less_then(cdataSequence, length)] == '[')
                        {                            
                            increment_untill_char_equals_any_from_sequence(_Begin, "]", cdataSequence, length);
                            element = increment_untill_char_unequals_all_from_sequence(_Begin, "]>", cdataSequence, length);
                            valueView = std::string_view(&_Begin[cdataSequenceBegin], element - cdataSequenceBegin);
                        }

                        // parse default value
                        else
                        {
                            int valueBegin = tagEnd + 1;
                            int valueEnd   = valueBegin;
                            for (;valueEnd < (int)length && _Begin[valueEnd] != '<'; valueEnd++, element = valueEnd);

                            // create a new element
                            valueView = std::string_view(&_Begin[valueBegin], valueEnd - valueBegin);
                        }
                    }

                    parent = _Document->create_element(
                        std::string_view(&_Begin[nameBegin], nameEnd - nameBegin),
                        valueView,
                        parent,
                        XML::Types::Tag);

                    // attributes
                    for (int attribute = nameEnd; attribute < tagEnd; attribute++)
                    {
                        if(_Begin[attribute] == ' ')
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
                            _Document->create_element(
                                std::string_view(&_Begin[attributeNameBegin], attributeNameEnd - attributeNameBegin),
                                std::string_view(&_Begin[attributeValueBegin], attributeValueEnd - attributeValueBegin),
                                parent,
                                XML::Types::Attribute);
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

bool XML::CompactWriter::write_file(const ElementObj& _Object, const std::string& _FilePath)
{
    // driver code
    if(_Object.is_null())
        return false;

    // open file
    FILE* file = std::fopen(_FilePath.c_str(), "wb");

    if(file == nullptr)
        return false;

    // traverse and write
    _Object.traverse(
        [file](const ElementObj& node, const int& depth)
        {
            switch (node.get_type())
            {
            case XML::Types::Tag:
            
            // being self
            fprintf(file, "<%.*s", (int)node.get_name().size(), node.get_name().data());

            // write attributres
            for(auto& child : node)
            {
                if(child.get_type() == XML::Types::Attribute)
                    fprintf(file, " %.*s=\"%.*s\"", (int)child.get_name().size(), child.get_name().data(), (int)child.get_value().size(), child.get_value().data());
            }

            // end self
            fprintf(file, ">%.*s", (int)node.get_value().size(), node.get_value().data());

                break;
                
            case XML::Types::Prolog:
            fprintf(file, "<?%.*s?>", (int)node.get_value().size(), node.get_value().data());
                break;
                
            case XML::Types::Comment:
            fprintf(file, "<!--%.*s-->", (int)node.get_value().size(), node.get_value().data());
                break;
            }
        },
        [file](const ElementObj& node, const int& depth)
        {
            switch (node.get_type())
            {
            case XML::Types::Tag:
            fprintf(file, "</%.*s>\n", (int)node.get_name().size(), node.get_name().data());
                break;
            default:
                break;
            }
        }
    );

    // close file
    fclose(file);

    return true;
}

bool XML::PrettyWriter::write_file(const ElementObj& _Object, const std::string& _FilePath)
{
    // driver code
    if(_Object.is_null())
        return false;

    // open file
    FILE* file = std::fopen(_FilePath.c_str(), "wb");

    if(file == nullptr)
        return false;

    // traverse and write
    _Object.traverse(
        [file](const ElementObj& node, const int& depth)
        {
            switch (node.get_type())
            {
            case XML::Types::Tag:
            for (int i = 0; i < depth - 1; i++)
                fprintf(file, "%s", "\t");
            
            // being self
            fprintf(file, "<%.*s", (int)node.get_name().size(), node.get_name().data());

            // write attributres
            for(auto& child : node)
            {
                if(child.get_type() == XML::Types::Attribute)
                    fprintf(file, " %.*s=\"%.*s\"", (int)child.get_name().size(), child.get_name().data(), (int)child.get_value().size(), child.get_value().data());
            }

            // end self
            fprintf(file, ">%.*s", (int)node.get_value().size(), node.get_value().data());
                break;
                
            case XML::Types::Prolog:
            fprintf(file, "<?%.*s?>\n", (int)node.get_value().size(), node.get_value().data());
                break;
                
            case XML::Types::Comment:
            fprintf(file, "<!--%.*s-->\n", (int)node.get_value().size(), node.get_value().data());
                break;
            }
        },
        [file](const ElementObj& node, const int& depth)
        {
            switch (node.get_type())
            {
            case XML::Types::Tag:
            for (int i = 0; i < depth - 1; i++)
                fprintf(file, "%s", "\t");
            fprintf(file, "</%.*s>\n", (int)node.get_name().size(), node.get_name().data());
                break;
            
            default:
                break;
            }
        }
    );

    // close file
    fclose(file);

    return true;
}
