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

                // name/value normalization
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

                static bool is_empty_symbol(const char& _Symbol)
                {
                    return _Symbol == '\t' || _Symbol == '\n' || _Symbol == '\0' || _Symbol == '\r' || _Symbol == ' ';
                }

                // tree nodes
                static void detach_child(ElementRef* _This)
                {
                    if(_This == nullptr)
                        return;

                    // unlink child
                    if(_This->m_NextSibling != nullptr)
                        _This->m_NextSibling->m_PrevSibling = _This->m_PrevSibling;

                    if(_This->m_PrevSibling != nullptr)
                        _This->m_PrevSibling->m_NextSibling = _This->m_NextSibling;

                    // reset parent last child pointer
                    if(_This->m_Parent != nullptr && _This->m_Parent->m_LastChild == _This)
                    {
                        _This->m_Parent->m_LastChild =
                            _This->m_NextSibling != nullptr ?
                                _This->m_NextSibling :
                                    _This->m_PrevSibling;
                    }

                    // reset parent fist child pointer
                    if(_This->m_Parent != nullptr && _This->m_Parent->m_FirstChild == _This)
                    {
                        _This->m_Parent->m_FirstChild =
                            _This->m_PrevSibling != nullptr ?
                                _This->m_PrevSibling :
                                    _This->m_NextSibling;
                    }

                    // nullify pointers
                    _This->m_Parent      = nullptr;
                    _This->m_NextSibling = nullptr;
                    _This->m_PrevSibling = nullptr;
                }

                static bool attach_child_to_end(ElementRef* _Node, ElementRef* _Parent)
                {
                    if(_Node == nullptr || _Parent == nullptr || _Node->m_Document != _Parent->m_Document)
                        return false;

                    if(_Node->m_Parent != nullptr)
                        Helpers::detach_child(_Node);
                    _Node->m_Parent = _Parent;

                    if(_Parent->m_FirstChild == nullptr)
                    {
                        _Parent->m_FirstChild = _Node;
                        if(_Parent->m_LastChild == nullptr)
                            _Parent->m_LastChild = _Node;
                        return true;
                    }

                    if(_Parent->m_LastChild == nullptr)
                    {
                        _Parent->m_LastChild = _Node;
                        if(_Parent->m_FirstChild == nullptr)
                            _Parent->m_FirstChild = _Node;
                        return true;
                    }

                    _Parent->m_LastChild->m_NextSibling = _Node;
                    _Node->m_PrevSibling = _Parent->m_LastChild;
                    _Parent->m_LastChild = _Parent->m_LastChild->m_NextSibling;
                    return true;
                }

                static bool attach_child_to_front(ElementRef* _Node, ElementRef* _Parent)
                {
                    if(_Node == nullptr || _Parent == nullptr || _Node->m_Document != _Parent->m_Document)
                        return false;

                    if(_Node->m_Parent != nullptr)
                        Helpers::detach_child(_Node);
                    _Node->m_Parent = _Parent;

                    if(_Parent->m_FirstChild == nullptr)
                    {
                        _Parent->m_FirstChild = _Node;
                        if(_Parent->m_LastChild == nullptr)
                            _Parent->m_LastChild = _Node;
                        return true;
                    }

                    if(_Parent->m_LastChild == nullptr)
                    {
                        _Parent->m_LastChild = _Node;
                        if(_Parent->m_FirstChild == nullptr)
                            _Parent->m_FirstChild = _Node;
                        return true;
                    }

                    _Parent->m_FirstChild->m_PrevSibling = _Node;
                    _Node->m_NextSibling  = _Parent->m_FirstChild;
                    _Parent->m_FirstChild = _Parent->m_FirstChild->m_PrevSibling;
                    return true;
                }

                static bool attach_child_before(ElementRef* _Node, ElementRef* _Before)
                {
                    if(_Node == nullptr || _Before == nullptr || _Before->m_Parent == nullptr || _Node->m_Document != _Before->m_Document)
                        return false;

                    if(_Node->m_Parent != nullptr)
                        Helpers::detach_child(_Node);
                    _Node->m_Parent = _Before->m_Parent;

                    if(_Before->m_Parent->m_FirstChild == nullptr && _Before->m_Parent->m_LastChild == nullptr)
                        return Helpers::attach_child_to_front(_Node, _Before);

                    ElementRef* prevOfBefore = _Before->m_PrevSibling;

                    if(prevOfBefore != nullptr)
                        prevOfBefore->m_NextSibling = _Node;
                    _Node->m_PrevSibling = prevOfBefore;

                    _Before->m_PrevSibling = _Node;
                    _Node->m_NextSibling = _Before;

                    ElementRef* prev = _Before->m_Parent->m_FirstChild->m_PrevSibling;

                    while (prev)
                    {
                        _Before->m_Parent->m_FirstChild = prev;
                        prev = prev->m_PrevSibling;
                    }

                    return true;
                }

                static bool attach_child_after(ElementRef* _Who, ElementRef* _After)
                {
                    if(_Who == nullptr || _After == nullptr || _After->m_Parent == nullptr || _Who->m_Document != _After->m_Document)
                        return false;

                    if(_Who->m_Parent != nullptr)
                        Helpers::detach_child(_Who);
                    _Who->m_Parent = _After->m_Parent;

                    if(_After->m_Parent->m_FirstChild == nullptr && _After->m_Parent->m_LastChild == nullptr)
                        return Helpers::attach_child_to_end(_Who, _After);

                    ElementRef* nextOfAfter = _After->m_NextSibling;

                    if(nextOfAfter != nullptr)
                        nextOfAfter->m_PrevSibling = _Who;
                    _Who->m_NextSibling = nextOfAfter;

                    _After->m_NextSibling = _Who;
                    _Who->m_PrevSibling = _After;

                    ElementRef* next = _After->m_Parent->m_LastChild->m_NextSibling;

                    while (next)
                    {
                        _After->m_Parent->m_LastChild = next;
                        next = next->m_NextSibling;
                    }

                    return true;
                }
            };
        }
    }
}

