#pragma once

#include <list>
#include <vector>
#include <string>
#include <functional>

namespace Frenchie
{
    namespace Core
    {
        struct Piece
        {
            Piece(const std::wstring* _Buffer, int _Start, int _Length) : 
                Buffer(_Buffer),
                Start(_Start),
                Length(_Length){}

            const std::wstring* Buffer;
            int                 Start  = 0;
            int                 Length = 0;
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

            // API
            void insert(const int& _Position, const std::wstring& _What);
            void erase(const int& _Position, const int& _Count = 1);
            void undo();
            void redo();

            const_iterator begin() const;
            const_iterator end() const;
            int size() const;

        protected:

            const   std::wstring                          m_Immutable;
            mutable std::wstring                          m_Appendable;
            mutable std::list<Piece>              m_Pieces;
            mutable std::vector<std::list<Piece>> m_States;
            mutable size_t                                m_CurrentState = 0;

            // service methods
            void command(std::function<void()> _Execute);
        };
    }
}