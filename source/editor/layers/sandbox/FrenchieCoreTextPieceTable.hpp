#pragma once

#include <list>
#include <vector>
#include <string>
#include <functional>

namespace Frenchie
{
    namespace Core
    {
        struct PieceTableRow
        {
            struct PieceTableBuffer
            {
                PieceTableBuffer(const std::wstring& _Buffer) : Buffer(_Buffer)
                {
                    for (int i = 0; i < (int)Buffer.size(); i++)
                    {
                        if(Buffer[i] == '\n')
                            Lines.push_back(i);
                    }   
                }

                void push(const std::wstring& _What)
                {
                    Buffer.append(_What);

                    for (int i = (int)(Buffer.size() - _What.size()); i < (int)Buffer.size(); i++)
                    {
                        if(Buffer[i] == '\n')
                            Lines.push_back(i);
                    }
                }

                std::wstring     Buffer;
                std::vector<int> Lines;
            };

            PieceTableRow(const std::wstring* _Buffer, int _Start, int _Length) : 
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

            PieceTable(const std::wstring& _Buffer = std::wstring());
            ~PieceTable();

            // getters
            std::wstring get_text() const;

            // API
            void insert(const int& _Position, const std::wstring& _What);
            void erase(const int& _Position, const int& _Count = 1);
            void undo();
            void redo();

            int size() const;

            std::list<PieceTableRow>::const_iterator begin() const
            {
                return m_TableRows.begin();
            }

            std::list<PieceTableRow>::const_iterator end() const
            {
                return m_TableRows.end();
            }

        protected:

            const   std::wstring                          m_Immutable;
            mutable std::wstring                          m_Modifiable;
            mutable std::list<PieceTableRow>              m_TableRows;
            mutable std::vector<std::list<PieceTableRow>> m_TableStates;
            mutable size_t                                m_CurrentState = 0;

            // service methods
            void command(std::function<void()> _Execute);
        };

        class PieceTablePiece
        {
        public:
        protected:
            mutable std::list<PieceTableRow> m_TableRows;
        };

    }
}