ElementRef::ElementRef(const DOMTree* _Document) : m_Document(_Document){}

// ElementObj
ElementObj::ElementObj(ElementRef* _Ref) : m_Ref(_Ref){}

int ElementObj::get_attributes() const
{
    return m_Ref != nullptr ? m_Ref->m_Attributes : -1;
}

ElementRef* ElementObj::get_ref() const
{
    return m_Ref;
}

const DOMTree* ElementObj::get_document() const
{
    return m_Ref != nullptr ? m_Ref->m_Document : nullptr;
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
    if(m_Ref != nullptr)
        m_Ref->m_Name = m_Ref->m_Document->copy_string(Helpers::normalize_name(_Value));
}

void ElementObj::set_value(const std::string& _Value)
{
    if(m_Ref != nullptr)
        m_Ref->m_Value = m_Ref->m_Document->copy_string(Helpers::normalize_value(_Value));
}

const ElementItr ElementObj::begin() const
{
    return ElementItr(get_first());
}

void ElementObj::set_attributes(const int& _Attributes)
{
    if(m_Ref != nullptr)
        m_Ref->m_Attributes = _Attributes;
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

    ElementObj obj = m_Ref->m_Document->create_node();
    obj.set_name(_Name);
    obj.set_value(_Value);

    if(m_Ref->m_Document->append_node(obj, *this))
        return obj;

    obj.remove();
    return ElementObj();
}

ElementObj ElementObj::append_after(const std::string& _Name, const std::string& _Value)
{
    if(m_Ref == nullptr || m_Ref->m_Document == nullptr)
        return ElementObj();

    ElementObj obj = m_Ref->m_Document->create_node();
    obj.set_name(_Name);
    obj.set_value(_Value);

    if(m_Ref->m_Document->append_after(obj, *this))
        return obj;

    obj.remove();
    return ElementObj();
}

