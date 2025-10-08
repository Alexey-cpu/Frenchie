#include <FrenchieTextEditorTextModel.hpp>

#include <FrenchieCoreHelpers.hpp>

namespace Frenchie
{
    namespace TextEditor
    {
        const char CURSOR = '|';
        const char ENTER  = '\n';
        const char TAB    = '\t';
    }
}

using namespace Frenchie::Core;

using namespace Frenchie::TextEditor;

TextEditorModel::TextEditorModel()
{
    m_Chunks.push_back(std::wstring());
    m_Cursor = {0, 0};
}

TextEditorModel::~TextEditorModel(){}

int TextEditorModel::get_text_lines_count() const
{
    return (int)m_Chunks.size();
}

int TextEditorModel::get_cursros_line() const 
{
    return m_Cursor.Line;
}

int TextEditorModel::get_cursros_column() const 
{
    return m_Cursor.Column;
}

std::wstring TextEditorModel::get_text_line(int _Line) const
{
    if(m_Chunks.empty()) 
        m_Chunks.push_back(std::wstring());

    return _Line < (int)m_Chunks.size() ? m_Chunks[_Line] : m_Chunks[std::max<int>((int)m_Chunks.size() - 1, 0)];
}

std::wstring TextEditorModel::get_selection() const
{
    std::wstring selection;

    for(auto&& range : m_Selection.Ranges)
    {
        int start  = std::min<int>(range.second.Start, (int)m_Chunks[range.first].size() - 1);
        int finish = std::min<int>(range.second.Finish + 1, (int)m_Chunks[range.first].size());

        selection.append(std::wstring(
            &m_Chunks[range.first].c_str()[start], 
            &m_Chunks[range.first].c_str()[finish])).append(L"\n");
    }

    if(!selection.empty())
        selection.pop_back();

    return selection;
}

bool TextEditorModel::is_dirty() const
{
    return m_Dirty;
}

void TextEditorModel::TextEditorModel::set_cursor_line(const int& _Value)
{
    m_Cursor.Line = _Value;
    adjust_cursor_position();
}

void TextEditorModel::set_cursor_column(const int& _Value)
{
    m_Cursor.Column = _Value;
    adjust_cursor_position();
}

void TextEditorModel::set_dirty(const bool& _Value)
{
    m_Dirty = _Value;
}

void TextEditorModel::reset(const std::string& _TextBuffer)
{
    m_Chunks.clear();
    m_Cursor = {0, 0};
    
    append((_TextBuffer.empty() ? " " : _TextBuffer));
}

void TextEditorModel::reset(const std::wstring& _TextBuffer)
{
    m_Chunks.clear();
    m_Cursor = {0, 0};
    
    append((_TextBuffer.empty() ? L" " : _TextBuffer));
}

void TextEditorModel::append(const std::string& _TextBuffer)
{
    for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < _TextBuffer.size(); textBegin = ++textEnd, ++lineNumber)
    {
        while(textEnd < _TextBuffer.size() && _TextBuffer[textEnd] != ENTER) ++textEnd;

        m_Chunks.push_back(Frenchie::Core::String::as_wide(std::string(&_TextBuffer[textBegin], &_TextBuffer[textEnd])));
    }
}

void TextEditorModel::append(const std::wstring& _TextBuffer)
{
    for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < _TextBuffer.size(); textBegin = ++textEnd, ++lineNumber)
    {
        while(textEnd < _TextBuffer.size() && _TextBuffer[textEnd] != ENTER) ++textEnd;

        m_Chunks.push_back(std::wstring(&_TextBuffer[textBegin], &_TextBuffer[textEnd]));
    }
}

void TextEditorModel::insert(const int& _Line, const int& _Position, const std::string& _What)
{
    if(_Line >= (int)m_Chunks.size() || _What.empty()) 
        return;

    //m_Chunks[_Line].insert(_Position, Frenchie::Core::String::as_wide(_What));

    std::wstring _TextBuffer = m_Chunks[_Line].insert(_Position, Frenchie::Core::String::as_wide(_What));

    std::vector<std::wstring> chunks;

    for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < _TextBuffer.size(); textBegin = ++textEnd, ++lineNumber)
    {
        while(textEnd < _TextBuffer.size() && _TextBuffer[textEnd] != ENTER) ++textEnd;

        chunks.push_back(std::wstring(&_TextBuffer[textBegin], &_TextBuffer[textEnd]));
    }

    m_Chunks[_Line] = chunks[0];

    for(int i = 1, j = 0; i < chunks.size(); i++, j++)
        m_Chunks.insert(m_Chunks.begin() + _Line + j, chunks[i]);

    adjust_cursor_position();
}

void TextEditorModel::insert(const int& _Line, const int& _Position, const std::wstring& _What)
{
    if(_Line >= (int)m_Chunks.size() || _What.empty()) 
        return;

    //m_Chunks[_Line].insert(_Position, _What);

    std::wstring _TextBuffer = m_Chunks[_Line].insert(_Position, _What);

    std::vector<std::wstring> chunks;

    for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < _TextBuffer.size(); textBegin = ++textEnd, ++lineNumber)
    {
        while(textEnd < _TextBuffer.size() && _TextBuffer[textEnd] != ENTER) ++textEnd;

        chunks.push_back(std::wstring(&_TextBuffer[textBegin], &_TextBuffer[textEnd]));
    }

    m_Chunks[_Line] = chunks[0];

    for(int i = 1, j = 0; i < chunks.size(); i++, j++)
        m_Chunks.insert(m_Chunks.begin() + _Line + j, chunks[i]);

    adjust_cursor_position();
}

