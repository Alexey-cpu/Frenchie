#pragma once

#include <array>
#include <iostream>
#include <functional>
#include <memory_resource>

namespace Frenchie
{
    namespace Core
    {
        namespace Serizliation
        {
            class DOMTree;
            class ElementRef;
            class ElementObj;
            class ElementItr;

            // ElementAttributes_
            enum ElementAttributes_ : int
            {
                // supported element types
                ElementAttributes_ElementTypeObject          = 1 << 0,
                ElementAttributes_ElementTypeAttribute       = 1 << 1,
                ElementAttributes_ElementTypeCollection      = 1 << 2,

                // supported element value types
                ElementAttributes_ElementValueTypeBoolean    = 1 << 3,
                ElementAttributes_ElementValueTypeFloat      = 1 << 4,
                ElementAttributes_ElementValueTypeDouble     = 1 << 5,
                ElementAttributes_ElementValueTypeLongDouble = 1 << 6,
                ElementAttributes_ElementValueTypeInt8       = 1 << 7,
                ElementAttributes_ElementValueTypeInt16      = 1 << 8,
                ElementAttributes_ElementValueTypeInt32      = 1 << 9,
                ElementAttributes_ElementValueTypeInt64      = 1 << 10,
                ElementAttributes_ElementValueTypeUint8      = 1 << 11,
                ElementAttributes_ElementValueTypeUint16     = 1 << 12,
                ElementAttributes_ElementValueTypeUint32     = 1 << 13,
                ElementAttributes_ElementValueTypeUint64     = 1 << 14,
                ElementAttributes_ElementValueTypeString     = 1 << 15,
                ElementAttributes_ElementValueTypeCDATA      = 1 << 16,
                ElementAttributes_ElementValueTypeProlog     = 1 << 17,
                ElementAttributes_ElementValueTypeComment    = 1 << 18,

                ElementAttributes_Defaults                   = ElementAttributes_ElementTypeObject | ElementAttributes_ElementValueTypeString
            };

            // ElementRef
            class ElementRef
            {
            public:
                ElementRef(const DOMTree* _Document);
                mutable std::string_view    m_Name        {std::string_view()};
                mutable std::string_view    m_Value       {std::string_view()};
                mutable ElementRef*         m_Parent      {nullptr};
                const   DOMTree*            m_Document    {nullptr};
                mutable ElementRef*         m_FirstChild  {nullptr};
                mutable ElementRef*         m_LastChild   {nullptr};
                mutable ElementRef*         m_NextSibling {nullptr};
                mutable ElementRef*         m_PrevSibling {nullptr};
                mutable int                 m_Attributes  {0};
            };

            // ElementObj
            class ElementObj
            {
            public:
                ElementObj(ElementRef* _Ref = nullptr);

                // getters
                int get_attributes() const;
                ElementRef* get_ref() const;
                const DOMTree* get_document() const;
                std::string_view get_name() const;
                std::string_view get_value() const;
                ElementObj get_next() const;
                ElementObj get_prev() const;
                ElementObj get_first() const;
                ElementObj get_last() const;
                ElementObj get_parent() const;
                const ElementItr begin() const;
                const ElementItr end() const;

                // setters
                void set_name(const std::string&);
                void set_value(const std::string&);
                void set_attributes(const int&);

                // predicates
                bool is_null() const;
                bool is_not_null() const;

                // API
                template<typename Predicate>
                ElementObj find_node(const Predicate& _Predicate) const
                {
                    for(auto& child : *this)
                    {
                        if(_Predicate(child))
                            return child;
                    }

                    return ElementObj();
                }

                ElementObj append_node(const std::string& _Name  = std::string(), const std::string& _Value = std::string());
                ElementObj append_after(const std::string& _Name = std::string(), const std::string& _Value = std::string());

