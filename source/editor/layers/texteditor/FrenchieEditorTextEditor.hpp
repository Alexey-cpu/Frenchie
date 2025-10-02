#pragma once

// Core
#include <FrenchieCoreProcess.hpp>
#include <FrenchieCoreRegex.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// TextEditor
#include <FrenchieTextEditorSyntaxHighlighter.hpp>

using namespace Frenchie::TextEditor;

// STL
#include <optional>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        class TextEditorModel
        {
        public:

            const char CURSOR = '|';
            const char ENTER  = '\n';
            const char TAB    = '\t';

            TextEditorModel()
            {
                m_Chunks.push_back(std::wstring());
                m_Cursor = {0, 0};
            }

            void reset(const std::string& _TextBuffer)
            {
                m_Chunks.clear();
                m_Cursor = {0, 0};
                
                append((_TextBuffer.empty() ? " " : _TextBuffer));
            }

            void reset(const std::wstring& _TextBuffer)
            {
                m_Chunks.clear();
                m_Cursor = {0, 0};
                
                append((_TextBuffer.empty() ? L" " : _TextBuffer));
            }

            void append(const std::string& _TextBuffer)
            {
                for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < _TextBuffer.size(); textBegin = ++textEnd, ++lineNumber)
                {
                    while(textEnd < _TextBuffer.size() && _TextBuffer[textEnd] != ENTER) ++textEnd;

                    m_Chunks.push_back(Frenchie::Core::String::as_wide(std::string(&_TextBuffer[textBegin], &_TextBuffer[textEnd])));
                }
            }

            void append(const std::wstring& _TextBuffer)
            {
                for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < _TextBuffer.size(); textBegin = ++textEnd, ++lineNumber)
                {
                    while(textEnd < _TextBuffer.size() && _TextBuffer[textEnd] != ENTER) ++textEnd;

                    m_Chunks.push_back(std::wstring(&_TextBuffer[textBegin], &_TextBuffer[textEnd]));
                }
            }

            void move_cursor_left()
            {
                --m_Cursor.Column;
                adjust_cursor_position();
            }
            
            void move_cursor_right()
            {
                m_Cursor.Column++;
                adjust_cursor_position();
            }
            
            void move_cursor_up()
            {
                --m_Cursor.Line;
                adjust_cursor_position();
            }
            
            void move_cursor_down()
            {
                ++m_Cursor.Line;
                adjust_cursor_position();
            }

            void next_line()
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

            void move_back()
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

            void insert(int _Line, int _Position, std::string _What)
            {
                if(_Line >= (int)m_Chunks.size() || _What.empty()) 
                    return;

                m_Chunks[_Line].insert(_Position, Frenchie::Core::String::as_wide(_What));

                adjust_cursor_position();
            }

            void insert(int _Line, int _Position, std::wstring _What)
            {
                if(_Line >= (int)m_Chunks.size() || _What.empty()) 
                    return;

                m_Chunks[_Line].insert(_Position, _What);

                adjust_cursor_position();
            }

            void adjust_cursor_position()
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

            // getters
            int get_text_lines_count() const
            {
                return (int)m_Chunks.size();
            }

            int get_cursros_line() const 
            {
                return m_Cursor.Line;
            }

            int get_cursros_column() const 
            {
                return m_Cursor.Column;
            }

            std::wstring& get_text_line(int _Line) const
            {
                if(m_Chunks.empty()) 
                    m_Chunks.push_back(std::wstring());

                return _Line < (int)m_Chunks.size() ? m_Chunks[_Line] : m_Chunks[std::max<int>((int)m_Chunks.size() - 1, 0)];
            }

            // setters
            void set_cursor_line(int _Line)
            {
                m_Cursor.Line = _Line;
                adjust_cursor_position();
            }

            void set_cursor_column(int _Position)
            {
                m_Cursor.Column = _Position;
                adjust_cursor_position();
            }

            mutable bool m_IsDirty{false};
            mutable std::vector<std::wstring> m_Chunks;

            struct Cursor
            {
                int Line     = 0;
                int Column   = 0;
            } m_Cursor = {0, 0};
        protected:
        };

        class TextEditor : public Frenchie::Application::Layer
        {
        public:
            TextEditor();
            virtual ~TextEditor();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        protected:

            enum Layers : int
            {
                BACKGROUND,
                TEXT,
                CURSOR,
                COUNT
            };

            struct Timer
            {
                uint64_t LaunchTime {0};
                uint64_t CurrentTime{0};
                uint64_t Elapsed    {0};
            };

            Timer      m_Timer;

            std::mutex m_Mutex;
            ImVec2     m_ScrollPos;
            ImRect     m_TextRect;
            ImRect     m_LineNumbersRect;
            ImVec2     m_CursorPosition;
            int        m_CurrentlyHoveredLine = 0;

            SyntaxHighlighter m_Highlighter;

            std::vector<RegexRule> m_Patterns = 
            {
                RegexRule(
                    L"alignas|alignof|and|and_eq|asm|auto|bitand|bitor|bool|break|case|catch|char|char8_t|char16_t|char32_t|class|compl|concept|const|consteval|constexpr|constinit|const_cast|continue|co_await|co_return|co_yield|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|not|not_eq|nullptr|operator|or|or_eq|private|protected|public|reflexpr|register|reinterpret_cast|requires|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|synchronized|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq", 
                    IM_COL32(0, 0, 255, 255)),
                RegexRule(
                    L"for", 
                    IM_COL32(255, 0, 0, 255)),

                RegexRule(
                    LR"([\(\)\{\}\[\]])", 
                    IM_COL32(0, 255, 0, 255)),

                // single line comment
                RegexRule(
                    L"//.*", 
                    IM_COL32(0, 255, 0, 255)),

                // multiline comment start
                RegexRule(
                    LR"(/\*)", 
                    IM_COL32(0, 255, 0, 255), 
                    RegexRule::MULTILINE_START),
                
                // multiline comment end
                RegexRule(
                    LR"(\*/)", 
                    IM_COL32(0, 255, 0, 255), 
                    RegexRule::MULTILINE_FINISH)
            };

            std::shared_ptr<TextEditorModel> m_TextModel{nullptr};

            // commands
            void move_cursor_left_command();
            void move_cursor_right_command();
            void move_cursor_down_command();
            void move_cursor_up_command();
            void move_next_line_command();
            void move_back_commnad();
            void insert_symbol_command();

            // drawers
            void draw_text_line_numbers();
            void draw_text_contents();
            void draw_status_panel();

            // static API
            static ImVec2 calculate_text_size(const char* _Begin, const char* _End = nullptr);
            static ImVec2 calculate_text_size(const std::wstring&);
            static ImU32  calculate_color(const ImVec4&);
        };
    }
}