void TextEditorModel::move_cursor_left()
{
    --m_Cursor.Column;
    adjust_cursor_position();
}

void TextEditorModel::move_cursor_right()
{
    m_Cursor.Column++;
    adjust_cursor_position();
}

void TextEditorModel::move_cursor_up()
{
    --m_Cursor.Line;
    adjust_cursor_position();
}

void TextEditorModel::move_cursor_down()
{
    ++m_Cursor.Line;
    adjust_cursor_position();
}

void TextEditorModel::move_next_line()
{
    if(m_Cursor.Line < 0 || m_Cursor.Line > m_Chunks.size()) 
    {
        adjust_cursor_position();
        return;
    }

    if(m_Cursor.Column < m_Chunks[m_Cursor.Line].size())
    {
        std::wstring copy = std::wstring(
            &m_Chunks[m_Cursor.Line][m_Cursor.Column], 
            m_Chunks[m_Cursor.Line].size() - m_Cursor.Column);

        m_Chunks[m_Cursor.Line].erase(m_Cursor.Column, m_Chunks[m_Cursor.Line].size());

        m_Cursor.Line++;
        m_Cursor.Column = 0;
        m_Chunks.insert(m_Chunks.begin() + m_Cursor.Line, copy);
    }
    else
    {
        m_Cursor.Line++;
        m_Cursor.Column = 0;
        m_Chunks.insert(m_Chunks.begin() + m_Cursor.Line, L"");
    }

    adjust_cursor_position();
}

void TextEditorModel::move_back()
{
    // remove element at specific index
    int index = m_Cursor.Column - 1;
    if(index >= 0 && !m_Chunks[m_Cursor.Line].empty()) 
        m_Chunks[m_Cursor.Line].erase(index, 1);
    
    // decrement cursor position in line
    --m_Cursor.Column;

    if(m_Cursor.Column < 0 && m_Cursor.Line > 0)
    {
        std::wstring copy = std::wstring(m_Chunks[m_Cursor.Line].begin(), m_Chunks[m_Cursor.Line].end()); 

        if(m_Cursor.Line >= 0)
            m_Chunks.erase(m_Chunks.begin() + m_Cursor.Line);

        --m_Cursor.Line;

        if(m_Cursor.Line >= 0) 
        {
            m_Cursor.Column = (int)m_Chunks[m_Cursor.Line].size();
            m_Chunks[m_Cursor.Line].append(copy);
        }
    }

    adjust_cursor_position();
}

void TextEditorModel::select(const int& _Line, const int& _Position)
{
    if(_Line >= m_Chunks.size() || _Position >= m_Chunks[_Line].size()) 
        return;

    if(m_Selection.Ranges.empty())
        m_Selection.Refline = _Line;

    if(m_Selection.Ranges.find(_Line) == m_Selection.Ranges.end())
    {
        if(m_Selection.Ranges.find(_Line - 1) != m_Selection.Ranges.end())
            m_Selection.Ranges[_Line - 1] = {0, (int)m_Chunks[_Line].size() - 1};
        
        if(m_Selection.Ranges.find(_Line + 1) != m_Selection.Ranges.end())
            m_Selection.Ranges[_Line + 1] = {0, (int)m_Chunks[_Line].size() - 1};

        m_Selection.Ranges[_Line] = {INT_MAX, INT_MIN};
    }
    else
    {
        if(m_Selection.Refline >= _Line && m_Selection.Ranges.find(_Line - 1) != m_Selection.Ranges.end())
            m_Selection.Ranges.erase(_Line - 1);
        
        if(m_Selection.Refline <= _Line && m_Selection.Ranges.find(_Line + 1) != m_Selection.Ranges.end())
            m_Selection.Ranges.erase(_Line + 1);
    }

    if(is_selected(_Line, _Position))
    {
        if(std::abs(_Position - m_Selection.Ranges[_Line].Start) < std::abs(_Position - m_Selection.Ranges[_Line].Finish)) 
            m_Selection.Ranges[_Line].Start  = std::max<int>(m_Selection.Ranges[_Line].Start, _Position);
        else 
            m_Selection.Ranges[_Line].Finish  = std::min<int>(m_Selection.Ranges[_Line].Finish, _Position);
    }
    else
    {
        m_Selection.Ranges[_Line].Start  = std::min<int>(m_Selection.Ranges[_Line].Start, _Position);
        m_Selection.Ranges[_Line].Finish = std::max<int>(m_Selection.Ranges[_Line].Finish, _Position);
    }
}

bool TextEditorModel::is_selected(const int& _Line, const int& _Position) const
{
    auto iterator = m_Selection.Ranges.find(_Line);

    if(iterator == m_Selection.Ranges.end()) 
        return false;

    return _Position >= iterator->second.Start && 
           _Position <= iterator->second.Finish;
}

void TextEditorModel::clear_selection()
{
    m_Selection.Ranges.clear();
    m_Selection.Refline = 0;
}

void TextEditorModel::adjust_cursor_position()
{
    if(m_Cursor.Line < 0) 
        m_Cursor.Line = 0;
    else if(m_Cursor.Line >= (int)m_Chunks.size()) 
        m_Cursor.Line = 0;
    else if(m_Chunks.empty()) 
        m_Cursor.Line = 0;
    
    if(!m_Chunks.empty())
    {
        int lineIndex = std::min<int>(m_Cursor.Line, (int)m_Chunks.size() - 1);

        m_Cursor.Column = 
            std::min<int>(std::max<int>(m_Cursor.Column, 0), (int)m_Chunks[lineIndex].size());
    }
    else
    {
        m_Cursor.Column = 0;
    }
}
