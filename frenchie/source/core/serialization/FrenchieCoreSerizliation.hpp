#pragma once

#include <array>
#include <string.h>
#include <iostream>
#include <functional>
#include <memory_resource>

/*! \defgroup <Core> (Core)
 *  @brief The module contains core utility functions and classes.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Core
    {
        namespace Serizliation
        {
            /*! \defgroup <Serizliation> (Serizliation)
            *  @ingroup Core
            *  @brief The module contains core utility classes for serialization/deserialization.
            * @{
            */

            class DOMTree;
            class ElementRef;
            class ElementObj;
            class ElementItr;

            /**
             * @brief This enum declares DOM tree node attributes
             * @enum ElementAttributes_
             */
            enum ElementAttributes_ : int
            {
                // supported element types
                ElementAttributes_ElementTypeObject          = 1 << 0,  ///< if true, the DOM tree node is an object
                ElementAttributes_ElementTypeAttribute       = 1 << 1,  ///< if true, the DOM tree node is an attribute
                ElementAttributes_ElementTypeCollection      = 1 << 2,  ///< if true, the DOM tree node is an collection of nodes or attributes

                // supported element value types
                ElementAttributes_ElementValueTypeBoolean    = 1 << 3,  ///< if true, the type of DOM tree node value is boolean
                ElementAttributes_ElementValueTypeFloat      = 1 << 4,  ///< if true, the type of DOM tree node value is float
                ElementAttributes_ElementValueTypeDouble     = 1 << 5,  ///< if true, the type of DOM tree node value is double
                ElementAttributes_ElementValueTypeLongDouble = 1 << 6,  ///< if true, the type of DOM tree node value is long double
                ElementAttributes_ElementValueTypeInt8       = 1 << 7,  ///< if true, the type of DOM tree node value is int8
                ElementAttributes_ElementValueTypeInt16      = 1 << 8,  ///< if true, the type of DOM tree node value is int16
                ElementAttributes_ElementValueTypeInt32      = 1 << 9,  ///< if true, the type of DOM tree node value is int32
                ElementAttributes_ElementValueTypeInt64      = 1 << 10, ///< if true, the type of DOM tree node value is int64
                ElementAttributes_ElementValueTypeUint8      = 1 << 11, ///< if true, the type of DOM tree node value is Uint8
                ElementAttributes_ElementValueTypeUint16     = 1 << 12, ///< if true, the type of DOM tree node value is Uint16
                ElementAttributes_ElementValueTypeUint32     = 1 << 13, ///< if true, the type of DOM tree node value is Uint32
                ElementAttributes_ElementValueTypeUint64     = 1 << 14, ///< if true, the type of DOM tree node value is Uint64
                ElementAttributes_ElementValueTypeString     = 1 << 15, ///< if true, the type of DOM tree node value is string
                ElementAttributes_ElementValueTypeCDATA      = 1 << 16, ///< if true, the type of DOM tree node value is character data (CDATA, it's supported only by XML parser and writer)
                ElementAttributes_ElementValueTypeProlog     = 1 << 17, ///< if true, the type of DOM tree node value is prolog (it's supported only by XML parser and writer)
                ElementAttributes_ElementValueTypeComment    = 1 << 18, ///< if true, the type of DOM tree node value is comment

                ElementAttributes_Defaults                   = ElementAttributes_ElementTypeObject | ElementAttributes_ElementValueTypeString
            };

            /**
             * @brief This class defines a reference to DOM tree node
             * @class ElementRef
             */
            class ElementRef final
            {
            public:

                /**
                 * @brief Constructs a new Element Ref object
                 * @param _Document document to which this DOM tree node ref belongs to
                 */
                ElementRef(const DOMTree* _Document);
                ~ElementRef();
                mutable std::string_view    m_Name        {std::string_view()}; ///< the view to DOM tree node name
                mutable std::string_view    m_Value       {std::string_view()}; ///< the view to DOM tree node value
                mutable ElementRef*         m_Parent      {nullptr};            ///< ref to this DOM tree node parent
                const   DOMTree*            m_Document    {nullptr};            ///< ref to this DOM tree document
                mutable ElementRef*         m_FirstChild  {nullptr};            ///< ref to the first child of this DOM tree node
                mutable ElementRef*         m_LastChild   {nullptr};            ///< ref to the last child of this DOM tree node
                mutable ElementRef*         m_NextSibling {nullptr};            ///< ref to the next sibling DOM tree node
                mutable ElementRef*         m_PrevSibling {nullptr};            ///< ref to the previous sibling DOM tree node
                mutable int                 m_Attributes  {0};                  ///< this DOM tree node attributes
            };

            /**
             * @brief This class defines a DOM tree node object
             * @class ElementObj
             */
            class ElementObj final
            {
            public:

                /**
                 * @brief Constructs a new Element Obj object
                 * @param _Ref reference to this DOM tree object
                 */
                ElementObj(ElementRef* _Ref = nullptr);
                ~ElementObj();

                /**
                 * @brief returns this DOM tree object attributes
                 * @returns an integer which encodes this DOM tree object attributes
                 */
                int get_attributes() const;

                /**
                 * @brief returns a pointer to reference of this DOM tree object
                 * @returns a pointer to reference of this DOM tree object
                 */
                ElementRef* get_ref() const;
                
                /**
                 * @brief returns a pointer to the document to which this DOM tree object belongs to
                 * @returns a pointer to the document to which this DOM tree object belongs to
                 */
                const DOMTree* get_document() const;
                
                /**
                 * @brief returns a view to the name of this DOM tree object
                 * @returns a view to the name of this DOM tree object
                 */
                std::string_view get_name() const;

                /**
                 * @brief returns a view to the value of this DOM tree object
                 * @returns a view to the value of this DOM tree object
                 */
                std::string_view get_value() const;

                /**
                 * @brief returns next sibling DOM tree node object
                 * @returns next sibling DOM tree node object
                 */
                ElementObj get_next() const;

                /**
                 * @brief returns previous sibling DOM tree node object
                 * @returns previous sibling DOM tree node object
                 */
                ElementObj get_prev() const;

                /**
                 * @brief returns first child DOM tree node object
                 * @returns first child DOM tree node object
                 */
                ElementObj get_first() const;

                /**
                 * @brief returns last child DOM tree node object
                 * @returns last child DOM tree node object
                 */
                ElementObj get_last() const;

                /**
                 * @brief returns parent DOM tree node object
                 * @returns parent DOM tree node object
                 */
                ElementObj get_parent() const;

                /**
                 * @brief returns an iterator to the first child DOM tree object
                 * @returns an iterator to the first child DOM tree object
                 */
                const ElementItr begin() const;

                /**
                 * @brief returns an iterator to the end of this DOM tree object children sequence
                 * @returns an iterator to the end of this DOM tree object children sequence
                 */
                const ElementItr end() const;

                // setters

                /**
                 * @brief sets the name of this DOM tree object
                 * @param _Name wanted name value
                 * @details this function allocates space for the new name of this object and copies input _Value into the pool
                 * normalizing it, i.e it removes all not-allowed symbols
                 */
                void set_name(const std::string& _Name);

                /**
                 * @brief sets the value of this DOM tree object
                 * @param _Value wanted name value
                 * @details this function allocates space for the new value of this object and copies input _Value into the pool
                 * normalizing it, i.e it replaces all not-allowed symbols by an appropriate sequence
                 */
                void set_value(const std::string& _Value);

                /**
                 * @brief sets attributes of this DOM tree object
                 * @param _Attributes input attributes
                 */
                void set_attributes(const int& _Attributes);

                /**
                 * @brief checks if the refence contained by this object is nullptr or that a document is nullptr
                 * @returns true if the refence contained by this object is nullptr or that a document is nullptr
                 */
                bool is_null() const;

                /**
                 * @brief checks if the refence contained by this object is not nullptr and that a document is not nullptr
                 * @returns true if the refence contained by this object is not nullptr and that a document is not nullptr
                 */
                bool is_not_null() const;

                /**
                 * @brief Looks for the child node that satisfies an input predicate
                 * @param _Predicate input predicate across which we check the child nodes
                 * @return the child node that satisfies an input predicate 
                 */
                template<typename Predicate>
                ElementObj find_node(const Predicate& _Predicate) const
                {
                    ElementObj next = get_first();

                    while (next.is_not_null())
                    {
                        if(_Predicate(next))
                            return next;

                        next = next.get_next();
                    }

                    return ElementObj();
                }

                /**
                 * @brief Traverses in-depth this tree DOM tree node children
                 * @param _StartCallback this callback is called when yet another node is found
                 * @param _EndCallback this callback is called when yet another node depth-first traversal has finished
                 */
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

                /**
                 * @brief Appends the new node to this node children sequence
                 * @param _Name new node name
                 * @param _Value new node value
                 * @param _Attributes new node attributes
                 * @returns the new node
                 */
                ElementObj append_node(const std::string& _Name  = std::string(), const std::string& _Value = std::string(), const int& _Attributes = ElementAttributes_::ElementAttributes_Defaults);

                /**
                 * @brief Appends the new node after this node within this node parent node children sequence
                 * @param _Name new node name
                 * @param _Value new node value
                 * @param _Attributes new node attributes
                 * @returns the new node
                 */
                ElementObj append_node_after(const std::string& _Name = std::string(), const std::string& _Value = std::string(), const int& _Attributes = ElementAttributes_::ElementAttributes_Defaults);

                /**
                 * @brief Prepends the new node to this node children sequence
                 * @param _Name new node name
                 * @param _Value new node value
                 * @param _Attributes new node attributes
                 * @returns the new node
                 */
                ElementObj prepend_node(const std::string& _Name = std::string(), const std::string& _Value = std::string(), const int& _Attributes = ElementAttributes_::ElementAttributes_Defaults);
                
                /**
                 * @brief Prepends the new node before this node within this node parent children sequence
                 * @param _Name new node name
                 * @param _Value new node value
                 * @param _Attributes new node attributes
                 * @returns the new node
                 */
                ElementObj prepend_node_before(const std::string& _Name = std::string(), const std::string& _Value = std::string(), const int& _Attributes = ElementAttributes_::ElementAttributes_Defaults);

                /**
                 * @brief Removes this node
                 */
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

            /**
             * @brief This class defines a DOM tree object
             * @class DOMTree
             * @details This is the model of the DOM tree that uses monotonic buffer resources for it's node and their names, and values.
             * All the nodes MUST BE allocated using an interface of this DOM tree model. To prevent this model from memory blow-up it's
             * recommended for scoped usage.
             */
            class DOMTree final
            {
            public:
                DOMTree();
                DOMTree(const DOMTree&) = delete;
                DOMTree& operator=(const DOMTree&) = delete;

                /**
                 * @brief Returns the root of this DOM tree
                 * @returns the root node object of this DOM tree 
                 */
                ElementObj get_root() const;

                /**
                 * @brief Releases the whole DOM tree
                 */
                void release();

                /**
                 * @brief Creates a node object
                 * @param _Name new node name
                 * @param _Value new node value
                 * @param _Attributes new node attributes
                 * @return new node
                 */
                ElementObj create_node(
                    const std::string_view& _Name       = std::string_view(),
                    const std::string_view& _Value      = std::string_view(),
                    const int&              _Attributes = ElementAttributes_::ElementAttributes_Defaults) const;

                bool append_node(const ElementObj& _Node, const ElementObj& _Parent) const;
                bool append_node_after(const ElementObj& _Node, const ElementObj& _Parent) const;
                bool prepend_node(const ElementObj& _Node, const ElementObj& _Parent) const;
                bool prepend_node_before(const ElementObj& _Node, const ElementObj& _Parent) const;

                /**
                 * @brief This function parses input string
                 * @tparam Parser parser that is used to parse input string
                 * @param _Begin the start of the input string
                 * @param _End the end of the input string
                 * @param _TargetObj the object in which children sequence the newly parsed sub-tree appended,
                 * if it's null then the whole document is released and the new document is createad
                 * @returns true if parsing succeeds 
                 */
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

                /**
                 * @brief Writes this DOM tree into the string
                 * @tparam Writer writer used to write a string
                 * @param _TargetObj if it's not null then only this object subtree is written into a string, otherwise the whole tree is written
                 * @returns a string containing written tree 
                 */
                template<typename Writer>
                std::string write_string(const ElementObj& _TargetObj = ElementObj(nullptr))
                {
                    return Writer::write_string(_TargetObj.is_not_null() ? _TargetObj : get_root());
                }

                /**
                 * @brief This function parses a file
                 * @tparam Parser parser that is used to parse input file
                 * @param _Path path of the input file
                 * @param _TargetObj the object in which children sequence the newly parsed sub-tree appended,
                 * if it's null then the whole document is released and the new document is createad
                 * @returns true if parsing succeeds 
                 */
                template<typename Parser>
                bool read_file(const std::string& _Path, const ElementObj& _TargetObj = ElementObj(nullptr))
                {
                    // open file
                    std::FILE* file = std::fopen(_Path.c_str(), "rb");
                    
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

                /**
                 * @brief Saves DOM tree to a file
                 * @tparam Writer writer used to write DOM tree to a file
                 * @param _Path path to the file, where we write out tree
                 * @param _TargetObj the object which children sequence is written to a file,
                 * if it's null then the whole document written
                 * @return true if write succeeds
                 */
                template<typename Writer>
                bool save_file(const std::string& _Path, const ElementObj& _TargetObj = ElementObj(nullptr))
                {
                    return Writer::save_file(_TargetObj.is_not_null() && _TargetObj.get_document() == this ? _TargetObj : get_root(), _Path);
                }

            private:

                // friends
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

                // service methods
                std::string_view copy_string(const std::string& _Value = std::string()) const;
            };

            // FileStreamer
            template<size_t _Size = 65536>
            class FileStreamer final
            {
            public:

                FileStreamer(const std::string& _Path) : m_StreamFile(fopen(_Path.c_str(), "wb")){}

                bool begin()
                {                    
                    if(m_StreamFile == nullptr)
                        return false;
                    
                    setvbuf(m_StreamFile, m_StreamBuffer, _IOFBF, _Size);
                    return true;
                }

                void write(const char _Input[], const int _Length)
                {
                    if(m_StreamFile == nullptr || _Input == nullptr || _Length <= 0)
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

            // StringStreamer
            template<size_t _Size = 65536>
            class StringStreamer final
            {
            public:

                StringStreamer(){}

                // getters
                const std::string& get_stream_string() const
                {
                    return m_StreamString;
                }

                // API
                bool begin()
                {
                    m_StreamString.clear();
                    m_StringOffset = 0;
                    return true;
                }

                void write(const char _Input[], const int _Length)
                {
                    if(_Input == nullptr || _Length <= 0)
                        return;

                    int written = 0;

                    while (written < _Length)
                    {
                        int length = std::min<int>(_Length - written, _Size - m_StringOffset);
                        memcpy(m_StringBuffer + m_StringOffset, _Input + written, length);
                        m_StringOffset += length;
                        written  += length;

                        if(m_StringOffset < _Size) continue;

                        m_StreamString.append(std::string_view(m_StringBuffer, m_StringOffset));
                        m_StringOffset = 0;
                    }
                }

                void end()
                {
                    if(m_StringOffset <= 0)
                        return;
                    
                    m_StreamString.append(std::string_view(m_StringBuffer, m_StringOffset));
                    m_StringOffset = 0;
                }

            protected:

                // info
                char        m_StringBuffer[_Size]{};
                char        m_StreamBuffer[_Size]{};
                std::string m_StreamString{std::string()};
                int         m_StringOffset{0};
            };
        
             /*! @} */
        }
    }
}