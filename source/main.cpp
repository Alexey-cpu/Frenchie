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

class Buffer
{
public:

    Buffer(const std::wstring& _Text = std::wstring())
    {
        append(_Text);
    }

    wchar_t at(const int& _Position) const
    {
        return m_Text[_Position];
    }

    int size() const
    {
        return (int)m_Text.size();
    }

    void append(const std::wstring& _What)
    {
        int start = (int)m_Text.size();
        int end   = start;

        m_Text.append(_What);

        for(; end < (int)m_Text.size(); end++)        
        {
            if (m_Text[end] == '\n')
            {
                m_Lines.push_back({start, end});
                start = end;
            }
        }

        m_Lines.push_back({start, end});
    }

    int line_start_index(const int& _LineNumber) const
    {
        return m_Lines[_LineNumber].first;
    }

    int line_end_index(const int& _LineNumber) const
    {
        return m_Lines[_LineNumber].second;
    }

    int line_closest_to_buffer_offset(int _Offset) const
    {
        // for (int i = 0; i < (int)m_Lines.size(); i++)
        // {
        //     if(_Offset >= m_Lines[i].first && _Offset <= m_Lines[i].second)
        //         return i;
        // }
        
        // return (int)m_Lines.size() - 1;

        // if(m_Lines.empty() || _Offset < 0)
        //     return 0;

        // if(_Offset > m_Lines[m_Lines.size() - 1].second)
        //     return (int)m_Lines.size() - 1;

        int low = 0;
        int high = (int)m_Lines.size() - 1;

        while(low <= high) 
        {
            int mid = low + (high - low) / 2;

            if (m_Lines[mid].second == _Offset)
                return mid;

            if (m_Lines[mid].second < _Offset)
                low = mid + 1;
            else
                high = mid - 1;
        }

        return std::min<int>(low, (int)m_Lines.size() - 1);
    }

    int lines_count() const
    {
        return (int)m_Lines.size();
    }

//protected:
    std::string m_Type = "default";

    std::wstring                     m_Text;
    std::vector<std::pair<int, int>> m_Lines;
};

class Piece
{
public:

    Piece(const Buffer* _Buffer, int _Start, int _Length) : 
        m_Buffer(_Buffer),
        m_Start(_Start),
        m_Length(_Length){}

    // getters
    const Buffer* get_buffer() const
    {
        return m_Buffer;
    }

    int get_start() const
    {
        return m_Start;
    }

    int get_end() const
    {
        return m_Start + m_Length;
    }

    int get_length() const
    {
        return m_Length;
    }

    int get_offset() const
    {
        return m_Offset;
    }

    // setters
    void set_start(const int& _Value) const
    {
        m_Start = _Value;
    }

    void set_length(const int& _Value) const
    {
        m_Length = _Value;
    }

    void set_offset(const int& _Value) const
    {
        m_Offset = _Value;
    }

protected:

    mutable int     m_Start  = 0;
    mutable int     m_Length = 0;
    mutable int     m_Offset = 0;
    const   Buffer* m_Buffer = nullptr;
};

class PieceTable
{
public:

    typedef std::list<Piece>::const_iterator const_iterator;
    typedef std::list<Piece>::iterator iterator;

    PieceTable(const std::wstring& _Text) : 
        m_Immutable(_Text),
        m_Appendable(std::wstring()),
        m_Pieces({Piece(&m_Immutable, 0, (int)m_Immutable.size())})
    {
        m_Immutable.m_Type = "immutable";
        m_Appendable.m_Type = "appendable";
    }
    
    ~PieceTable(){}

