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

int PieceTable::get_line_start_index(const int& _Line) const
{
    return _Line - 1 < 0 ? 0 : get_line_end_index(_Line - 1);
}

int PieceTable::get_line_end_index(const int& _Line) const
{
    // look for the row where to append
    int  cursorLine     = 0;
    int  cursorPosition = 0;
    auto cursorIterator = m_Pieces.begin();

    for(auto iterator = m_Pieces.begin(); iterator != m_Pieces.end(); iterator++)
    {
        if(iterator->LineBreaksCount <= 0)
        {
            cursorPosition += iterator->Length;
        }
        else
        {
            if(_Line - cursorLine >= iterator->LineBreaksCount)
            {
                cursorPosition += iterator->Length;
                cursorLine     += iterator->LineBreaksCount;
                continue;
            }

            int lineBreakIndex = iterator->LineBreaksStart + (_Line - cursorLine);
            cursorPosition += iterator->Buffer->m_LineBreaksPositions[lineBreakIndex] - iterator->Start;
            return cursorPosition;

            // TODO: uncomment this code if the above code does not work =)
            // for (int lineBreakIndex = iterator->LineBreaksStart; lineBreakIndex <= iterator->LineBreaksEnd; lineBreakIndex++)
            // {
            //     if(cursorLine == _Line)
            //     {
            //         cursorPosition += iterator->Buffer->m_LineBreaksPositions[lineBreakIndex] - iterator->Start;

            //         return cursorPosition;
            //     }

            //     cursorLine++;
            // }

            //cursorPosition += iterator->Length;
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
        int firstLine = iterator->Buffer->binarySearch(iterator->Buffer->m_LineBreaksPositions, iterator->Start);
        int lastLine  = iterator->Buffer->binarySearch(iterator->Buffer->m_LineBreaksPositions, iterator->Start + iterator->Length);
        cursorLine += lastLine - firstLine;
    }

    if(!m_Pieces.empty() && std::prev(m_Pieces.end())->LineBreaksCount <= 0)
        ++cursorLine;

    return cursorLine;
}

std::pair<PieceTable::const_iterator, int>
PieceTable::get_iterator_by_global_index(const int _Position) const
{
    // look for the row where to append
    int  cursorPosition = 0;
    auto cursorIterator = m_Pieces.begin();

    for(auto iterator = m_Pieces.begin();
        iterator != m_Pieces.end() && cursorPosition < _Position;
        cursorPosition += iterator->Length, iterator++) 
        cursorIterator = iterator;
    
    if(cursorIterator == m_Pieces.end())
        return {cursorIterator, -1};

    // beggining
    if(_Position <= 0 && (cursorPosition - _Position) == 0)
        return {cursorIterator, 0};

    // end
    if(_Position > 0 && (cursorPosition - _Position) == 0)
        return {cursorIterator, cursorIterator->Length};

    // middle
    return {cursorIterator, cursorIterator->Length - (cursorPosition - _Position)};
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

            // insert into empty table
            if(cursorIterator == m_Pieces.end())
            {
                m_Pieces.push_back(Piece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));
                m_Appendable.append(_What);
                return;
            }

            // insert into beginning
            if(_Position <= 0 && (cursorPosition - _Position) == 0)
            {
                m_Pieces.insert(cursorIterator, Piece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));
                m_Appendable.append(_What);
                return;
            }

            // insert into end
            if(_Position > 0 && (cursorPosition - _Position) == 0)
            {
                m_Pieces.insert(std::next(cursorIterator), Piece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));
                m_Appendable.append(_What);
                return;
            }

            // insert into a middle
            int a = cursorPosition - _Position;
            int b = cursorIterator->Length - a;

            auto newPiece = m_Pieces.insert(std::next(cursorIterator), Piece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));
            m_Appendable.append(_What);

            cursorIterator->Length = b;

            m_Pieces.insert(std::next(newPiece), Piece(cursorIterator->Buffer, cursorIterator->Start + b, a));
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

        // remove from empty table
        if(cursorIterator == m_Pieces.end())
            return;

        // remove from beginning
        if(_Position <= 0 && (cursorPosition - _Position) == 0)
        {
            if(cursorIterator != m_Pieces.begin())
            {
                cursorIterator--;
                cursorIterator->Length--;
            }

            return;
        }

        // remove from beginning plus one
        if(_Position > 0 && (cursorIterator->Length - (cursorPosition - _Position)) == 1)
        {
            cursorIterator->Start++;
            cursorIterator->Length--;
            return;
        }

        // remove from end
        if(_Position > 0 && (cursorPosition - _Position) == 0)
        {
            cursorIterator->Length--;
            return;
        }

        // remove from middle
        int a = cursorPosition - _Position;
        int b = cursorIterator->Length - a;

        cursorIterator->Length = b - 1;

        m_Pieces.insert(std::next(cursorIterator), Piece(cursorIterator->Buffer, cursorIterator->Start + b, a));
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

    // update state
    m_States.push_back(m_Pieces);
    m_CurrentState = m_States.size() - 1;

    // recompute metadata of pieces
    for(auto iterator = m_Pieces.begin(); iterator != m_Pieces.end(); iterator++)
    {
        iterator->LineBreaksStart = INT_MAX;
        iterator->LineBreaksEnd   = INT_MIN;
        iterator->LineBreaksCount = 0;

        int firstLine = iterator->Buffer->binarySearch(iterator->Buffer->m_LineBreaksPositions, iterator->Start);
        int lastLine  = iterator->Buffer->binarySearch(iterator->Buffer->m_LineBreaksPositions, iterator->Start + iterator->Length);

        for(int i = firstLine; i < lastLine; i++)
        {
            int pos = iterator->Buffer->m_LineBreaksPositions[i];

            if(pos >= iterator->Start && pos <= iterator->Start + iterator->Length)
            {
                iterator->LineBreaksStart = std::min<int>(iterator->LineBreaksStart, i);
                iterator->LineBreaksEnd   = std::max<int>(iterator->LineBreaksEnd, i);
                iterator->LineBreaksCount++;
            }
        }
    }
}