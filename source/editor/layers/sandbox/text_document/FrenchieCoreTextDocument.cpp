#include <FrenchieCoreTextDocument.hpp>

// STL
#include <iostream>
#include <vector>
#include <map>
#include <set>

using namespace Frenchie::Core;

// TextDocumentBuffer
TextDocumentBuffer::TextDocumentBuffer(const std::u32string& _Buffer)
{
    append(_Buffer);
}

TextDocumentBuffer::~TextDocumentBuffer(){}

char32_t& TextDocumentBuffer::at(const int& _Position) const
{
    return m_Text[_Position];
}

int TextDocumentBuffer::size() const
{
    return (int)m_Text.size();
}

void TextDocumentBuffer::append(const std::u32string& _Text)
{
    m_Text.append(_Text);

    for(int i = (int)(m_Text.size() - _Text.size()); i < (int)m_Text.size(); i++)
    {
        if(m_Text[i] == '\n')
            m_LineBreaksPositions.push_back(i);
    }
}

const std::vector<int>& TextDocumentBuffer::get_line_breaks_positions() const
{
    return m_LineBreaksPositions;
}

// TextDocumentIterator
TextDocumentSymbolIterator::TextDocumentSymbolIterator(const TextDocument* _Table, const int& _Position) :
    m_Table(_Table == nullptr ? TextDocument::empty_document() : _Table), // we guarantee this is never nullptr
    m_Position(_Position)
{
    // retrieve iterator and it's offset
    TextDocument::PieceInfo it = m_Table->get_piece_iterator_by_global_index(m_Position);
    m_Iterator = it.Iterator;
    m_Offset   = it.Offset;
}

TextDocumentSymbolIterator::~TextDocumentSymbolIterator(){}

char32_t TextDocumentSymbolIterator::operator*() const
{
    return m_Iterator->Buffer->at(m_Iterator->Start + m_Offset);
}

std::list<TextDocumentPiece>::const_iterator TextDocumentSymbolIterator::operator->() const
{
    return m_Iterator;
}

TextDocumentSymbolIterator& TextDocumentSymbolIterator::operator++()
{
    ++m_Position;

    if(++m_Offset >= m_Iterator->Length)
    {
        ++m_Iterator;
        m_Offset = 0;
    }

    // TODO: this is naive implementation...
    // auto it = m_Table->get_piece_iterator_by_global_index(++m_Position);
    // m_Iterator = it.Iterator;
    // m_Offset   = it.Offset;

    return *this;
}

TextDocumentSymbolIterator& TextDocumentSymbolIterator::operator--()
{
    --m_Position;

    if(--m_Offset < 0)
    {
        --m_Iterator;
        m_Offset = m_Iterator->Length - 1;
    }

    // TODO: this is naive implementation...
    // auto it = m_Table->get_piece_iterator_by_global_index(--m_Position);
    // m_Iterator = it.Iterator;
    // m_Offset   = it.Offset;

    return *this;
}

TextDocumentSymbolIterator TextDocumentSymbolIterator::operator++(int)
{
    auto tmp = *this; 
    ++(*this); 
    return tmp;
}

TextDocumentSymbolIterator TextDocumentSymbolIterator::operator--(int)
{
    auto tmp = *this; 
    --(*this); 
    return tmp;
}

bool TextDocumentSymbolIterator::equal(const TextDocumentSymbolIterator& _First, const TextDocumentSymbolIterator& _Second)
{
    if(_First.m_Table->empty() && _Second.m_Table->empty())
        return true;

    return _First.m_Position == _Second.m_Position;
}

// TextDocument
TextDocument::TextDocument(const std::u32string& _Buffer) :
    m_Immutable(_Buffer),
    m_Appendable(std::u32string()),
    m_Pieces(TextDocumentPieceTable({TextDocumentPiece(&m_Immutable, 0, (int)m_Immutable.size())}, 0))
    {
        command(nullptr);
    }

TextDocument::~TextDocument(){}

