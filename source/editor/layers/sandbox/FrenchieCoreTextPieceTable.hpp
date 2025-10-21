#pragma once

#include <list>
#include <vector>
#include <string>
#include <functional>

namespace Frenchie
{
    namespace Core
    {
        struct TextDocumentBuffer;
        struct TextDocumentTextPointer;
        class  TextDocumentIterator;
        class  TextDocument;


        struct TextDocumentBuffer final
        {
            TextDocumentBuffer(const std::wstring& _Buffer = std::wstring());
            ~TextDocumentBuffer();

            wchar_t& at(const int& _Position) const;
            int size() const;
            void append(const std::wstring& _Text);

            const std::vector<int>& get_line_breaks_positions() const;

        protected:
            mutable std::wstring     m_Text;
            mutable std::vector<int> m_LineBreaksPositions;
        };

        struct TextDocumentTextPointer final
        {
            TextDocumentTextPointer(const TextDocumentBuffer* _Buffer, int _Start, int _Length) : 
                Buffer(_Buffer),
                Start(_Start),
                Length(_Length){}

            const TextDocumentBuffer* Buffer          = nullptr;
            int                       Start           = 0;
            int                       Length          = 0;
            int                       LineBreaksStart = 0;
            int                       LineBreaksEnd   = 0;
            int                       LineBreaksCount = 0;
        };

        class TextDocumentIterator final
        {
        public:
            TextDocumentIterator(const TextDocument* _Table, const int& _Position);
            ~TextDocumentIterator();

            wchar_t operator*() const;
            const wchar_t* operator->() const;
            
            TextDocumentIterator& operator++();
            TextDocumentIterator& operator--();
            TextDocumentIterator  operator++(int);
            TextDocumentIterator  operator--(int);

            friend bool operator==(const TextDocumentIterator& _First, const TextDocumentIterator& _Second)
            { 
                return _First.m_Iterator == _Second.m_Iterator && 
                       _First.m_Offset == _Second.m_Offset; 
            }

            friend bool operator!=(const TextDocumentIterator& _First, const TextDocumentIterator& _Second)
            {
                return _First.m_Iterator != _Second.m_Iterator || 
                       _First.m_Offset != _Second.m_Offset; 
            }

        //protected:
            std::list<TextDocumentTextPointer>::const_iterator m_Iterator;
            int                                                m_Offset;
        };

        class TextDocument final
        {
        public:

            typedef std::list<TextDocumentTextPointer>::const_iterator const_iterator;
            typedef std::list<TextDocumentTextPointer>::iterator iterator;

            TextDocument(const std::wstring& _Buffer = std::wstring());
            ~TextDocument();

            // getters
            int get_line_start_index(const int& _Line) const;
            int get_line_end_index(const int& _Line) const;
            int get_lines_count() const;

            std::pair<const_iterator, int>
            get_iterator_by_global_index(const int& _Position) const;

            // API
            void insert(const int& _Position, const std::wstring& _What);
            void erase(const int& _Position, const int& _Count = 1);
            void undo();
            void redo();

            TextDocumentIterator b() const
            {
                return TextDocumentIterator(this, 0);
            }

            TextDocumentIterator e() const
            {
                int size = 0;

                for(auto iterator = m_Pieces.begin(); iterator != m_Pieces.end(); iterator++)
                {
                    size += iterator->Length;
                }

                return TextDocumentIterator(this, size);
            }

            const_iterator begin() const;
            const_iterator end() const;
            int size() const;

        protected:

            // friends
            friend class TextDocumentIterator;

            // info
            const   TextDocumentBuffer                              m_Immutable;
            mutable TextDocumentBuffer                              m_Appendable;
            mutable std::list<TextDocumentTextPointer>              m_Pieces;
            mutable std::vector<std::list<TextDocumentTextPointer>> m_States;
            mutable size_t                                          m_CurrentState = 0;

            // service methods
            void command(std::function<void()> _Execute);
        };
    }
}