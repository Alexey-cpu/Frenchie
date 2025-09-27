#include <FrenchieEditorTextEditor.hpp>

using namespace Frenchie::Editor;

#include <string>
#include <fstream>
#include <iostream>

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
				//for (size_t i = 0; i < 100; i++)
				{
					textBuffer.append("Привет");
				}

				textBuffer.append("\n");
			}

			// // load text
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

	// initialize navigation cursor
	m_NavigationCursor.PositionInLine = 0;
	m_NavigationCursor.LineNumber     = 0;

	// initialize editor cursor
	m_EditorCursor.PositionInLine = 0;
	m_EditorCursor.LineNumber     = 0;

	// initialize navigation cursor timer
	m_EditorCursorTimer.LaunchTime  = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	m_EditorCursorTimer.CurrentTime = 0;
	m_EditorCursorTimer.Elapsed     = 0;

    return true;
}

void TextEditor::frame_update()
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	// timers
	m_EditorCursorTimer.CurrentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	m_EditorCursorTimer.Elapsed     = m_EditorCursorTimer.CurrentTime - m_EditorCursorTimer.LaunchTime;

    ImGui::Begin("TextEditor", &m_Opened);
    {
		ImGui::BeginChild(
			"TextBufferLineNumbers", 
			ImVec2(TextEditor::calculate_text_size(std::to_string(INT_MAX).c_str()).x, ImGui::GetContentRegionAvail().y),
			ImGuiChildFlags_::ImGuiChildFlags_Borders, 
			ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
		{
			ImGui::GetWindowDrawList()->ChannelsSplit(DrawLayers::Count);
			ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Text);

			ImGuiListClipper clipper;
			clipper.Begin((int)m_Chunks.size());

			while(clipper.Step())
			{
				for (int lineNumber = clipper.DisplayStart; lineNumber < clipper.DisplayEnd; lineNumber++)
				{
					m_LineWidth = std::max<float>(
						m_LineWidth, 
						TextEditor::calculate_row_rect(m_Chunks[lineNumber]).GetSize().x);

					// draw text
					ImRect textLineRect = TextEditor::calculate_row_rect(std::to_string(lineNumber).c_str());
					
					ImGui::GetWindowDrawList()->AddText(textLineRect.GetTL(), IM_COL32(0, 255, 0, 255), std::to_string(lineNumber).c_str());
					ImGui::ItemSize(textLineRect.GetSize(), 0.0f);
					ImGui::ItemAdd(textLineRect, 0);
				}
			}

			ImGui::SetScrollY(m_ScrollY);

			ImGui::EndChild();
		}

		// draw text
		ImGui::SameLine();

		ImGui::BeginChild(
			"TextBufferContents", 
			ImGui::GetContentRegionAvail(),
			ImGuiChildFlags_::ImGuiChildFlags_Borders, 
			ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoNavInputs);
		{
			// handle keys
			if(ImGui::IsWindowHovered())
			{
				// move cursors
				if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftArrow))
				{
					if(m_EditorCursor.PositionInLine > 0)
						--m_EditorCursor.PositionInLine;

					m_EditorCursor.isMoving = true;
				}
				else if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightArrow))
				{
					if(m_EditorCursor.PositionInLine < m_Chunks[m_EditorCursor.LineNumber].size()) 
						m_EditorCursor.PositionInLine++;

					m_EditorCursor.isMoving = true;
				}
				else if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_UpArrow))
				{
					if(m_EditorCursor.LineNumber > 0)
					{
						--m_EditorCursor.LineNumber;
						m_EditorCursor.isMoving = true;
					}
				}
				else if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_DownArrow))
				{
					if(m_EditorCursor.LineNumber < m_Chunks.size() - 1)
					{
						++m_EditorCursor.LineNumber;
						m_EditorCursor.isMoving = true;
					}
				}
				else
				{
					m_EditorCursor.isMoving = false;
				}

				if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter))
				{
					Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
						[this]()
						{
							if(m_EditorCursor.PositionInLine < 
								m_Chunks[m_EditorCursor.LineNumber].size())
							{
								std::wstring copy = std::wstring(
									&m_Chunks[m_EditorCursor.LineNumber][m_EditorCursor.PositionInLine], 
									m_Chunks[m_EditorCursor.LineNumber].size() - m_EditorCursor.PositionInLine);

								m_Chunks[m_EditorCursor.LineNumber].erase(m_EditorCursor.PositionInLine, m_Chunks[m_EditorCursor.LineNumber].size());

									// m_EditorCursor.LineNumber + 1, copy

								m_EditorCursor.LineNumber++;
								m_EditorCursor.PositionInLine = 0;
								m_Chunks.insert(m_Chunks.begin() + m_EditorCursor.LineNumber, copy);
							}
							else
							{
								m_EditorCursor.LineNumber++;
								m_EditorCursor.PositionInLine = 0;
								m_Chunks.insert(m_Chunks.begin() + m_EditorCursor.LineNumber, L" ");
							}
						}
					);
				}

				if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Backspace))
				{
					Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
						[this]()
						{
							if(!m_Chunks[m_EditorCursor.LineNumber].empty())
							{
								if(m_EditorCursor.PositionInLine < m_Chunks[m_EditorCursor.LineNumber].size())
									m_Chunks[m_EditorCursor.LineNumber].erase(m_EditorCursor.PositionInLine, 1);
								else 
									m_Chunks[m_EditorCursor.LineNumber].pop_back();

								m_EditorCursor.PositionInLine = std::max(--m_EditorCursor.PositionInLine, 0);
							}
							else
							{
								m_EditorCursor.LineNumber     = std::max(--m_EditorCursor.LineNumber, 0);
								m_EditorCursor.PositionInLine = (int)m_Chunks[m_EditorCursor.LineNumber].size() - 1;
							}
						}
					);
				}

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
					m_Chunks[m_EditorCursor.LineNumber].insert(m_EditorCursor.PositionInLine++, Frenchie::Core::String::as_wide(std::string(utf8, count)));
				};

				if (ImGui::Shortcut(ImGuiKey_Tab, ImGuiInputFlags_Repeat))
				{
					unsigned int c = '\t'; // Insert TAB
					onCharPressed(c);
				}

				if (io.InputQueueCharacters.Size > 0)
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

					// Consume characters
					io.InputQueueCharacters.resize(0);
				}
			}

			m_LineWidth = std::max<float>(m_LineWidth, ImGui::GetContentRegionAvail().x);

			// draw text
			ImGui::GetWindowDrawList()->ChannelsSplit(DrawLayers::Count);
			ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Text);

			ImGuiListClipper clipper;
			clipper.Begin((int)m_Chunks.size());

			while(clipper.Step())
			{
				for (int lineNumber = clipper.DisplayStart; lineNumber < clipper.DisplayEnd; lineNumber++)
				{
					// draw text
					ImRect textLineBoundingRect = TextEditor::calculate_row_rect(m_Chunks[lineNumber].c_str());

					textLineBoundingRect = ImRect(
						textLineBoundingRect.Min, 
						textLineBoundingRect.Min + ImVec2(m_LineWidth, textLineBoundingRect.GetSize().y));
					
					ImGui::GetWindowDrawList()->AddText(
						textLineBoundingRect.GetTL(), 
						IM_COL32(0, 255, 0, 255), 
						Frenchie::Core::String::as_utf8(m_Chunks[lineNumber]).c_str()
					);
					
					ImGui::ItemSize(textLineBoundingRect.GetSize(), 0.0f);
					ImGui::ItemAdd(textLineBoundingRect, 0);

					// highlight current symbol and calculate cursor position
					ImVec2 symbolOffset    = ImVec2(0.f, 0.f);
					ImVec2 cursorPosition  = ImVec2(0.f, 0.f);
					ImVec2 symbolSize      = ImVec2(0.f, 0.f);
					ImRect symbolRect      = ImRect();
					bool   symbolIsHovered = false;

					for (size_t positionInLine = 0; positionInLine < m_Chunks[lineNumber].size(); positionInLine++)
					{
						auto utf8 = Frenchie::Core::String::as_utf8(std::wstring(1, m_Chunks[lineNumber][positionInLine]));

						symbolSize = TextEditor::calculate_text_size(utf8.c_str());
						
						symbolRect = ImRect(textLineBoundingRect.Min + symbolOffset, textLineBoundingRect.Min + symbolOffset + symbolSize);

						// highlight symbol
						if(symbolRect.Contains(ImGui::GetMousePos()))
							ImGui::GetWindowDrawList()->AddRect(symbolRect.Min, symbolRect.Max, IM_COL32(255, 0, 0, 255));

						// setup cursor position
						if(ImGui::IsWindowFocused() && 
							(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle)) && 
							symbolRect.Contains(ImGui::GetMousePos()))
						{
							m_EditorCursor.LineNumber     = (int)lineNumber;
							m_EditorCursor.PositionInLine = (int)positionInLine;
							symbolIsHovered               = true;
						}

						if(m_EditorCursor.LineNumber == lineNumber && m_EditorCursor.PositionInLine == positionInLine) 
							cursorPosition = symbolRect.Min;

						symbolOffset = ImVec2(symbolOffset.x + symbolSize.x, 0.f);
					}

					if(m_EditorCursor.PositionInLine >= m_Chunks[lineNumber].size()) 
						cursorPosition = ImVec2(symbolRect.Max.x, symbolRect.Min.y);

					// draw cursor
					if(m_EditorCursor.LineNumber == lineNumber)
					{	
						ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Cursor);

						// animated
						if(m_EditorCursorTimer.Elapsed > 500 || m_EditorCursor.isMoving)
						{
							ImGui::GetWindowDrawList()->AddText(
								cursorPosition - ImVec2(TextEditor::calculate_text_size("|").x, 0.f) * 0.5f, 
								IM_COL32(255, 0, 0, 255), 
								"|");

							// relaunch timer
							if(m_EditorCursorTimer.Elapsed > 1000) 
								m_EditorCursorTimer.LaunchTime = m_EditorCursorTimer.CurrentTime;
						}
					}

					// draw text line bounding rectangle
					ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Background);

					if(ImGui::IsWindowHovered() && 
						ImRect(textLineBoundingRect.Min, textLineBoundingRect.Max).Contains(ImGui::GetMousePos()))
					{
						ImGui::GetWindowDrawList()->AddRect(textLineBoundingRect.Min, textLineBoundingRect.Max, IM_COL32(255, 255, 255, 255));

						ImGui::GetWindowDrawList()->AddText(textLineBoundingRect.Min, IM_COL32(255, 0, 0, 255), std::to_string(m_Chunks[lineNumber].size()).c_str());

						// reset cursor position
						if(!symbolIsHovered && 
							(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle)))
						{
							m_EditorCursor.LineNumber     = lineNumber;
							m_EditorCursor.PositionInLine = 0;
						}
					}
				}
			}

			m_ScrollY = ImGui::GetScrollY();
			m_ScrollX = ImGui::GetScrollY();

			ImGui::EndChild();
		}
    }

    ImGui::End();
}

bool TextEditor::allows_multiple_instances() const 
{
    return false;
}

void TextEditor::handle_key_events()
{
}

ImRect TextEditor::calculate_row_rect(const char* _Begin, const char* _End)
{
	return ImRect(
		ImGui::GetCursorScreenPos(), 
		ImGui::GetCursorScreenPos() + ImVec2(std::max<float>(ImGui::GetContentRegionAvail().x, calculate_text_size(_Begin, _End).x), ImGui::GetFontSize()));
}

ImRect TextEditor::calculate_row_rect(const std::wstring& _Input)
{
	return ImRect(
		ImGui::GetCursorScreenPos(), 
		ImGui::GetCursorScreenPos() + ImVec2(std::max<float>(ImGui::GetContentRegionAvail().x, calculate_text_size(_Input).x), ImGui::GetFontSize()));
}

ImVec2 TextEditor::calculate_text_size(const char* _Begin, const char* _End)
{
	return ImGui::GetCurrentContext()->Font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.f, _Begin, _End, NULL);
}

ImVec2 TextEditor::calculate_text_size(const std::wstring& _Input)
{
	return TextEditor::calculate_text_size(Frenchie::Core::String::as_utf8(_Input).c_str());
}