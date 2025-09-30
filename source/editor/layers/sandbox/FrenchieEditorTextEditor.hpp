#pragma once

// Core
#include <FrenchieCoreProcess.hpp>
#include <FrenchieCoreRegex.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

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
		class SyntaxHighlighter
		{
		public:
			struct RegularExpressionRule
			{
                enum Type
                {
                    SINGLE_LINE,
                    MULTILINE_START,
                    MULTILINE_FINISH
                };

				RegularExpressionRule(
                    const std::wstring& _Pattern = std::wstring(), 
                    const unsigned int& _Color   = 0, 
                    Type                _Type    = Type::SINGLE_LINE, 
                    int                 _Context = -1) : 
					Pattern(_Pattern), Color(_Color), Type(_Type), Context(_Context){}

				std::wstring Pattern = std::wstring();
				unsigned int Color   = 0;
                Type         Type    = Type::SINGLE_LINE;
                int          Context = -1;
			};

			struct RegularExpressionEstimationResult
			{
				RegularExpressionEstimationResult(
					const Frenchie::Core::Regex::Match& _Match = Frenchie::Core::Regex::Match(), 
					const unsigned int&                 _Color = 0) : 
					Match(_Match), Color(_Color){}

				Frenchie::Core::Regex::Match Match;
				unsigned int                 Color  = 0;
			};

			typedef std::map<int, RegularExpressionEstimationResult> regexEstimationResults;

			regexEstimationResults highlight(
				const std::wstring&                       _Contents, 
				const std::vector<RegularExpressionRule>& _Rules,
                const int&                                _LineNumber,
				const unsigned int&                       _DefaultColor)
			{
				regexEstimationResults colors = 
					{{0, RegularExpressionEstimationResult(Frenchie::Core::Regex::Match(), _DefaultColor)}};

				for(auto&& rule : _Rules)
				{
					auto matches = Frenchie::Core::Regex::match(_Contents, rule.Pattern);

                    if(!matches.empty())
                    {
                        if(rule.Type == RegularExpressionRule::Type::MULTILINE_START) 
                        {
                            if(m_MultilineRules.empty() || m_MultilineRules.top().second.Context == rule.Context)
                            {
                                std::cout << "push \n";
                                if(!m_MultilineRules.empty() && m_MultilineRules.top().first == _LineNumber)
                                {

                                }
                                else
                                {
                                    m_MultilineRules.push({_LineNumber, rule});
                                }
                            }
                        }
                        else if(rule.Type == RegularExpressionRule::Type::MULTILINE_FINISH)
                        {
                            std::cout << "pop \n";

                            if(!m_MultilineRules.empty() && rule.Context == m_MultilineRules.top().second.Context) 
                                m_MultilineRules.pop();
                        }
                    }
                    else if(!m_MultilineRules.empty() && m_MultilineRules.top().first == _LineNumber)
                    {
                        m_MultilineRules.pop();
                    }

					for(auto&& match : matches)
					{
						colors[match.Start] = 
							RegularExpressionEstimationResult(match, (m_MultilineRules.empty() ? rule.Color : m_MultilineRules.top().second.Color));
					}
				}

                // add missing ranges
				regexEstimationResults results;

				for(auto&& color : colors)
				{
					int source = color.second.Match.Finish;
					int target = color.second.Match.Finish;

					do
					{
						target++;
					} 
					while(colors.find(target) == colors.end() && target < (int)_Contents.size());

					if(source < (int)_Contents.size())
					{
						results[source] = 
							RegularExpressionEstimationResult(
								Frenchie::Core::Regex::Match(source, target),
                                (m_MultilineRules.empty() ? _DefaultColor : m_MultilineRules.top().second.Color)
							);
					}
				}

				for(auto&& result : results) 
					colors[result.first] = result.second;

                // finalize without overlapping ranges
                regexEstimationResults output;

                int range = 0;

                for(auto&& color : colors)
                {
                    if(range >= (int)_Contents.size()) break;

                    output[color.second.Match.Start] = color.second;

                    range += (color.second.Match.Finish - color.second.Match.Start);
                }

				return output;
			}

        protected:
            std::stack<std::pair<int, RegularExpressionRule>> m_MultilineRules;
		};

        class TextEditorModel
        {
        public:

            const char CURSOR = '|';
            const char ENTER  = '\n';
            const char TAB    = '\t';

            TextEditorModel()
            {
                m_Chunks.push_back(L" ");
                m_Cursor = {0, 0};
            }

            void reset()
            {
                m_Chunks.clear();
                m_Cursor = {0, 0};
                m_Chunks.push_back(L" ");
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


        protected:

            mutable std::vector<std::wstring> m_Chunks;

            struct Cursor
            {
                int Line     = 0;
                int Column   = 0;
            } m_Cursor = {0, 0};
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

            std::vector<SyntaxHighlighter::RegularExpressionRule> m_Patterns = 
            {
                SyntaxHighlighter::RegularExpressionRule(
                    L"alignas|alignof|and|and_eq|asm|auto|bitand|bitor|bool|break|case|catch|char|char8_t|char16_t|char32_t|class|compl|concept|const|consteval|constexpr|constinit|const_cast|continue|co_await|co_return|co_yield|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|not|not_eq|nullptr|operator|or|or_eq|private|protected|public|reflexpr|register|reinterpret_cast|requires|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|synchronized|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq", 
                    IM_COL32(0, 0, 255, 255)),
                SyntaxHighlighter::RegularExpressionRule(
                    LR"(/\*)", 
                    IM_COL32(0, 255, 0, 255), 
                    SyntaxHighlighter::RegularExpressionRule::MULTILINE_START, 
                    1),
                SyntaxHighlighter::RegularExpressionRule(
                    LR"(\*/)", 
                    IM_COL32(0, 255, 0, 255), 
                    SyntaxHighlighter::RegularExpressionRule::MULTILINE_FINISH, 
                    1),
                // SyntaxHighlighter::RegularExpressionRule(
                //     L".**//", 
                //     IM_COL32(0, 255, 0, 255), 
                //     SyntaxHighlighter::RegularExpressionRule::MULTILINE_FINISH, 
                //     1)
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