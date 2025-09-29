#include <FrenchieEditorTextEditor.hpp>

using namespace Frenchie::Editor;

#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include <map>

// PCRE
#define PCRE2_CODE_UNIT_WIDTH 16
#include <pcre2.h>

class Colors
{
public:
	struct RegularExpressionRule
	{
		RegularExpressionRule(const std::wstring& Pattern, const unsigned int& _Color) : 
			Pattern(Pattern), Color(_Color){}

		std::wstring Pattern = std::wstring();
		unsigned int Color   = 0;
	};

	struct RegularExpressionEstimationResult
	{
		RegularExpressionEstimationResult(){}

		RegularExpressionEstimationResult(const int& _Position, const int& _Length, const unsigned int& _Color) : 
			Position(_Position), Length(_Length), Color(_Color){}

		int          Position = 0;
		int          Length   = 0;
		unsigned int Color    = 0;
	};

	static std::map<int, RegularExpressionEstimationResult> colorize(
		const std::wstring&                       _Contents, 
		const std::vector<RegularExpressionRule>& _Rules, 
		unsigned int                              _DefaultColor = 1)
	{
		std::map<int, RegularExpressionEstimationResult> colors;

		colors[0] = RegularExpressionEstimationResult(0, 0, _DefaultColor);

		PCRE2_SPTR subject = (PCRE2_SPTR)_Contents.c_str();

		for(auto&& rule : _Rules)
		{
			PCRE2_SPTR pattern = (PCRE2_SPTR)rule.Pattern.c_str();

			int errorcode;
			PCRE2_SIZE erroroffset;

			pcre2_code *re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroroffset, NULL);

			if (re == NULL) 
			{
				std::cout << "Handle compilation error " << "\n";
				return  colors;
			}

			pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
			if (match_data == NULL) 
			{
				// Handle compilation error
				pcre2_code_free(re);
				return colors;
			}

			PCRE2_SIZE startoffset = 0;
			int rc;

			while ((rc = pcre2_match(re, subject, (PCRE2_SIZE)wcslen((wchar_t*)subject), startoffset, 0, match_data, NULL)) >= 0) 
			{
				PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);

				colors[(int)ovector[0]] = RegularExpressionEstimationResult((int)ovector[0], (int)(ovector[1] - ovector[0]), rule.Color);

				// Advance startoffset for the next search
				startoffset = ovector[1];

				if (ovector[0] == ovector[1]) 
				{ // Handle zero-length matches to prevent infinite loops
					startoffset++;
				}
			}

			if (rc != PCRE2_ERROR_NOMATCH) 
			{
				// Handle other matching errors
			}

			pcre2_match_data_free(match_data);
			pcre2_code_free(re);
		}

		auto result = colors;

		// handle untill the end
		for(auto&& color : colors)
		{
			int source = color.second.Position + color.second.Length;
			int target = color.second.Position + color.second.Length;

			do
			{
				target++;
			} 
			while(colors.find(target) == colors.end() && target < (int)_Contents.size());

			if(source < (int)_Contents.size())
			{
				result[source] = RegularExpressionEstimationResult(
					source, 
					target - source, _DefaultColor);
			}
		}

		return result;
	}
};

class Helpers
{
public:

	static inline int ImTextCharToUtf8_inline(char* buf, int buf_size, unsigned int c)
	{
		if (c < 0x80)
		{
			buf[0] = (char)c;
			return 1;
		}
		if (c < 0x800)
		{
			if (buf_size < 2) return 0;
			buf[0] = (char)(0xc0 + (c >> 6));
			buf[1] = (char)(0x80 + (c & 0x3f));
			return 2;
		}
		if (c < 0x10000)
		{
			if (buf_size < 3) return 0;
			buf[0] = (char)(0xe0 + (c >> 12));
			buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
			buf[2] = (char)(0x80 + ((c ) & 0x3f));
			return 3;
		}
		if (c <= 0x10FFFF)
		{
			if (buf_size < 4) return 0;
			buf[0] = (char)(0xf0 + (c >> 18));
			buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
			buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
			buf[3] = (char)(0x80 + ((c ) & 0x3f));
			return 4;
		}
		// Invalid code point, the max unicode is 0x10FFFF
		return 0;
	}