ElementObj ElementObj::prepend_node(const std::string& _Name, const std::string& _Value)
{
    if(m_Ref == nullptr || m_Ref->m_Document == nullptr)
        return ElementObj();

    ElementObj obj = m_Ref->m_Document->create_node();
    obj.set_name(_Name);
    obj.set_value(_Value);
    
    if(m_Ref->m_Document->prepend_node(obj, *this))
        return obj;

    obj.remove();
    return ElementObj();
}

ElementObj ElementObj::prepend_before(const std::string& _Name, const std::string& _Value)
{
    if(m_Ref == nullptr || m_Ref->m_Document == nullptr)
        return ElementObj();

    ElementObj obj = m_Ref->m_Document->create_node();
    obj.set_name(_Name);
    obj.set_value(_Value);

    if(m_Ref->m_Document->prepend_before(obj, *this))
        return obj;

    obj.remove();
    return ElementObj();
}

void ElementObj::remove()
{
    Helpers::detach_child(m_Ref);
}

bool ElementObj::operator ==(const ElementObj& _Other)
{
    return m_Ref == _Other.m_Ref;
}

bool ElementObj::operator !=(const ElementObj& _Other)
{
    return m_Ref != _Other.m_Ref;
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

// DOMTree
ElementObj DOMTree::get_root() const
{
    if(m_DocumentObj.is_null())
        m_DocumentObj = create_node();
    return m_DocumentObj;
}

void DOMTree::release()
{
    // release memory pools
    m_ElementsMemoryPool.release();
    m_StringMemoryPool.release();

    // nullify root
    m_DocumentObj = ElementObj(nullptr);
}

ElementObj DOMTree::create_node(const std::string_view& _Name, const std::string_view& _Value, const int& _Attributes) const
{
    // allocate and construct element
    ElementRef* newElement = m_ElementsAllocator.allocate(1);
    m_ElementsAllocator.construct<ElementRef>(newElement, this);

    // creat object
    newElement->m_Name        = _Name;
    newElement->m_Value       = _Value;
    newElement->m_Attributes  = _Attributes;

    return ElementObj(newElement);
}

bool DOMTree::append_node(const ElementObj& _Node, const ElementObj& _Parent) const
{
    return Helpers::attach_child_to_end(_Node.m_Ref, _Parent.m_Ref);
}

bool DOMTree::append_after(const ElementObj& _Node, const ElementObj& _Parent) const
{
    return Helpers::attach_child_after(_Node.m_Ref, _Parent.m_Ref);
}

bool DOMTree::prepend_node(const ElementObj& _Node, const ElementObj& _Parent) const
{
    return Helpers::attach_child_to_front(_Node.m_Ref, _Parent.m_Ref);
}

bool DOMTree::prepend_before(const ElementObj& _Node, const ElementObj& _Parent) const
{
    return Helpers::attach_child_before(_Node.m_Ref, _Parent.m_Ref);
}

std::string_view DOMTree::copy_string(const std::string& _Value) const
{
    if(_Value.empty())
        return std::string_view();

    char* value = m_StringAllocator.allocate(_Value.size() + 1);
    std::strcpy(value, &_Value[0]);
    value[_Value.size()] = '\0';

    return std::string_view(value, _Value.size());
}

bool XML::Parser::read_string(const ElementObj& _Object, const char* _Begin, const char* _End)
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

                    {
                        // parse CDATA value
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
                    bool selfClosingTag = _Begin[tagEnd - 1] != '/';
                    if(!selfClosingTag && parent.get_ref()->m_Value.empty())
                        parent.get_ref()->m_Value = valueView;

                    ElementObj newObj = document->create_node(
                        std::string_view(&_Begin[nameBegin], nameEnd - nameBegin),
                        selfClosingTag ? valueView : std::string_view(),
                        ElementAttributes_::ElementAttributes_ElementTypeObject
                        | ElementAttributes_::ElementAttributes_ElementValueTypeString);

                    if(document->append_node(newObj, parent))
                        parent = newObj;

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

bool XML::CompactWriter::save_file(const ElementObj& _Object, const std::string& _Path)
{
    TextFileWriter writer;

    if(_Object.is_null() || !writer.begin(_Path))
    {
        writer.end();
        return false;
    }

    // traverse and write
    _Object.traverse(
        [&writer](const ElementObj& _Node, const int&)
        {
            if(!(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject))
                return;

            // write prolog
            if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeProlog)
            {
                writer.write("<?", 2);
                writer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                writer.write("?>", 2);
            }
            // write comment
            else if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeComment)
            {
                writer.write("<!--", 4);
                writer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                writer.write("-->", 3);
            }
            // write default element
            else
            {
                writer.write("<", 1);
                writer.write(_Node.get_name().data(), (int)_Node.get_name().size());

                // write attributres
                for(auto& child : _Node)
                {
                    if(child.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeAttribute)
                    {
                        writer.write(" ", 1);
                        writer.write(child.get_name().data(), (int)child.get_name().size());
                        writer.write("=\"", 2);
                        writer.write(child.get_value().data(), (int)child.get_value().size());
                        writer.write("\"", 1);
                    }
                }

                writer.write(">", 1);
                writer.write(_Node.get_value().data(), (int)_Node.get_value().size());
            }
        },
        [&writer](const ElementObj& _Node, const int&)
        {
            if(
                 (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject)      &&
                !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeAttribute)   &&
                !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeProlog) &&
                !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeComment))
            {                
                writer.write("</", 2);
                writer.write(_Node.get_name().data(), (int)_Node.get_name().size());
                writer.write(">", 1);
            }
        }
    );

    // close file
    writer.end();

    return true;
}

