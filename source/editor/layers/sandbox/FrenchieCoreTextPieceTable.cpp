#include <FrenchieCoreTextPieceTable.hpp>

#include <FrenchieCoreHelpers.hpp>

#include <iostream>

using namespace Frenchie::Core;

PieceTable::PieceTable(const std::wstring& _Buffer) :
    m_Immutable(_Buffer),
    m_Appendable(std::wstring()),
    m_Pieces({Piece(&m_Immutable, 0, (int)m_Immutable.size())})
    {
        command(nullptr);
    }

PieceTable::~PieceTable(){}

std::wstring PieceTable::get_text() const
{
    std::wstring text;

    for(auto&& row : m_Pieces)
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
            int  cursorPosition = 0;
            auto cursorIterator = m_Pieces.begin();

            for(auto iterator = m_Pieces.begin();
                iterator != m_Pieces.end() && cursorPosition < _Position;
                cursorPosition += iterator->Length, iterator++) 
                cursorIterator = iterator;

            if(cursorIterator == m_Pieces.end())
            {
                m_Pieces.push_back(Piece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));
                m_Appendable.append(_What);
                return;
            }

            // append new data
            int newLength = cursorPosition - _Position;

            auto newPiece = 
                m_Pieces.insert(
                    (_Position <= 0 ? cursorIterator : std::next(cursorIterator)),
                    Piece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));

            m_Appendable.append(_What);

            if(newLength > 0)
            {
                cursorIterator->Length -= newLength;

                m_Pieces.insert(
                    std::next(newPiece),
                    Piece(
                        cursorIterator->Buffer, 
                        cursorIterator->Start + cursorIterator->Length,
                        newLength));
            }
        }
    );
}

void PieceTable::erase(const int& _Position, const int& _Count)
{
    auto remove = [this](const int& _Position)
    {
        if(_Position < 0) return;

        // look for the point where to append
        int  cursorPosition = 0;
        auto cursorIterator = m_Pieces.begin();

        for(auto iterator = m_Pieces.begin();
            iterator != m_Pieces.end() && cursorPosition < _Position;
            cursorPosition += iterator->Length, iterator++) 
            cursorIterator = iterator;

        if(cursorIterator == m_Pieces.end())
            return;

        // append new data
        int newLength = cursorPosition - _Position;

        if(newLength <= 0)
        {
            cursorIterator->Start++;
            cursorIterator->Length--;

            if(cursorIterator->Length <= 0)
                m_Pieces.erase(cursorIterator);

            return;
        }

        cursorIterator->Length -= newLength;

        if(cursorIterator->Length <= 0) 
        {
            m_Pieces.erase(cursorIterator);
            return;
        }

        if(newLength - 1 > 0)
        {
            m_Pieces.insert(
                std::next(cursorIterator),
                Piece(
                    cursorIterator->Buffer,
                    cursorIterator->Start + cursorIterator->Length + 1,
                    newLength - 1));
        }
    };

    command([this, &remove, &_Position, &_Count]()
        {
            for(int i = 0; i < _Count; i++)
                remove(_Position - i);
        }
    );
}

void PieceTable::undo()
{
    if(m_CurrentState > 0) 
        --m_CurrentState;
        
    m_Pieces = m_States[m_CurrentState];
}

void PieceTable::redo()
{
    m_CurrentState = std::min(m_States.size() - 1, m_CurrentState + 1);
    m_Pieces    = m_States[m_CurrentState];
}

PieceTable::const_iterator PieceTable::begin() const
{
    return m_Pieces.begin();
}

PieceTable::const_iterator PieceTable::end() const
{
    return m_Pieces.end();
}

int PieceTable::size() const
{
    return (int)m_Pieces.size();
}

void PieceTable::command(std::function<void()> _Execute)
{
    if(_Execute != nullptr)
        _Execute();

    m_States.push_back(m_Pieces);
    m_CurrentState = m_States.size() - 1;
}