	static int ImTextCharToUtf8(char out_buf[5], unsigned int c)
	{
		int count = ImTextCharToUtf8_inline(out_buf, 5, c);
		out_buf[count] = 0;
		return count;
	}
};

// add sandbox elements into main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenTextEditorDebug : 
                public Frenchie::Application::Command::Registry<OpenTextEditorDebug, void*>
            {
            public:

                OpenTextEditorDebug(void* _Sender = nullptr) : 
                    Frenchie::Application::Command::Registry<OpenTextEditorDebug, void*>(_Sender){}
                virtual ~OpenTextEditorDebug(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<TextEditor>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Debug::Text Editor");
                }
            };

            const bool openTextEditorDebug = OpenTextEditorDebug::registerFactory();
        }
    }
}

enum DrawLayers : int
{
	Background,
	Text,
	Cursor,
	Count
};

const char CURSOR = '|';
const char ENTER  = '\n';
const char TAB    = '\t';

// view
TextEditor::TextEditor() : Frenchie::Application::Layer(STRINGIFY(TextEditor)){}
TextEditor::~TextEditor(){}

bool TextEditor::awake()
{
//     m_TextBuffer = R"(#include <iostream>
// int main(int, int)
// {
//     std::cout << "Hello world \n";
//     return 0;
// })";

	// fill buffer
	Frenchie::Core::ThreadPool::instance()->enqueue(
		[this]()
		{
			//	setup text buffer
			std::string textBuffer;

			for (size_t j = 0; j < 100; j++)
			{
				for (size_t i = 0; i < 4; i++)
				{
					textBuffer.append("for(int i = 0; i < 10; i++)");
				}

				textBuffer.append("\n");
			}

			// load text
			// std::ifstream ifsream(std::filesystem::path(L"C:/SDK/Qt_Projects/PowerCAD/tests/models/rastrWin3/computable/Центр/Центр неопознанное/1_Летний минимум_2027_ГОСТ_п.5.3_Г.pwrct"));

			// std::string textBuffer = 
			// 	std::string(
			// 		(std::istreambuf_iterator<char>(ifsream)), 
			// 		(std::istreambuf_iterator<char>()));

			for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < textBuffer.size(); textBegin = ++textEnd, ++lineNumber)
			{
				while(textEnd < textBuffer.size() && textBuffer[textEnd] != ENTER) ++textEnd;

				m_Chunks.push_back(
					Frenchie::Core::String::as_wide(std::string().append(&textBuffer[textBegin], &textBuffer[textEnd])));
			}

			//m_Chunks = chunks;
		}
	);

	// initialize editor cursor
	m_Cursor.Column = 0;
	m_Cursor.Line     = 0;

	// initialize navigation cursor timer
	m_Timer.LaunchTime  = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	m_Timer.CurrentTime = 0;
	m_Timer.Elapsed     = 0;

	m_Timer1.LaunchTime  = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	m_Timer1.CurrentTime = 0;
	m_Timer1.Elapsed     = 0;

    return true;
}

void TextEditor::frame_update()
{
	std::lock_guard<std::mutex> lock(m_Mutex);

    ImGui::Begin("TextEditor", &m_Opened);
    {
		ImGui::BeginChild("TextEditorContents", 
			ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.9f),
			ImGuiChildFlags_::ImGuiChildFlags_Borders, 
			ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
		{
			drawTextLineNumbers();
			
			ImGui::SameLine();
			
			drawTextContents();
			
			ImGui::EndChild();
		}

		ImGui::End();
    }
}

bool TextEditor::allows_multiple_instances() const 
{
    return false;
}

void TextEditor::moveCursorLeft()
{
	if(m_Cursor.Column > 0)
		--m_Cursor.Column;
}

void TextEditor::moveCursorRight()
{
	if(m_Cursor.Column < m_Chunks[m_Cursor.Line].size()) 
		m_Cursor.Column++;
}

void TextEditor::moveCursorDown()
{
	if(m_Cursor.Line >= m_Chunks.size() - 1) 
		return;

	++m_Cursor.Line;

	if(m_Cursor.Column > m_Chunks[m_Cursor.Line].size())
		m_Cursor.Column = 0;
}

