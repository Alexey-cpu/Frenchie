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
                ElementObj append_node(const std::string& _Name  = std::string(), const std::string& _Value = std::string());
                void remove();

                // operators
                bool operator ==(const ElementObj& _Other);
                bool operator !=(const ElementObj& _Other);

            private:

                friend class DOMTree;

                ElementRef* m_Ref {nullptr};

                // service methods
                static void attach_child(ElementRef* _Who, ElementRef* _Where);
                static void detach_child(ElementRef* _This);
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

                DOMTree();
                ~DOMTree();

                template<typename Parser>
                bool load_file(const std::string& _FilePath)
                {
                    // open file
                    std::FILE* file = std::fopen(_FilePath.c_str(), "rb");
                    
                    if(!file)
                        return false;

                    // write the whole file to a buffer
                    std::fseek(file, 0, SEEK_END);
                    size_t size = std::ftell(file);
                    std::rewind(file);

                    char* text = m_StringAllocator.allocate(size + 1);                    
                    size_t read_bytes = std::fread(text, 1, size, file);
                    text[size] = '\0';
                    std::fclose(file);

                    // parse buffer
                    return Parser::parse_string(this, text, &text[size]);
                }

                template<typename Writer>
                bool write_file(const std::u32string& _FilePath)
                {
                    return Writer::write_file(this, _FilePath);
                }

                ElementObj get_root() const;

                ElementObj create_element(
                    const std::string_view& _Name       = std::string_view(),
                    const std::string_view& _Value      = std::string_view(),
                    const ElementObj&       _Parent     = ElementObj(nullptr),
                    const int&              _Attributes = 0) const;

            private:

                friend class ElementObj;

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

            // Format
            class XML
            {
            public:

                class Parser
                {
                public:
                    static bool parse_string(const DOMTree* _Document, const char* _Begin, const char* _End);
                };

                class CompactWriter
                {
                public:
                    static bool write_file(const DOMTree* _Document, const std::u32string& _FilePath);
                };

                class PrettyWriter
                {
                public:
                    static bool write_file(const DOMTree* _Document, const std::u32string& _FilePath);
                };
            };
        }
    }
}