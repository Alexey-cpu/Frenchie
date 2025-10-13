#include <FrenchieCoreTextPieceTable.hpp>

#include <iostream>

using namespace Frenchie::Core;

PieceTable::PieceTable(const std::wstring& _Buffer) :
    m_Immutable(_Buffer),
    m_AppendOnly(std::wstring()),
    m_TableRows({PieceTableRow(&m_Immutable, 0, (int)m_Immutable.size())})
    {
        command(nullptr);
    }

PieceTable::~PieceTable(){}

std::wstring PieceTable::get_text() const
{
    std::wstring text;

    for(auto&& row : m_TableRows)
    {
        text.append(std::wstring(
            &row.Buffer->c_str()[row.Start],
            &row.Buffer->c_str()[row.Start + row.Length]));
    }

    return text;
}

void PieceTable::insert(const int& _Position, const std::wstring& _What)
{
    command([this, &_Position, &_What]()
        {
            // look for the row where to append
            int  cursorRowPosition = 0;
            auto cursorRowIterator = m_TableRows.begin();

            for(auto iterator = m_TableRows.begin();
                iterator != m_TableRows.end() && cursorRowPosition < _Position;
                cursorRowPosition += iterator->Length, iterator++) 
                cursorRowIterator = iterator;

            if(cursorRowIterator == m_TableRows.end())
            {
                m_TableRows.push_back(PieceTableRow(&m_AppendOnly, (int)m_AppendOnly.size(), (int)_What.size()));
                m_AppendOnly.append(_What);
                return;
            }

            // append new data
            cursorRowIterator->Length -= (cursorRowPosition - _Position);

            m_TableRows.insert(
                std::next(
                    m_TableRows.insert(
                        (_Position <= 0 ? cursorRowIterator : std::next(cursorRowIterator)), 
                        PieceTableRow(&m_AppendOnly, (int)m_AppendOnly.size(), (int)_What.size()))),
                PieceTableRow(
                    cursorRowIterator->Buffer, 
                    cursorRowIterator->Start + cursorRowIterator->Length,
                    cursorRowPosition - _Position));

            m_AppendOnly.append(_What);
        }
    );
}

void PieceTable::erase(const int& _Position, const int& _Count)
{
    command([this, &_Position, &_Count]()
        {
            if(_Count <= 0)
                return;

            // look for the point where to append
            int  cursorRowPosition = 0;
            auto cursorRowIterator = m_TableRows.begin();

            for(auto iterator = m_TableRows.begin();
                iterator != m_TableRows.end() && cursorRowPosition < _Position;
                cursorRowPosition += iterator->Length, iterator++) 
                cursorRowIterator = iterator;

            // remove symbols
            if(cursorRowIterator->Length > _Count)
            {
                cursorRowIterator->Start  += _Count;
                cursorRowIterator->Length -= _Count;
                return;
            }

            int count = _Count;

            while(count > 0 && !m_TableRows.empty())
            {
                count -= cursorRowIterator->Length;

                if(cursorRowIterator != m_TableRows.begin())
                {
                    auto newIt = std::prev(cursorRowIterator);
                    m_TableRows.erase(cursorRowIterator);
                    cursorRowIterator = newIt;
                }
                else
                {
                    m_TableRows.erase(cursorRowIterator);
                }
            }
        }
    );
}

void PieceTable::undo()
{
    if(m_CurrentState > 0) 
        --m_CurrentState;
        
    m_TableRows = m_TableStates[m_CurrentState];
}

void PieceTable::redo()
{
    m_CurrentState = std::min(m_TableStates.size() - 1, m_CurrentState + 1);
    m_TableRows    = m_TableStates[m_CurrentState];
}

int PieceTable::size() const
{
    int size = 0;

    for(auto iterator = m_TableRows.begin(); iterator != m_TableRows.end(); iterator++) 
        size += iterator->Length;

    return size;
}

void PieceTable::command(std::function<void()> _Execute)
{
    if(_Execute != nullptr)
        _Execute();

    m_TableStates.push_back(m_TableRows);
    m_CurrentState = m_TableStates.size() - 1;
}