std::u32string TextDocument::get_text(const TextDocumentSymbolIterator& _Begin, const TextDocumentSymbolIterator& _End) const
{
    std::u32string text;
    for(auto cursorIterator = _Begin; cursorIterator != _End; cursorIterator++)
        text += *cursorIterator;
    return text;
}

int TextDocument::get_line_start_index(const int& _Line) const
{
    return _Line - 1 < 0 ? 0 : get_line_end_index(_Line - 1);
}

int TextDocument::get_cursor_position() const
{
    return m_Pieces.Cursor;
}

int TextDocument::get_line_end_index(const int& _Line) const
{
    // look for the row where to append
    int  cursorLine     = 0;
    int  cursorPosition = 0;
    auto cursorIterator = m_Pieces.Pieces.begin();

    for(auto iterator = m_Pieces.Pieces.begin(); iterator != m_Pieces.Pieces.end(); iterator++)
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
            cursorPosition += iterator->Buffer->get_line_breaks_positions()[lineBreakIndex] - iterator->Start;
            return cursorPosition;
        }
    }
    
    return cursorPosition;
}

TextDocument::PieceInfo TextDocument::get_piece_iterator_by_global_index(const int& _Position) const
{
    // look for the row where to append
    int  cursorPosition = 0;
    auto cursorIterator = m_Pieces.Pieces.begin();

    for(auto iterator = m_Pieces.Pieces.begin();
        iterator != m_Pieces.Pieces.end() && cursorPosition < _Position;
        cursorPosition += iterator->Length, iterator++) 
        cursorIterator = iterator;
    
    // empty table
    if(cursorIterator == m_Pieces.Pieces.end())
        return {m_Pieces.Pieces.end(), 0};

    // beggining
    if(_Position <= 0 && (cursorPosition - _Position) == 0)
    {
        if(cursorIterator != m_Pieces.Pieces.begin()) 
        {
            --cursorIterator;
            return {cursorIterator, cursorIterator->Length - 1};
        }

        return {cursorIterator, 0};
    }

    // end
    if(_Position > 0 && (cursorPosition - _Position) == 0)
        return {++cursorIterator, 0};

    // middle
    int offset = cursorIterator->Length - (cursorPosition - _Position);

    return {cursorIterator, offset};
}

void TextDocument::set_cursor_position(const int& _Position) const
{
    m_Pieces.Cursor = _Position;

    // adjust cursor position
    if(m_Pieces.Cursor >= symbols_count())
    {
        m_Pieces.Cursor = symbols_count();
    }
    else if(m_Pieces.Cursor < 0)
    {
        m_Pieces.Cursor = 0;
    }
}

void TextDocument::insert(const std::u32string& _What)
{
    // insert at cursor position
    insert(get_cursor_position(), _What);

    // move cursor on the right
    for(int i = 0; i < (int)_What.size(); i++)
        move_cursor_right();   
}

void TextDocument::erase(const int& _Count)
{
    // erase at cursor position
    erase(get_cursor_position(), _Count);

    // move cursor on the left
    for(int i = 0; i < _Count; i++)
        move_cursor_left();  
}