    const_iterator find(const int& _Line, const int& _Position = 0)
    {
        int currentLine         = 0;
        int currentLinePosition = 0;

        for(auto lineIterator = begin(); lineIterator != end(); lineIterator++)
        {
            int firstLine = lineIterator->get_buffer()->line_closest_to_buffer_offset(lineIterator->get_start());
            int lastLine  = lineIterator->get_buffer()->line_closest_to_buffer_offset(lineIterator->get_end());

            for(;firstLine <= lastLine; firstLine++, currentLine++)
            {
                if(currentLine < _Line)
                    continue;

                std::cout << "found currentLine " << currentLine << "\n";

                for(auto linePositionIterator = lineIterator; linePositionIterator != end(); linePositionIterator++)
                {
                    int lineStarts = std::max(linePositionIterator->get_buffer()->line_start_index(firstLine), linePositionIterator->get_start());
                    int lineEnds   = std::min(linePositionIterator->get_buffer()->line_end_index(firstLine), linePositionIterator->get_end());

                    for(int offset = lineStarts; offset < lineEnds; offset++)
                        std::cout << Frenchie::Core::String::as_utf8(std::wstring(1, linePositionIterator->get_buffer()->at(offset)));

                    for(int offset = lineStarts; offset < lineEnds && currentLinePosition <= _Position; offset++, currentLinePosition++)
                    {
                        if(currentLinePosition < _Position)
                            continue;

                        linePositionIterator->set_offset(offset);
                        return linePositionIterator;
                    }
                }
            }
        }

        return end();
    }

    void insert(const int& _Line, const int& _Position = 0, const std::wstring& _What = std::wstring())
    {
        auto found = find(_Line, _Position);

        if(found == end())
            return;

        auto iterator = m_Pieces.insert(std::next(found), Piece(&m_Appendable, (int)m_Appendable.size(), (int)_What.size()));
        m_Pieces.insert(
            std::next(iterator),
            Piece(found->get_buffer(), found->get_offset(), found->get_end() - found->get_offset())
        );

        found->set_length(found->get_offset() - found->get_start());

        m_Appendable.append(_What);
    }

    void remove(const int& _Line, const int& _Position, const int& _Count)
    {
    }

    int size() const
    {
        return (int)m_Pieces.size();
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

    // info
    mutable   Buffer           m_Immutable;
    mutable Buffer           m_Appendable;
    mutable std::list<Piece> m_Pieces;
};

void show(const PieceTable& _Table)
{
    std::cout << "\n";

    std::cout << _Table.size() << "\n";

    for(auto it = _Table.begin(); it != _Table.end(); it++)
    {
        //std::cout << it->get_start() << "\t" << it->get_end() << "\n";

        for (int i = it->get_start(); i < it->get_end(); i++)
        {
            std::cout << Frenchie::Core::String::as_utf8(std::wstring(1, it->get_buffer()->at(i)));
        }
    }

    std::cout << "\n";
}

void find(const PieceTable& _Table)
{
    for(auto it = _Table.begin(); it != _Table.end(); it++)
    {
        for (int i = it->get_start(); i < it->get_end(); i++)
        {
            if(it->get_buffer()->at(i) == '\n') break;
        }
    }
}

int main()
{
	// //	setup text buffer
	// std::wstring textBuffer;

	// for (size_t j = 0; j < 1e6; j++)
	// {
	// 	for (size_t i = 0; i < 10; i++)
	// 		textBuffer.append(L"for(int i = 0; i < 10; i++)");

	// 	textBuffer.append(L"\n");
	// }

	// //m_TextModel->append(textBuffer);

    // PieceTable table(textBuffer);

    // auto then = Frenchie::Core::Time::tic();

    // for (size_t j = 300000; j < 300000 + 100; j++) 
    // {
    //     table.find(j);
    // }

    // //for (size_t j = 0; j < 10; j++) find(table);

    // std::cout << Frenchie::Core::Time::elapsed<std::chrono::milliseconds>(then, Frenchie::Core::Time::tic()) << " ms \n";

    std::wstring textBuffer = L"Hello\nWorld";
    PieceTable table(textBuffer);

    show(table);

    table.insert(0, 0, L"123");
    show(table);

    table.insert(1, 1, L"456");
    show(table);

    // table.insert(2, 2, L"789");
    // show(table);

    return 0;
}