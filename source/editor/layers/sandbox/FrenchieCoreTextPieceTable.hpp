#pragma once

#include <list>
#include <vector>
#include <string>
#include <functional>

#include <FrenchieCoreUTF.hpp>

namespace Frenchie
{
    namespace Core
    {
        class TextDocumentBuffer;
        class TextDocumentPiece;
        class TextDocument;

        class TextDocumentBuffer final
        {
        public:
            TextDocumentBuffer(const std::u32string& _Buffer = std::u32string());
            ~TextDocumentBuffer();

            char32_t& at(const int& _Position) const;
            int size() const;
            void append(const std::u32string& _Text);

            const std::vector<int>& get_line_breaks_positions() const;

        protected:
            mutable std::u32string     m_Text;
            mutable std::vector<int> m_LineBreaksPositions;
        };

        class TextDocumentPiece final
        {
        public:

            TextDocumentPiece(const TextDocumentBuffer* _Buffer, int _Start, int _Length) : 
                Buffer(_Buffer),
                Start(_Start),
                Length(_Length){}

            ~TextDocumentPiece(){}

            const TextDocumentBuffer* Buffer          = nullptr;
            int                       Start           = 0;
            int                       Length          = 0;
            int                       LineBreaksStart = 0;
            int                       LineBreaksEnd   = 0;
            int                       LineBreaksCount = 0;
        };

        class TextDocumentSymbolIterator final
        {
        public:
            TextDocumentSymbolIterator(const TextDocument* _Table, const int& _Position);
            ~TextDocumentSymbolIterator();

            char32_t operator*() const;
            std::list<TextDocumentPiece>::const_iterator operator->() const;
            
            TextDocumentSymbolIterator& operator++();
            TextDocumentSymbolIterator& operator--();
            TextDocumentSymbolIterator  operator++(int);
            TextDocumentSymbolIterator  operator--(int);

            friend bool operator==(const TextDocumentSymbolIterator& _First, const TextDocumentSymbolIterator& _Second)
            { 
                return _First.m_Iterator == _Second.m_Iterator && 
                       _First.m_Offset == _Second.m_Offset; 
            }

            friend bool operator!=(const TextDocumentSymbolIterator& _First, const TextDocumentSymbolIterator& _Second)
            {
                return _First.m_Iterator != _Second.m_Iterator || 
                       _First.m_Offset != _Second.m_Offset; 
            }

        protected:
            const TextDocument*                                  m_Table    = nullptr;
            mutable int                                          m_Position = 0;
            mutable int                                          m_Offset   = 0;
            mutable std::list<TextDocumentPiece>::const_iterator m_Iterator;
        };

        class TextDocument final
        {
        public:

            typedef std::list<TextDocumentPiece>::const_iterator ConstPieceIterator;

            struct PieceIteratorInfo
            {
                ConstPieceIterator Iterator;
                int                Offset;
            };

            TextDocument(const std::u32string& _Buffer = std::u32string());
            ~TextDocument();

            // getters
            int get_line_start_index(const int&) const;
            int get_line_end_index(const int&) const;

            PieceIteratorInfo get_piece_iterator_by_global_index(const int&) const;

            // API
            void insert(const int& _Position, const std::u32string& _What);
            void erase(const int& _Position, const int& _Count = 1);
            void undo();
            void redo();

            ConstPieceIterator pieces_begin() const;
            ConstPieceIterator pieces_end() const;
            int pieces_count() const;

            TextDocumentSymbolIterator symbols_begin() const;
            TextDocumentSymbolIterator symbols_end() const;
            int symbols_count() const;

            TextDocumentSymbolIterator line_begin(const int&) const;
            TextDocumentSymbolIterator line_end(const int&) const;
            int lines_count() const;

        protected:

            // friends
            friend class TextDocumentSymbolIterator;

            // info
            const   TextDocumentBuffer                        m_Immutable;
            mutable TextDocumentBuffer                        m_Appendable;
            mutable std::list<TextDocumentPiece>              m_Pieces;
            mutable std::vector<std::list<TextDocumentPiece>> m_States;
            mutable size_t                                    m_CurrentState = 0;

            // service methods
            void command(std::function<void()>);
        };
    }
}