void TextEditor::moveCursorUp()
{
	if(m_Cursor.Line < 0) 
		return;

	--m_Cursor.Line;

	if(m_Cursor.Column > m_Chunks[m_Cursor.Line].size())
		m_Cursor.Column = 0;
}

void TextEditor::moveNextLine()
{
	Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
		[this]()
		{
			if(m_Cursor.Line < 0 || m_Cursor.Line > m_Chunks.size()) 
				return;

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
				m_Chunks.insert(m_Chunks.begin() + m_Cursor.Line, L" ");
			}
		}
	);
}

void TextEditor::removeSymbol()
{
	Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
		[this]()
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
		}
	);
}

void TextEditor::handleUserInput()
{
	// Process regular text input (before we check for Return because using some IME will effectively send a Return?)
	// We ignore CTRL inputs, but need to allow ALT+CTRL as some keyboards (e.g. German) use AltGR (which _is_ Alt+Ctrl) to input certain characters.
	ImGuiIO& io = ImGui::GetIO();

	const bool is_osx = io.ConfigMacOSXBehaviors;
	
	const bool ignore_char_inputs = (io.KeyCtrl && !io.KeyAlt) || (is_osx && io.KeyCtrl);
	
	auto onCharPressed = [this](unsigned int c)
	{
		// retrieve user input in UTF-8 codec
		char utf8[5];
		int  count = Helpers::ImTextCharToUtf8(utf8, c);

		// insert user input into a given line as wide character string
		m_Chunks[m_Cursor.Line].insert(m_Cursor.Column++, Frenchie::Core::String::as_wide(std::string(utf8, count)));
	};

	if (ImGui::Shortcut(ImGuiKey_Tab, ImGuiInputFlags_Repeat))
	{
		unsigned int c = '\t'; // Insert TAB
		onCharPressed(c);
	}
	else if (io.InputQueueCharacters.Size > 0)
	{
		if (!ignore_char_inputs)
		{
			for (int n = 0; n < io.InputQueueCharacters.Size; n++)
			{
				// Insert character if they pass filtering
				unsigned int c = (unsigned int)io.InputQueueCharacters[n];
				if (c == '\t') // Skip Tab, see above.
					continue;

				onCharPressed(c);
			}
		}

		io.InputQueueCharacters.resize(0);
	}
}

void TextEditor::drawTextLineNumbers()
{
	ImGui::BeginChild("TextBufferLineNumbers", 
		ImVec2(TextEditor::calculate_text_size(std::to_string(INT_MAX).c_str()).x, ImGui::GetContentRegionAvail().y), 
		ImGuiChildFlags_::ImGuiChildFlags_Borders, 
		ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoInputs);
	{
		ImGui::GetWindowDrawList()->ChannelsSplit(DrawLayers::Count);

		// draw background
		ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Background);
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImGui::GetCursorScreenPos(),
			ImGui::GetCursorScreenPos() + ImVec2((float)INT_MAX, (float)INT_MAX),
			IM_COL32(ImGui::GetStyle().Colors[ImGuiCol_FrameBg].x * 255.f,
						ImGui::GetStyle().Colors[ImGuiCol_FrameBg].y * 255.f,
						ImGui::GetStyle().Colors[ImGuiCol_FrameBg].z * 255.f,
						ImGui::GetStyle().Colors[ImGuiCol_FrameBg].w * 255.f));

		ImGuiListClipper clipper;
		clipper.Begin((int)m_Chunks.size());

		while(clipper.Step())
		{
			for (int lineNumber = clipper.DisplayStart; lineNumber < clipper.DisplayEnd; lineNumber++)
			{
				ImRect rowRect = ImRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize()));

				ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Background);

				if(lineNumber == m_CurrentlyHoveredLine)
				{
					ImGui::GetWindowDrawList()->AddRectFilled(
						rowRect.Min, 
						rowRect.Max, 
						IM_COL32(ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].x * 255.f,
							ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].y * 255.f,
							ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].z * 255.f,
							ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].w * 255.f)
						);
				}
				else if(lineNumber % 2 == 0)
				{
					ImGui::GetWindowDrawList()->AddRectFilled(
						rowRect.Min,
						rowRect.Max,
						IM_COL32(ImGui::GetStyle().Colors[ImGuiCol_TableRowBg].x * 255.f,
									ImGui::GetStyle().Colors[ImGuiCol_TableRowBg].y * 255.f,
									ImGui::GetStyle().Colors[ImGuiCol_TableRowBg].z * 255.f,
									ImGui::GetStyle().Colors[ImGuiCol_TableRowBg].w * 255.f));
				}
				else
				{
					ImGui::GetWindowDrawList()->AddRectFilled(
						rowRect.Min,
						rowRect.Max,
						IM_COL32(ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt].x * 255.f,
									ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt].y * 255.f,
									ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt].z * 255.f,
									ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt].w * 255.f));
				}

				// draw text
				ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Text);
				ImGui::GetWindowDrawList()->AddText(
					rowRect.GetTL(), 
					IM_COL32(ImGui::GetStyle().Colors[ImGuiCol_Text].x * 255.f,
								ImGui::GetStyle().Colors[ImGuiCol_Text].y * 255.f,
								ImGui::GetStyle().Colors[ImGuiCol_Text].z * 255.f,
								ImGui::GetStyle().Colors[ImGuiCol_Text].w * 255.f),
					std::to_string(lineNumber).c_str());

				ImGui::ItemSize(rowRect.GetSize(), 0.0f);
				ImGui::ItemAdd(rowRect, 0);
			}
		}

		ImGui::SetScrollY(m_Scroll.y);
		ImGui::EndChild();
	}
}

