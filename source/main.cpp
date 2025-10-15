// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorLauncher.hpp>

// int main(int argc, char *argv[])
// {
//     return Frenchie::Editor::Launcher::execute();
// }

#include <FrenchieCoreTextPieceTable.hpp>

#include <iostream>
#include <string>

struct Buffer
{
public:

    Buffer(const std::wstring& _Text = std::wstring()) : m_Text(_Text)
    {
        m_Lines.push_back(0);

        for(int i = 0; i < (int)m_Text.size(); i++)        
        {
            if (m_Text[i] == '\n')
                m_Lines.push_back(i);
        }
    }

    wchar_t at(const int& _Position) const
    {
        return m_Text[_Position];
    }

    int size() const
    {
        return m_Text.size();
    }

    int line_start_index(const int& _LineNumber) const
    {        
        return _LineNumber > 0 ? m_Lines[std::min<int>(_LineNumber - 1, m_Lines.size() - 1)] + 1 : 0;
    }

    int line_end_index(const int& _LineNumber) const
    {
        return m_Lines[std::min<int>(_LineNumber, m_Lines.size() - 1)];
    }

    int line_closest_to_buffer_offset(int _Offset) const
    {
        if(m_Lines.size() <= 0 || _Offset < 0)
            return 0;

        if(_Offset > m_Lines[m_Lines.size() - 1])
            return m_Lines.size() - 1;

        int low = 0;
        int high = m_Lines.size() - 1;

        while(low <= high) 
        {
            int mid = low + (high - low) / 2;

            if (m_Lines[mid] == _Offset)
                return mid;

            if (m_Lines[mid] < _Offset)
                low = mid + 1;
            else
                high = mid - 1;
        }

        return std::min<int>(low, m_Lines.size() - 1);
    }

    int lines_count() const
    {
        return (int)m_Lines.size();
    }

protected:
    std::wstring     m_Text;
    std::vector<int> m_Lines;
};

class Piece
{
public:

    Piece(const Buffer* _Buffer, int _Start, int _Length) : 
        m_Buffer(_Buffer),
        m_Start(_Start),
        m_Length(_Length){}
    
    const Buffer* buffer() const
    {
        return m_Buffer;
    }

    wchar_t at(int _Position) const
    {
        return m_Buffer->at(m_Start + _Position);
    }

    int start() const
    {
        return m_Start;
    }

    int end() const
    {
        return m_Start + m_Length;
    }

    int length() const
    {
        return m_Length;
    }

protected:

    const int     m_Start      = 0;
    const int     m_Length     = 0;
    const Buffer* m_Buffer     = nullptr;
};

class PieceTable
{
public:

    typedef std::list<Piece>::const_iterator const_iterator;
    typedef std::list<Piece>::iterator iterator;

    PieceTable(const std::wstring& _Text) : 
        m_Immutable(_Text),
        m_Appendable(std::wstring()),
        m_Pieces({Piece(&m_Immutable, 0, (int)m_Immutable.size())}){}
    
    ~PieceTable(){}

    void insert(const int& _Line, const int& _Position = 0, const std::wstring& _What = std::wstring())
    {
        // look for the line
        int currentLine = 0;

        for(auto it = begin(); it != end(); it++)
        {
            int firstLine = it->buffer()->line_closest_to_buffer_offset(it->start()) + currentLine;
            int lastLine  = it->buffer()->line_closest_to_buffer_offset(it->end()) + currentLine;

            std::cout << "buffer start " << it->start() << "\n";
            std::cout << "buffer end   " << it->end() << "\n";

            std::cout << "firstLine " << firstLine << "\n";
            std::cout << "lastLine " << lastLine << "\n";

            if(_Line >= firstLine && _Line <= lastLine)
            {
                std::cout << "found " << _Line << " offset " << it->buffer()->line_start_index(_Line) << "\n";

                // int availableSpace    = it->buffer()->size() - it->buffer()->line_start_index(_Line);
                // int insertionPosition = _Position;

                // if(_Position > availableSpace)
                // {

                // }

                break;
            }

            currentLine += (lastLine - firstLine) + 1;
        }
    }

    void remove(const int& _Line, const int& _Position, const int& _Count)
    {
    }

    const_iterator begin() const
    {
        return m_Pieces.begin();
    }

    const_iterator end() const
    {
        return m_Pieces.end();
    }

protected:
    const   Buffer           m_Immutable;
    mutable Buffer           m_Appendable;
    mutable std::list<Piece> m_Pieces;
};

int main()
{
    PieceTable table(L"Hello\nWorld");

    table.insert(1, 7, L"123");

    // std::wstring text = L"Hello\nWorld";

    // Buffer buffer(text);

    // std::cout << "\n";

    // int lineNumber = buffer.line_closest_to_buffer_offset(6);

    // for(int i = buffer.line_start_index(lineNumber); i < buffer.line_end_index(lineNumber); i++)
    // {
    //     std::cout << Frenchie::Core::String::as_utf8(std::wstring(1, buffer.at(i))) << "\n";
    // }

    // int first = buffer.line_closest_to_buffer_offset(2);
    // int last  = buffer.line_closest_to_buffer_offset(6);

    // std::cout << "first " << first << "\n";
    // std::cout << "last  " << last << "\n";
    // std::cout << "line count " << (last - first + 1) << "\t" << buffer.lines_count() << "\n";

    return 0;
}