bool XML::PrettyWriter::save_file(const ElementObj& _Object, const std::string& _Path)
{
    TextFileWriter writer;

    if(_Object.is_null() || !writer.begin(_Path))
    {
        writer.end();
        return false;
    }

    _Object.traverse(
        [&writer](const ElementObj& _Node, const int& _Depth)
        {
            if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject)
            {
                // write prolog
                if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeProlog)
                {
                    writer.write("<?", 2);
                    writer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                    writer.write("?>\n", 3);
                }
                // write comment
                else if(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeComment)
                {
                    writer.write("<!--", 4);
                    writer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                    writer.write("-->\n", 4);
                }
                // write default element
                else
                {
                    for (int i = 0; i < _Depth - 1; i++)
                        writer.write("\t", 1);

                    writer.write("<", 1);
                    writer.write(_Node.get_name().data(), (int)_Node.get_name().size());

                    // write attributres
                    for(auto& child : _Node)
                    {
                        if(child.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeAttribute)
                        {
                            writer.write(" ", 1);
                            writer.write(child.get_name().data(), (int)child.get_name().size());
                            writer.write("=\"", 2);
                            writer.write(child.get_value().data(), (int)child.get_value().size());
                            writer.write("\"", 1);
                        }
                    }

                    writer.write(">", 1);
                    writer.write(_Node.get_value().data(), (int)_Node.get_value().size());
                    writer.write("\n", 1);
                }
            }
        },
        [&writer](const ElementObj& _Node, const int& _Depth)
        {
            if(
                 (_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeObject)      &&
                !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementTypeAttribute)   &&
                !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeProlog) &&
                !(_Node.get_attributes() & ElementAttributes_::ElementAttributes_ElementValueTypeComment))
            {
                for (int i = 0; i < _Depth - 1; i++)
                    writer.write("\t", 1);
                
                writer.write("</", 2);
                writer.write(_Node.get_name().data(), (int)_Node.get_name().size());
                writer.write(">\n", 2);
            }
        }
    );

    writer.end();

    return true;
}
