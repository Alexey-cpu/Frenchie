#pragma once

#include <vector>
#include <string>

namespace Frenchie
{
    namespace TextEditor
    {
        class TextEditorModel final
        {
        public:
            TextEditorModel();
            ~TextEditorModel();

            // getters
            int get_text_lines_count() const;
            int get_cursros_line() const;
            int get_cursros_column() const;
            std::wstring& get_text_line(int) const;
            bool is_dirty() const;

            // setters
            void set_cursor_line(const int&);
            void set_cursor_column(const int&);
            void set_dirty(const bool&);

            void reset(const std::string&);
            void reset(const std::wstring&);
            void append(const std::string&);
            void append(const std::wstring&);

            void move_cursor_left();
            void move_cursor_right();
            void move_cursor_up();
            void move_cursor_down();
            void next_line();
            void move_back();

            void insert(const int& _Line, const int& _Position, const std::string& _What);
            void insert(const int& _Line, const int& _Position, const std::wstring& _What);

        protected:

            mutable std::vector<std::wstring> m_Chunks {std::vector<std::wstring>()};
            mutable bool                      m_Dirty{false};

            struct Cursor
            {
                int Line     = 0;
                int Column   = 0;
            } m_Cursor{0, 0};

            // service methods
            void adjust_cursor_position();
        };
    }
}