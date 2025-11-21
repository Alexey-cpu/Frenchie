#pragma once

#include <map>
#include <list>
#include <vector>
#include <string>
#include <functional>

#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreContainersRingBuffer.hpp>

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
            mutable std::u32string   m_Text;
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

        class TextDocumentPieceTable
        {
        public:

            typedef std::list<TextDocumentPiece>::const_iterator ConstPieceIterator;
            typedef std::list<TextDocumentPiece>::iterator PieceIterator;

            TextDocumentPieceTable(const std::list<TextDocumentPiece>& _Pieces = std::list<TextDocumentPiece>(), const int& _Position = 0) :
                Pieces(_Pieces),
                Cursor(_Position){}

            std::list<TextDocumentPiece> Pieces;
            int                          Cursor = 0;
        };

        class TextDocumentSymbolIterator final
        {
        public:
            TextDocumentSymbolIterator(const TextDocument* _Table = nullptr, const int& _Position = 0);
            ~TextDocumentSymbolIterator();

            int get_position() const
            {
                return m_Position;
            }

            char32_t operator*() const;
            TextDocumentPieceTable::ConstPieceIterator operator->() const;
            
            TextDocumentSymbolIterator& operator++();
            TextDocumentSymbolIterator& operator--();
            TextDocumentSymbolIterator  operator++(int);
            TextDocumentSymbolIterator  operator--(int);

            friend bool operator==(const TextDocumentSymbolIterator& _First, const TextDocumentSymbolIterator& _Second)
            {
                return TextDocumentSymbolIterator::equal(_First, _Second);
            }

            friend bool operator!=(const TextDocumentSymbolIterator& _First, const TextDocumentSymbolIterator& _Second)
            {
                return !TextDocumentSymbolIterator::equal(_First, _Second);
            }

            void increment_by(const int& _Distance);

        protected:

            static bool equal(const TextDocumentSymbolIterator& _First, const TextDocumentSymbolIterator& _Second);

            const TextDocument*                                m_Table    = nullptr;
            mutable int                                        m_Position = 0;
            mutable int                                        m_Offset   = 0;
            mutable TextDocumentPieceTable::ConstPieceIterator m_Iterator = TextDocumentPieceTable::ConstPieceIterator();
        };

        class TextDocument final
        {
        public:

            // nested types
            struct PieceInfo
            {
                TextDocumentPieceTable::ConstPieceIterator Iterator;
                int                                        Offset;
            };

            struct LineInfo
            {
                TextDocumentSymbolIterator Begin;
                TextDocumentSymbolIterator End;
            };

            // construction
            TextDocument(const std::u32string& _Buffer = std::u32string());
            ~TextDocument();

            // getters
            std::u32string get_text(
                const TextDocumentSymbolIterator& _Begin,
                const TextDocumentSymbolIterator& _End,
                const int&                        _MaximumSymbolsToExtract = INT_MAX) const;

            int get_line_start_index(const int&) const;
            int get_line_end_index(const int&) const;
            int get_cursor_position() const;

            // setters
            void set_cursor_position(const int&) const;

            // API
            void insert(const int& _Position, const std::u32string& _What);
            void insert(const std::u32string& _What);

            void erase(const int& _Position, const int& _Count);
            void erase(const int& _Count = 1);

            void undo();
            void redo();

            void move_cursor_right();
            void move_cursor_left();
            void move_cursor_down();
            void move_cursor_up();

            TextDocumentPieceTable::ConstPieceIterator pieces_begin() const;
            TextDocumentPieceTable::ConstPieceIterator pieces_end() const;
            int pieces_count() const;

            TextDocumentSymbolIterator symbol_begin(const int&) const;

            TextDocumentSymbolIterator symbols_begin() const;
            TextDocumentSymbolIterator symbols_end() const;
            int symbols_count() const;

            TextDocumentSymbolIterator line_begin(const int&) const;
            TextDocumentSymbolIterator line_end(const int&) const;
            int lines_count() const;

            bool empty() const;
            static const TextDocument* empty_document();

        protected:

            // friends
            friend class TextDocumentSymbolIterator;

            // static vars
            inline static const TextDocument* m_EmptyDocument = nullptr;

            // info
            const   TextDocumentBuffer      m_Immutable;
            mutable TextDocumentBuffer      m_Appendable;
            mutable TextDocumentPieceTable  m_Pieces;
            mutable std::map<int, LineInfo> m_Cache;

            mutable Frenchie::Core::Containers::RingBuffer<TextDocumentPieceTable, 2048> m_States;

            // service methods
            void command(std::function<void()> _Command, const bool& _SaveState = true);

            PieceInfo get_piece_iterator_by_global_index(const int&) const;
            PieceInfo get_piece_iterator_by_global_index(const int&, TextDocumentPieceTable::ConstPieceIterator&) const;
        };
    }
}