void TextDocument::insert(const int& _Position, const std::u32string& _What)
{
    command([this, &_Position, &_What]()
        {
            // look for the row where to append
            int  cursorPosition = 0;
            auto cursorIterator = m_Pieces.Pieces.begin();

            for(auto iterator = m_Pieces.Pieces.begin();
                iterator != m_Pieces.Pieces.end() && cursorPosition < _Position;
                cursorPosition += iterator->Length, iterator++) 
                cursorIterator = iterator;

            // insert into empty table
            if(cursorIterator == m_Pieces.Pieces.end())
            {
                m_Pieces.Pieces.push_back(TextDocumentPiece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));
                m_Appendable.append(_What);
                return;
            }

            // insert into beginning
            if(_Position <= 0 && (cursorPosition - _Position) == 0)
            {
                m_Pieces.Pieces.insert(cursorIterator, TextDocumentPiece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));
                m_Appendable.append(_What);
                return;
            }

            // insert into end
            if(_Position > 0 && (cursorPosition - _Position) == 0)
            {
                m_Pieces.Pieces.insert(std::next(cursorIterator), TextDocumentPiece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));
                m_Appendable.append(_What);
                return;
            }

            // insert into a middle
            int a = cursorPosition - _Position;
            int b = cursorIterator->Length - a;

            auto newPiece = m_Pieces.Pieces.insert(std::next(cursorIterator), TextDocumentPiece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));
            m_Appendable.append(_What);

            cursorIterator->Length = b;

            m_Pieces.Pieces.insert(std::next(newPiece), TextDocumentPiece(cursorIterator->Buffer, cursorIterator->Start + b, a));
        }
    );
}

void TextDocument::erase(const int& _Position, const int& _Count)
{
    auto remove = [this](const int& _Position)
    {
        if(_Position < 0)
            return;

        // look for the point where to append
        int  cursorPosition = 0;
        auto cursorIterator = m_Pieces.Pieces.begin();

        for(auto iterator = m_Pieces.Pieces.begin();
            iterator != m_Pieces.Pieces.end() && cursorPosition < _Position;
            cursorPosition += iterator->Length, iterator++) 
            cursorIterator = iterator;

        // remove from empty table
        if(cursorIterator == m_Pieces.Pieces.end())
            return;

        // remove from beginning
        if(_Position <= 0 && (cursorPosition - _Position) == 0)
        {
            if(cursorIterator != m_Pieces.Pieces.begin())
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

            // remove an empty piece
            if(cursorIterator->Length <= 0)
                m_Pieces.Pieces.erase(cursorIterator);

            return;
        }

        // remove from end
        if(_Position > 0 && (cursorPosition - _Position) == 0)
        {
            cursorIterator->Length--;

            // remove an empty piece
            if(cursorIterator->Length <= 0)
                m_Pieces.Pieces.erase(cursorIterator);

            return;
        }

        // remove from middle
        int a = cursorPosition - _Position;
        int b = cursorIterator->Length - a;

        cursorIterator->Length = b - 1;

        m_Pieces.Pieces.insert(std::next(cursorIterator), TextDocumentPiece(cursorIterator->Buffer, cursorIterator->Start + b, a));
    };

    command([this, &remove, &_Position, &_Count]()
        {
            for(int i = 0; i < _Count; i++)
                remove(_Position - i);
        }
    );
}

void TextDocument::undo()
{
    command([this]()
        {
            m_States.set_position(m_States.get_position() - 1);
            m_Pieces = m_States.at(1);
        },
        false // do not save state as we are rolling back
    );
}

void TextDocument::redo()
{
    command([this]()
        {
            m_States.set_position(m_States.get_position() + 1); 
            m_Pieces = m_States.at(1);
        },
        false // do not save state
    );
}

void TextDocument::move_cursor_right()
{
    set_cursor_position(get_cursor_position() + 1);
}

void TextDocument::move_cursor_left()
{
    set_cursor_position(get_cursor_position() - 1);
}

void TextDocument::move_cursor_down()
{
    int position = get_cursor_position();

    for(auto it = TextDocumentSymbolIterator(this, position); it != symbols_end(); it++, position++)
    {
        if(*it == '\n')
            break;
    }

    set_cursor_position(++position);
}

void TextDocument::move_cursor_up()
{
    auto cursorPosition    = get_cursor_position();
    auto cursorIterator    = TextDocumentSymbolIterator(this, cursorPosition);

    for(auto it = cursorIterator; it != symbols_begin(); it--, cursorPosition--)
    {
        if(it != symbols_end() && it != cursorIterator && *it == '\n')
            break;
    }

    set_cursor_position(cursorPosition);
}

TextDocumentPieceTable::ConstPieceIterator TextDocument::pieces_begin() const
{
    return m_Pieces.Pieces.begin();
}