                ElementObj prepend_node(const std::string& _Name = std::string(), const std::string& _Value = std::string());
                ElementObj prepend_before(const std::string& _Name = std::string(), const std::string& _Value = std::string());

                template<typename ElementBeginCallback, typename ElementEndCallback>
                void traverse(const ElementBeginCallback& _StartCallback, const ElementEndCallback& _EndCallback) const
                {
                    // the following algorithm borrowed from pugixml
                    ElementObj node  = *this;
                    ElementObj root  = *this;
                    int        depth = 0;

                    do
                    {
                        if(node != root)
                            _StartCallback(node, depth);

                        if(node.get_first().is_not_null())
                        {
                            node = node.get_first();
                            ++depth;
                            continue;
                        }

                        // continue to the next node
                        while (node != root)
                        {
                            _EndCallback(node, depth);

                            if (node.get_next().is_not_null())
                            {
                                node = node.get_next();
                                break;
                            }

                            node = node.get_parent();
                            --depth;
                        }
                    }
                    while (node != root);
                }

                void remove();

                // operators
                bool operator ==(const ElementObj& _Other);
                bool operator !=(const ElementObj& _Other);

            private:

                friend class DOMTree;

                ElementRef* m_Ref {nullptr};
            };
        
            // ElementItr
            class ElementItr final
            {
            public:
                ElementItr(const ElementObj& _Object);
                ~ElementItr();

                ElementObj  operator*() const;
                ElementObj* operator->();
                ElementItr& operator++();
                ElementItr& operator--();
                ElementItr  operator++(int);
                ElementItr  operator--(int);
                bool operator ==(const ElementItr& _Other);
                bool operator !=(const ElementItr& _Other);


            private:
                ElementObj m_Object {ElementObj(nullptr)};
            };

            // ElementDoc
            class DOMTree final
            {
            public:
                DOMTree(){}
                DOMTree(const DOMTree&) = delete;
                DOMTree& operator=(const DOMTree&) = delete;

                // getters
                ElementObj get_root() const;

                // API
                void release();

                ElementObj create_node(const std::string_view& _Name = std::string_view(), const std::string_view& _Value = std::string_view(), const int& _Attributes = ElementAttributes_::ElementAttributes_Defaults) const;
                bool append_node(const ElementObj& _Node, const ElementObj& _Parent) const;
                bool append_after(const ElementObj& _Node, const ElementObj& _Parent) const;
                bool prepend_node(const ElementObj& _Node, const ElementObj& _Parent) const;
                bool prepend_before(const ElementObj& _Node, const ElementObj& _Parent) const;

                std::string_view copy_string(const std::string& _Value = std::string()) const;

                template<typename Parser>
                bool read_string(const char* _Begin, const char* _End, const ElementObj& _TargetObj = ElementObj(nullptr))
                {
                    if(_Begin == nullptr || _End == nullptr || (size_t)(_End - _Begin) <= 0)
                        return false;

                    if(_TargetObj.is_not_null() && _TargetObj.get_document() == this)
                    {
                        std::string_view view = copy_string(std::string(_Begin, (size_t)(_End - _Begin)));
                        return Parser::read_string(_TargetObj, &view[0], &view[view.size() - 1]);
                    }

                    release();
                    std::string_view view = copy_string(std::string(_Begin, (size_t)(_End - _Begin)));
                    return Parser::read_string(get_root(), &view[0], &view[view.size() - 1]);
                }

                template<typename Parser>
                bool read_file(const std::string& _FilePath, const ElementObj& _TargetObj = ElementObj(nullptr))
                {
                    // open file
                    std::FILE* file = std::fopen(_FilePath.c_str(), "rb");
                    
                    if(!file)
                        return false;

                    // write the whole file to a buffer
                    std::fseek(file, 0, SEEK_END);
                    size_t size = std::ftell(file);
                    std::rewind(file);

                    if(_TargetObj.is_not_null() && _TargetObj.get_document() == this)
                    {
                        char* text = m_StringAllocator.allocate(size + 1);                    
                        size_t read_bytes = std::fread(text, 1, size, file);
                        text[size] = '\0';
                        std::fclose(file);
                        return Parser::read_string(_TargetObj, text, &text[size]);
                    }

                    release();
                    char* text = m_StringAllocator.allocate(size + 1);                    
                    size_t read_bytes = std::fread(text, 1, size, file);
                    text[size] = '\0';
                    std::fclose(file);
                    return Parser::read_string(get_root(), text, &text[size]);
                }