void TextEditor::drawTextContents()
{
	ImGui::BeginChild("TextBufferContents", 
		ImGui::GetContentRegionAvail(), 
		ImGuiChildFlags_::ImGuiChildFlags_Borders, 
		ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoNavInputs);
	{
		// draw background
		ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Background);
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImGui::GetCursorScreenPos(),
			ImGui::GetCursorScreenPos() + ImVec2((float)INT_MAX, (float)INT_MAX),
			IM_COL32(ImGui::GetStyle().Colors[ImGuiCol_FrameBg].x * 255.f,
						ImGui::GetStyle().Colors[ImGuiCol_FrameBg].y * 255.f,
						ImGui::GetStyle().Colors[ImGuiCol_FrameBg].z * 255.f,
						ImGui::GetStyle().Colors[ImGuiCol_FrameBg].w * 255.f));

		// get text viewport
		m_TextViewPort = ImRect(
			ImGui::GetCursorScreenPos() + m_Scroll, 
			ImGui::GetCursorScreenPos() + m_Scroll + ImGui::GetContentRegionAvail());

		if(ImGui::IsWindowHovered())
		{
			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftArrow)) 
				moveCursorLeft();
			
			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightArrow)) 
				moveCursorRight();
			
			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_UpArrow)) 
				moveCursorUp();
			
			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_DownArrow)) 
				moveCursorDown();

			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter)) 
				moveNextLine();

			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Backspace)) 
				removeSymbol();

			handleUserInput();
		}

		// process cursor timer
		m_Timer.CurrentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		m_Timer.Elapsed     = m_Timer.CurrentTime - m_Timer.LaunchTime;

		float threshold = 3000;
		m_Timer1.CurrentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		m_Timer1.Elapsed     = m_Timer1.CurrentTime - m_Timer1.LaunchTime;

		if(m_Timer1.Elapsed > threshold) m_Timer1.LaunchTime = m_Timer1.CurrentTime;

		// adjust cursor position
		m_Cursor.Line   = std::max(m_Cursor.Line, 0);
		m_Cursor.Column = std::max(m_Cursor.Column, 0);

		ImGui::GetWindowDrawList()->ChannelsSplit(DrawLayers::Count);
		ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Text);

		// draw text
		ImGuiListClipper clipper;
		clipper.Begin((int)m_Chunks.size());

		while(clipper.Step())
		{	
			for (int lineNumber = clipper.DisplayStart; lineNumber < clipper.DisplayEnd; lineNumber++)
			{
				auto text = Frenchie::Core::String::as_utf8(m_Chunks[lineNumber]);

				m_RowSize = ImVec2(
					std::max(TextEditor::calculate_text_size(text.c_str()).x, ImGui::GetContentRegionAvail().x), 
					ImGui::GetFontSize());

				ImRect rowRect = ImRect(
					ImGui::GetCursorScreenPos(), 
					ImGui::GetCursorScreenPos() + m_RowSize);

				//data.append(m_Chunks[lineNumber]).append(L"\n");

				//Helpers::colorize1(m_Chunks[lineNumber]);

				//--------------------------------------------------------------------------------------------
				// colorify text here
				//--------------------------------------------------------------------------------------------

				std::vector<Colors::RegularExpressionRule> patterns = 
				{
				    Colors::RegularExpressionRule(
						L"alignas|alignof|and|and_eq|asm|auto|bitand|bitor|bool|break|case|catch|char|char8_t|char16_t|char32_t|class|compl|concept|const|consteval|constexpr|constinit|const_cast|continue|co_await|co_return|co_yield|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|not|not_eq|nullptr|operator|or|or_eq|private|protected|public|reflexpr|register|reinterpret_cast|requires|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|synchronized|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq", 
						IM_COL32(255, 0, 0, 255))
				};

				auto colors = Colors::colorize(
					m_Chunks[lineNumber], 
					patterns, 
					TextEditor::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_Text]));

				ImVec2 offset    = ImVec2(0.f, 0.f);

				for(auto&& color : colors)
				{
				    auto result = color.second;

					ImGui::GetWindowDrawList()->AddText(
						rowRect.Min + offset,
						result.Color, 
						Frenchie::Core::String::as_utf8(
							std::wstring(
								&m_Chunks[lineNumber][result.Position], 
								&m_Chunks[lineNumber][result.Position + result.Length])).c_str());

					offset.x += TextEditor::calculate_text_size(Frenchie::Core::String::as_utf8(
							std::wstring(
								&m_Chunks[lineNumber][result.Position], 
								&m_Chunks[lineNumber][result.Position + result.Length])).c_str()).x;
				}

				//--------------------------------------------------------------------------------------------

				ImGui::ItemSize(rowRect.GetSize(), 0.0f);
				ImGui::ItemAdd(rowRect, 0);

				// highlight current symbol and calculate cursor position
				ImVec2 symbolOffset    = ImVec2(0.f, 0.f);
				ImVec2 symbolSize      = ImVec2(0.f, 0.f);
				ImRect symbolRect      = ImRect();
				bool   symbolIsHovered = false;

				for(int positionInLine = 0; positionInLine < (int)m_Chunks[lineNumber].size(); positionInLine++)
				{
					symbolSize = TextEditor::calculate_text_size(Frenchie::Core::String::as_utf8(std::wstring(1, m_Chunks[lineNumber][positionInLine])).c_str());
					symbolRect = ImRect(rowRect.Min + symbolOffset, rowRect.Min + symbolOffset + symbolSize);

					// highlight symbol
					if(symbolRect.Contains(ImGui::GetMousePos()))
					{
						ImGui::GetWindowDrawList()->AddRectFilled(
							symbolRect.Min,
							symbolRect.Max,
							IM_COL32(ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].x * 255.f,
								ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].y * 255.f,
								ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].z * 255.f,
								ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].w * 255.f)
						);
					}

					// update cursor position
					if(ImGui::IsWindowFocused() &&
						m_TextViewPort.Contains(ImGui::GetMousePos()) &&
						symbolRect.Contains(ImGui::GetMousePos()) &&
						(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) || 
						ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right) || 
						ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle)))
					{
						m_Cursor.Line   = lineNumber;
						m_Cursor.Column = positionInLine;
						symbolIsHovered = true;
					}

					if(m_Cursor.Line == lineNumber && m_Cursor.Column == positionInLine) 
						m_Cursor.Position = symbolRect.Min;

					symbolOffset = ImVec2(symbolOffset.x + symbolSize.x, 0.f);
				}

				// move cursor at the very end
				if(m_Cursor.Column >= m_Chunks[lineNumber].size()) 
					m_Cursor.Position = ImVec2(symbolRect.Max.x, symbolRect.Min.y);

				// move cursor at the very start
				if(m_Cursor.Column <= 0) 
					m_Cursor.Position = rowRect.Min;

				// move cursor at the very start
				if(!symbolIsHovered &&
					m_TextViewPort.Contains(ImGui::GetMousePos()) &&
					ImGui::IsWindowHovered() &&
					rowRect.Contains(ImGui::GetMousePos()) &&
					(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) || 
					ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right) || 
					ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle)))
				{
					m_Cursor.Line   = lineNumber;
					m_Cursor.Column = 0;
				}

				// draw cursor
				if(m_Cursor.Line == lineNumber)
				{	
					ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Cursor);

					// animated
					if(m_Timer.Elapsed > 500)
					{
						ImGui::GetWindowDrawList()->AddText(
							m_Cursor.Position - ImVec2(TextEditor::calculate_text_size("|").x, 0.f) * 0.5f, 
							IM_COL32(ImGui::GetStyle().Colors[ImGuiCol_InputTextCursor].x * 255.f,
										ImGui::GetStyle().Colors[ImGuiCol_InputTextCursor].y * 255.f,
										ImGui::GetStyle().Colors[ImGuiCol_InputTextCursor].z * 255.f,
										ImGui::GetStyle().Colors[ImGuiCol_InputTextCursor].w * 255.f),
							"|"
						);

						// relaunch timer
						if(m_Timer.Elapsed > 1000) 
							m_Timer.LaunchTime = m_Timer.CurrentTime;
					}
				}

				// draw text line bounding rectangle
				ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Background);

				if(m_Cursor.Line == lineNumber)
				{
					ImGui::GetWindowDrawList()->AddRectFilled(
						rowRect.Min, 
						rowRect.Max, 
						IM_COL32(ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].x * 255.f,
							ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].y * 255.f,
							ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].z * 255.f,
							ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].w * 255.f)
						);
				}

				if(rowRect.Contains(ImGui::GetMousePos()))
					m_CurrentlyHoveredLine = lineNumber;
			}
		}

		//Helpers::colorize1(data);

		// if(m_Timer1.Elapsed > threshold)
		// {
		// 	if(!Helpers::colorize1(data).empty()) std::cout << "colorize !!! " << m_Timer1.Elapsed << "\n";
		// }

		if(ImGui::IsWindowHovered())
		{
			for(int key = ImGuiKey::ImGuiKey_NamedKey_BEGIN; key < ImGuiKey::ImGuiKey_NamedKey_END; ++key)
			{
				if(!ImGui::IsKeyPressed((ImGuiKey)key)              ||
					(ImGuiKey)key == ImGuiKey::ImGuiKey_MouseLeft   ||
					(ImGuiKey)key == ImGuiKey::ImGuiKey_MouseRight  ||
					(ImGuiKey)key == ImGuiKey::ImGuiKey_MouseMiddle ||
					(ImGuiKey)key == ImGuiKey::ImGuiKey_MouseX1     ||
					(ImGuiKey)key == ImGuiKey::ImGuiKey_MouseX2     ||
					(ImGuiKey)key == ImGuiKey::ImGuiKey_MouseWheelX ||
					(ImGuiKey)key == ImGuiKey::ImGuiKey_MouseWheelY)
				{
					continue;
				}

				// adjust Y scroll
				if(m_Cursor.Line > clipper.DisplayEnd - 1)
					ImGui::SetScrollY(ImGui::GetScrollY() + ImGui::GetFontSize() * 4.f);
				else if(m_Cursor.Line < clipper.DisplayStart + 1)
					ImGui::SetScrollY(ImGui::GetScrollY() - ImGui::GetFontSize() * 4.f);

				// adjust X scroll
				if(!m_TextViewPort.Contains(m_Cursor.Position))
					ImGui::SetScrollX(ImGui::GetScrollX() + m_Cursor.Position.x - m_TextViewPort.Max.x);

				// on enter we move cursor back onto it's position
				if((ImGuiKey)key == ImGuiKey::ImGuiKey_Enter)
					ImGui::SetScrollX(0.f);
			}
		}

		m_Scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());

		ImGui::EndChild();
	}
}

ImVec2 TextEditor::calculate_text_size(const char* _Begin, const char* _End)
{
	return ImGui::GetCurrentContext()->Font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.f, _Begin, _End, NULL);
}

ImVec2 TextEditor::calculate_text_size(const std::wstring& _Input)
{
	return TextEditor::calculate_text_size(Frenchie::Core::String::as_utf8(_Input).c_str());
}

ImU32 TextEditor::calculate_color(const ImVec4& _Vector)
{
	return IM_COL32(_Vector.x * 255.f, _Vector.y * 255.f, _Vector.z * 255.f, _Vector.w * 255.f);
}

#undef PCRE2_CODE_UNIT_WIDTH