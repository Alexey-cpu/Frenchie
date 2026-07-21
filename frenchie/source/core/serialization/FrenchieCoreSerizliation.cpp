#include <FrenchieCoreSerizliation.hpp>
#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Core::Serizliation;

ElementRef::ElementRef(const ElementDoc* _Document) : m_Document(_Document){}

// ElementObj
ElementObj::ElementObj(ElementRef* _Ref) : m_Ref(_Ref){}

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

    // generate name
    char* name = !_Name.empty() ? m_Ref->m_Document->m_StringAllocator.allocate(_Name.size() + 1) : nullptr;
    std::strcpy(name, &_Name[0]);
    name[_Name.size()] = '\0';

    char* value = !_Value.empty() ? m_Ref->m_Document->m_StringAllocator.allocate(_Value.size() + 1) : nullptr;
    std::strcpy(value, &_Value[0]);
    value[_Value.size()] = '\0';

    // generate new element
    return m_Ref->m_Document->create_element(std::string_view(name, _Name.size()), std::string_view(value, _Value.size()), *this);
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
ElementDoc::ElementDoc() : m_DocumentObj(ElementObj(create_element())){}
ElementDoc::~ElementDoc(){}

ElementObj ElementDoc::get_root() const
{
    return m_DocumentObj;
}

ElementObj ElementDoc::create_element(
    const std::string_view& _Name,
    const std::string_view& _Value,
    const ElementObj&       _Parent) const
{
    // allocate and construct element
    ElementRef* newElement = m_ElementsAllocator.allocate(1);
    m_ElementsAllocator.construct<ElementRef>(newElement, this);

    // creat object
    ElementObj::attach_child(newElement, _Parent.m_Ref);
    newElement->m_Name  = _Name;
    newElement->m_Value = _Value;

    return ElementObj(newElement);
}

bool XML::Parser::parse_string(const ElementDoc* _Document, const char* _Begin, const char* _End)
{
    ElementObj parent = _Document->get_root();
    size_t     length = (size_t)(_End - _Begin);

    for (int element = 0; element < (int)length;)
    {
        if(_Begin[element] == '<')
        {
            // retrieve tag
            int tagBegin = element + 1;
            int tagEnd   = tagBegin;
            for (;tagEnd < (int)length && _Begin[tagEnd] != '>'; tagEnd++, element = tagEnd);

            // parse attributes and name
            if(_Begin[tagBegin] != '/')
            {
                // name
                int nameBegin = tagBegin;
                int nameEnd   = tagBegin;
                for (;nameEnd < tagEnd && _Begin[nameEnd] != ' ' && _Begin[nameEnd] != '/' && _Begin[nameEnd] != '>'; nameEnd++);

                // value
                int valueBegin = tagEnd + 1;
                int valueEnd   = valueBegin;
                for (;valueEnd < (int)length && _Begin[valueEnd] != '<'; valueEnd++, element = valueEnd);

                // create a new element
                parent = _Document->create_element(
                    std::string_view(&_Begin[nameBegin], nameEnd - nameBegin),
                    std::string_view(&_Begin[valueBegin], valueEnd - valueBegin),
                    parent);

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
                            parent);
                    }
                }
            }
            
            if(_Begin[tagBegin] == '/' || _Begin[tagEnd - 1] == '/')
            {
                parent = parent.get_parent();
            }
        }
        else element++;
    }

    return true;
}

bool XML::PrettyWriter::write_file(const ElementDoc* _Document, const std::u32string& _FilePath)
{
    // nested types
    class Writer
    {
    public:
        static void write(const ElementObj& _Object, FILE* _File, const std::string& _Prefix = "")
        {
            fprintf(_File, "%s<%s>%s\n", _Prefix.c_str(), _Object.get_name().data(), _Object.get_value().data());

            for(auto& child : _Object)
                write(child, _File, _Prefix + "\t");

            fprintf(_File, "%s</%s>\n", _Prefix.c_str(), _Object.get_name().data());
        }
    };

    if(_Document == nullptr)
        return false;

    // open file
    FILE* file = std::fopen(Frenchie::Core::String::convert_utf32_to_utf8(_FilePath).c_str(), "w");

    if(file == nullptr)
        return false;

    // write file
    for (auto singletone : _Document->get_root())
        Writer::write(singletone, file, "");

    // close file
    fclose(file);

    return true;
}

bool XML::CompactWriter::write_file(const ElementDoc* _Document, const std::u32string& _FilePath)
{
    // nested types
    class Writer
    {
    public:
        static void write(const ElementObj& _Object, FILE* _File)
        {
            fprintf(_File, "<%s>%s", _Object.get_name().data(), _Object.get_value().data());

            for(auto& child : _Object)
                write(child, _File);

            fprintf(_File, "</%s>", _Object.get_name().data());
        }
    };

    // driver code
    if(_Document == nullptr)
        return false;

    // open file
    FILE* file = std::fopen(Frenchie::Core::String::convert_utf32_to_utf8(_FilePath).c_str(), "w");

    if(file == nullptr)
        return false;

    // write file
    for (auto singletone : _Document->get_root())
        Writer::write(singletone, file);

    // close file
    fclose(file);

    return true;
}