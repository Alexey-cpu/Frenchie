#include <FrenchieCoreTextPieceTable.hpp>

#include <FrenchieCoreHelpers.hpp>

#include <vector>
#include <iostream>
#include <map>
#include <set>

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
            &row.Buffer->at(row.Start),
            &row.Buffer->at(row.Start + row.Length)));
    }

    return text;
}

int binarySearch(std::vector<int> &arr, int x) 
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

int PieceTable::get_line_start_index(const int& _Line) const
{
    // look for the row where to append
    int  cursorLine     = 0;
    int  cursorPosition = 0;
    auto cursorIterator = m_Pieces.begin();

    for(auto iterator = m_Pieces.begin(); iterator != m_Pieces.end(); iterator++)
    {
        int firstLine = binarySearch(iterator->Buffer->m_LineBreaksPositions, iterator->Start);
        int lastLine  = binarySearch(iterator->Buffer->m_LineBreaksPositions, iterator->Start + iterator->Length);

        for (int i = firstLine; i < lastLine; i++, cursorLine++)
        {
            auto pos = iterator->Buffer->m_LineBreaksPositions[i];
            
            if(pos >= iterator->Start && pos <= iterator->Start + iterator->Length)
            {
                cursorPosition += pos - iterator->Start;
            }
            else
            {
                cursorPosition += iterator->Length;
            }

            if(cursorLine == _Line)
            {
                return cursorPosition;
            }
        }
    }
    
    return cursorPosition;
}

int PieceTable::get_lines_count() const
{
    // look for the row where to append
    int cursorLine = 0;

    for(auto iterator = m_Pieces.begin(); iterator != m_Pieces.end(); iterator++)
    {
        int firstLine = binarySearch(iterator->Buffer->m_LineBreaksPositions, iterator->Start);
        int lastLine  = binarySearch(iterator->Buffer->m_LineBreaksPositions, iterator->Start + iterator->Length);
        cursorLine += lastLine - firstLine;
    }
    
    return ++cursorLine;
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

            // insert in the very begining
            if(cursorIterator == m_Pieces.end())
            {
                m_Pieces.push_back(Piece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));
                m_Appendable.append(_What);
                return;
            }

            // insert in the very end
            if(cursorIterator->Buffer == &m_Appendable && _Position > 0 && (cursorPosition - _Position) == 0)
            {
                cursorIterator->Length++;
                m_Appendable.append(_What);
                return;
            }

            // append new data
            auto newPiece = m_Pieces.insert(
                (_Position <= 0 ? cursorIterator : std::next(cursorIterator)),
                Piece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));

            m_Appendable.append(_What);

            // slice existing data
            int newLength = cursorPosition - _Position;

            if(newLength > 0)
            {
                cursorIterator->Length -= newLength;

                m_Pieces.insert(
                    std::next(newPiece),
                    Piece(cursorIterator->Buffer, cursorIterator->Start + cursorIterator->Length, newLength));
            }
        }
    );
}

void PieceTable::erase(const int& _Position, const int& _Count)
{
    auto remove = [this](const int& _Position)
    {
        if(_Position < 0)
            return;

        // look for the point where to append
        int  cursorPosition = 0;
        auto cursorIterator = m_Pieces.begin();

        for(auto iterator = m_Pieces.begin();
            iterator != m_Pieces.end() && cursorPosition < _Position;
            cursorPosition += iterator->Length, iterator++) 
            cursorIterator = iterator;

        if(cursorIterator == m_Pieces.end())
            return;

        //---------------------------------------------------------------------------------------------------------
        // append new data
        //---------------------------------------------------------------------------------------------------------
        //---------------------------------------------------------------------------------------------------------

        int newLength = cursorPosition - _Position;

        // remove the last symbol in line
        if(newLength <= 0)
        {
            cursorIterator->Length--;

            if(cursorIterator->Length <= 0)
                m_Pieces.erase(cursorIterator);

            return;
        }

        // remove symbols in between
        cursorIterator->Length -= newLength;

        if(cursorIterator->Length <= 0) 
        {
            m_Pieces.erase(cursorIterator);
            return;
        }

        // slice existing data
        if(newLength - 1 > 0)
        {
            m_Pieces.insert(
                std::next(cursorIterator),
                Piece(cursorIterator->Buffer, cursorIterator->Start + cursorIterator->Length + 1, newLength - 1));
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