TextDocumentPieceTable::ConstPieceIterator TextDocument::pieces_end() const
{
    return m_Pieces.Pieces.end();
}

int TextDocument::pieces_count() const
{
    return (int)m_Pieces.Pieces.size();
}

TextDocumentSymbolIterator TextDocument::symbols_begin() const
{
    return TextDocumentSymbolIterator(this, 0);
}

TextDocumentSymbolIterator TextDocument::symbols_end() const
{
    return TextDocumentSymbolIterator(this, symbols_count());
}

int TextDocument::symbols_count() const
{
    int size = 0;

    for(auto iterator = m_Pieces.Pieces.begin(); iterator != m_Pieces.Pieces.end(); iterator++)
        size += iterator->Length;

    return size;
}

TextDocumentSymbolIterator TextDocument::line_begin(const int& _Line) const
{
    if(m_Cache.find(_Line) == m_Cache.end())
    {
        m_Cache[_Line].Begin = TextDocumentSymbolIterator(this, get_line_start_index(_Line));
        m_Cache[_Line].End   = TextDocumentSymbolIterator(this, get_line_end_index(_Line));
    }

    return m_Cache[_Line].Begin;
}

TextDocumentSymbolIterator TextDocument::line_end(const int& _Line) const
{
    if(m_Cache.find(_Line) == m_Cache.end())
    {
        m_Cache[_Line].Begin = TextDocumentSymbolIterator(this, get_line_start_index(_Line));
        m_Cache[_Line].End   = TextDocumentSymbolIterator(this, get_line_end_index(_Line));
    }

    return m_Cache[_Line].End;
}

int TextDocument::lines_count() const
{
    int cursorLine = 0;

    for(auto iterator = m_Pieces.Pieces.begin(); iterator != m_Pieces.Pieces.end(); iterator++)
        cursorLine += iterator->LineBreaksCount;

    if(!m_Pieces.Pieces.empty() && std::prev(m_Pieces.Pieces.end())->LineBreaksCount <= 0)
        ++cursorLine;

    return cursorLine;
}

bool TextDocument::empty() const
{
    return m_Pieces.Pieces.empty();
}

const TextDocument* TextDocument::empty_document()
{
    if(m_EmptyDocument == nullptr)
        m_EmptyDocument = new TextDocument(std::u32string());

    return m_EmptyDocument;
}

void TextDocument::command(std::function<void()> _Command, const bool& _SaveState)
{
    if(_Command != nullptr)
        _Command();

    // auxiliary lambdas
    auto binary_search = [](const std::vector<int>& _Array, int _Value)->int
    {
        int low = 0;
        int high = (int)_Array.size() - 1;
        
        while (low <= high) 
        {
            int mid = low + (high - low) / 2;

            if (_Array[mid] == _Value)
                return mid;

            if (_Array[mid] < _Value)
                low = mid + 1;
            else
                high = mid - 1;
        }

        return low;
    };

    // save state
    if(_SaveState)
        m_States.push(m_Pieces);

    // clear cache
    m_Cache.clear();

    // process pieces
    for(auto iterator = m_Pieces.Pieces.begin(); iterator != m_Pieces.Pieces.end(); iterator++)
    {
        // remove empty pieces
        if(iterator->Length <= 0)
        {
            auto newIterator = iterator;
            iterator++;
            m_Pieces.Pieces.erase(newIterator);

            if(iterator == m_Pieces.Pieces.end())
                break;

            continue;
        }

        // recompute metadata of pieces
        iterator->LineBreaksStart = binary_search(iterator->Buffer->get_line_breaks_positions(), iterator->Start);
        iterator->LineBreaksEnd   = binary_search(iterator->Buffer->get_line_breaks_positions(), iterator->Start + iterator->Length);
        iterator->LineBreaksCount = iterator->LineBreaksEnd - iterator->LineBreaksStart;
    }
}