                template<typename Writer>
                bool save_file(const std::string& _FilePath, const ElementObj& _TargetObj = ElementObj(nullptr))
                {
                    return Writer::save_file(_TargetObj.is_not_null() && _TargetObj.get_document() == this ? _TargetObj : get_root(), _FilePath);
                }

            private:

                // elements allocator
                mutable std::array<std::byte, 2048>                 m_ElementsMemoryPoolBuffer {std::array<std::byte, 2048>()};
                mutable std::pmr::monotonic_buffer_resource         m_ElementsMemoryPool       {m_ElementsMemoryPoolBuffer.data(), m_ElementsMemoryPoolBuffer.size()};
                mutable std::pmr::polymorphic_allocator<ElementRef> m_ElementsAllocator        {&m_ElementsMemoryPool};

                // strings allocator
                mutable std::array<std::byte, 2048>                 m_StringMemoryPoolBuffer   {std::array<std::byte, 2048>()};
                mutable std::pmr::monotonic_buffer_resource         m_StringMemoryPool         {m_StringMemoryPoolBuffer.data(), m_StringMemoryPoolBuffer.size()};
                mutable std::pmr::polymorphic_allocator<char>       m_StringAllocator          {&m_StringMemoryPool};

                // document
                mutable ElementObj                                  m_DocumentObj              {ElementObj()};
            };

            // TextFileWriter
            template<size_t _Size = 65536>
            class TextFileWriter
            {
            public:

                bool begin(const std::string& _Path)
                {
                    m_StreamFile = fopen(_Path.c_str(), "wb");
                    
                    if(m_StreamFile == nullptr)
                        return false;
                    
                    setvbuf(m_StreamFile, m_StreamBuffer, _IOFBF, _Size);
                    return true;
                }

                void write(const char _Input[], const int _Length)
                {
                    if(m_StreamFile == nullptr)
                        return;

                    int written = 0;

                    while (written < _Length)
                    {
                        int length = std::min<int>(_Length - written, _Size - m_StringOffset);
                        memcpy(m_StringBuffer + m_StringOffset, _Input + written, length);
                        m_StringOffset += length;
                        written  += length;

                        if(m_StringOffset < _Size) continue;

                        fwrite(m_StringBuffer, 1, _Size, m_StreamFile);
                        m_StringOffset = 0;
                    }
                }

                void end()
                {
                    if(m_StringOffset <= 0 || m_StreamFile == nullptr)
                        return;
                    
                    fwrite(m_StringBuffer, 1, m_StringOffset, m_StreamFile);
                    fclose(m_StreamFile);
                    m_StringOffset = 0;
                }

            protected:

                // info
                char  m_StringBuffer[_Size]{};
                char  m_StreamBuffer[_Size]{};
                FILE* m_StreamFile  {nullptr};
                int   m_StringOffset{0};
            };

            // Format
            namespace XML
            {
                // Parser
                class Parser
                {
                public:
                    static bool read_string(const ElementObj& _Object, const char* _Begin, const char* _End);
                };

                // PrettyWriter
                class PrettyWriter
                {
                public:
                    static bool save_file(const ElementObj& _Object, const std::string& _Path);
                };

                // CompactWriter
                class CompactWriter
                {
                public:
                    static bool save_file(const ElementObj& _Object, const std::string& _Path);
                };
            }
        }
    }
}