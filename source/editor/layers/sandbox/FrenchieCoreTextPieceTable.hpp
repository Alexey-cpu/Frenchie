#pragma once

#include <list>
#include <vector>
#include <string>
#include <functional>

namespace Frenchie
{
    namespace Core
    {
        struct TextBuffer
        {
            TextBuffer(const std::wstring& _Buffer = std::wstring())
            {
                append(_Buffer);
            }

            // API
            wchar_t& at(const int& _Position) const
            {
                return m_Text[_Position];
            }

            int size() const
            {
                return (int)m_Text.size();
            }

            void append(const std::wstring& _Text)
            {
                m_Text.append(_Text);

                for(int i = (int)(m_Text.size() - _Text.size()); i < (int)m_Text.size(); i++)
                {
                    if(m_Text[i] == '\n')
                        m_LineBreaksPositions.push_back(i);
                }
            }

            int binarySearch(std::vector<int> &arr, int x) const
            {
                int low = 0;
                int high = (int)arr.size() - 1;
                
                while (low <= high) 
                {
                    int mid = low + (high - low) / 2;

                    // Check if x is present at mid
                    if (arr[mid] == x)
                        return mid;

                    // If x greater, ignore left half
                    if (arr[mid] < x)
                        low = mid + 1;

                    // If x is smaller, ignore right half
                    else
                        high = mid - 1;
                }

                // If we reach here, then element was not present
                return low;
            }

        //protected:
            mutable std::wstring     m_Text;
            mutable std::vector<int> m_LineBreaksPositions;
        };

        struct Piece
        {
            Piece(const TextBuffer* _Buffer, int _Start, int _Length) : 
                Buffer(_Buffer),
                Start(_Start),
                Length(_Length){}

            const TextBuffer* Buffer          = nullptr;
            int               Start           = 0;
            int               Length          = 0;
            int               LineBreaksStart = 0;
            int               LineBreaksEnd   = 0;
            int               LineBreaksCount = 0;
        };

        class PieceTable
        {
        public:

            typedef std::list<Piece>::const_iterator const_iterator;
            typedef std::list<Piece>::iterator iterator;

            PieceTable(const std::wstring& _Buffer = std::wstring());
            ~PieceTable();

            // getters
            std::wstring get_text() const;

            int get_line_start_index(const int& _Line) const;
            int get_line_end_index(const int& _Line) const;
            int get_lines_count() const;

            std::pair<const_iterator, int>
            get_iterator_by_global_index(const int _GlobalIndex) const;

            // API
            void insert(const int& _Position, const std::wstring& _What);
            void erase(const int& _Position, const int& _Count = 1);
            void undo();
            void redo();

            const_iterator begin() const;
            const_iterator end() const;
            int size() const;

        protected:

            friend class PieceTableSymbolIterator;

            const   TextBuffer                    m_Immutable;
            mutable TextBuffer                    m_Appendable;
            mutable std::list<Piece>              m_Pieces;
            mutable std::vector<std::list<Piece>> m_States;
            mutable size_t                        m_CurrentState = 0;

            // service methods
            void command(std::function<void()> _Execute);
        };

        class PieceTableSymbolIterator final
        {
        public:
            PieceTableSymbolIterator(const PieceTable* _Table, const int& _Position)
            {
                auto it = _Table->get_iterator_by_global_index(_Position);

                m_Iterator = it.first;
                m_Offset   = it.second;
            }

            ~PieceTableSymbolIterator()
            {
            }

            wchar_t operator*() const
            {
                return m_Iterator->Buffer->at(m_Iterator->Start + m_Offset);
            }

            const wchar_t* operator->() const
            {
                return &m_Iterator->Buffer->at(m_Iterator->Start + m_Offset);
            }
            
            PieceTableSymbolIterator& operator++()
            {
                if(m_Offset < m_Iterator->Length)
                {
                    ++m_Offset;
                }
                else
                {
                    m_Iterator++;
                    m_Offset = 0;
                }
                
                return *this; 
            }

            PieceTableSymbolIterator& operator--()
            {
                if(m_Offset > m_Iterator->Start)
                {
                    --m_Offset;
                }
                else
                {
                    m_Iterator--;
                    m_Offset = m_Iterator->Length - 1;
                }

                return *this;
            }

            PieceTableSymbolIterator  operator++(int)
            {
                auto tmp = *this; 
                ++(*this); 
                return tmp;
            }

            PieceTableSymbolIterator  operator--(int)
            {
                auto tmp = *this; 
                --(*this); 
                return tmp;
            }

            friend bool operator==(const PieceTableSymbolIterator& _First, const PieceTableSymbolIterator& _Second)
            { 
                return _First.m_Iterator == _Second.m_Iterator && 
                       _First.m_Offset == _Second.m_Offset; 
            }

            friend bool operator!=(const PieceTableSymbolIterator& _First, const PieceTableSymbolIterator& _Second)
            {
                return _First.m_Iterator != _Second.m_Iterator || 
                       _First.m_Offset != _Second.m_Offset; 
            }

        //protected:

            PieceTable::const_iterator m_Iterator;
            int                        m_Offset;
        };
    }
}