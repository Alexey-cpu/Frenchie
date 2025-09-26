#include <FrenchieEditorTextEditor.hpp>

using namespace Frenchie::Editor;

#include <string>
#include <fstream>
#include <iostream>

// static inline int ImTextCharToUtf8(char* buf, int buf_size, unsigned int c)
// {
// 	if (c < 0x80)
// 	{
// 		buf[0] = (char)c;
// 		return 1;
// 	}
// 	if (c < 0x800)
// 	{
// 		if (buf_size < 2) return 0;
// 		buf[0] = (char)(0xc0 + (c >> 6));
// 		buf[1] = (char)(0x80 + (c & 0x3f));
// 		return 2;
// 	}
// 	if (c >= 0xdc00 && c < 0xe000)
// 	{
// 		return 0;
// 	}
// 	if (c >= 0xd800 && c < 0xdc00)
// 	{
// 		if (buf_size < 4) return 0;
// 		buf[0] = (char)(0xf0 + (c >> 18));
// 		buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
// 		buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
// 		buf[3] = (char)(0x80 + ((c) & 0x3f));
// 		return 4;
// 	}
// 	//else if (c < 0x10000)
// 	{
// 		if (buf_size < 3) return 0;
// 		buf[0] = (char)(0xe0 + (c >> 12));
// 		buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
// 		buf[2] = (char)(0x80 + ((c) & 0x3f));
// 		return 3;
// 	}
// }

// static void HandleKeyboardInputs()
// {
// 	ImGuiIO& io = ImGui::GetIO();
// 	auto shift = io.KeyShift;
// 	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
// 	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

// 	if (ImGui::IsWindowFocused())
// 	{
// 		if (ImGui::IsWindowHovered())
// 			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
// 		//ImGui::CaptureKeyboardFromApp(true);

// 		io.WantCaptureKeyboard = true;
// 		io.WantTextInput = true;

// 		// if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
// 		// 	Undo();
// 		// else if (!IsReadOnly() && !ctrl && !shift && alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
// 		// 	Undo();
// 		// else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Y)))
// 		// 	Redo();
// 		// else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
// 		// 	MoveUp(1, shift);
// 		// else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
// 		// 	MoveDown(1, shift);
// 		// else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)))
// 		// 	MoveLeft(1, shift, ctrl);
// 		// else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)))
// 		// 	MoveRight(1, shift, ctrl);
// 		// else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageUp)))
// 		// 	MoveUp(GetPageSize() - 4, shift);
// 		// else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageDown)))
// 		// 	MoveDown(GetPageSize() - 4, shift);
// 		// else if (!alt && ctrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
// 		// 	MoveTop(shift);
// 		// else if (ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
// 		// 	MoveBottom(shift);
// 		// else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
// 		// 	MoveHome(shift);
// 		// else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
// 		// 	MoveEnd(shift);
// 		// else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
// 		// 	Delete();
// 		// else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
// 		// 	Backspace();
// 		// else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
// 		// 	mOverwrite ^= true;
// 		// else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
// 		// 	Copy();
// 		// else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)))
// 		// 	Copy();
// 		// else if (!IsReadOnly() && !ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
// 		// 	Paste();
// 		// else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)))
// 		// 	Paste();
// 		// else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
// 		// 	Cut();
// 		// else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
// 		// 	Cut();
// 		// else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
// 		// 	SelectAll();
// 		// else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
// 		// 	EnterCharacter('\n', false);
// 		// else if (!IsReadOnly() && !ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)))
// 		// 	EnterCharacter('\t', shift);

// 		// if (!io.InputQueueCharacters.empty())
// 		// {
// 		// 	for (int i = 0; i < io.InputQueueCharacters.Size; i++)
// 		// 	{
// 		// 		auto c = io.InputQueueCharacters[i];
// 		// 		if (c != 0 && (c == '\n' || c >= 32)) 
//         //             std::cout << ImTextCharToUtf8(с, 1) << "\n";
// 		// 	}
// 		// 	io.InputQueueCharacters.resize(0);
// 		// }
// 	}
// }

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
			// //	setup text buffer
			// std::string textBuffer;

			// for (size_t j = 0; j < 100; j++)
			// {
			// 	//for (size_t i = 0; i < 100; i++)
			// 	{
			// 		textBuffer.append("TextEditor::TextEditor() : Frenchie::Application::Layer(STRINGIFY(TextEditor)){}");
			// 	}

			// 	textBuffer.append("\n");
			// }

			// load text
			std::ifstream ifsream(std::filesystem::path(L"C:/SDK/Qt_Projects/PowerCAD/tests/models/rastrWin3/computable/Центр/Центр неопознанное/1_Летний минимум_2027_ГОСТ_п.5.3_Г.pwrct"));

			std::string textBuffer = 
				std::string(
					(std::istreambuf_iterator<char>(ifsream)), 
					(std::istreambuf_iterator<char>()));
		
			std::vector<std::string> chunks;

			for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < textBuffer.size(); textBegin = ++textEnd, ++lineNumber)
			{
				while(textEnd < textBuffer.size() && textBuffer[textEnd] != ENTER) ++textEnd;

				m_Chunks.push_back(std::string().append(&textBuffer[textBegin], &textBuffer[textEnd]));
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

	// add an empty string to empty chunk
	// if(m_Chunks.empty())
	// 	m_Chunks.push_back(std::string(" "));

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
					m_LineWidth = std::max<float>(m_LineWidth, TextEditor::calculate_row_rect(m_Chunks[lineNumber].c_str()).GetSize().x);

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
			// move text edit cursor
			if(ImGui::IsWindowHovered())
			{
				if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftArrow))
				{
					if(m_EditorCursor.PositionInLine > 0)
						--m_EditorCursor.PositionInLine;

					m_EditorCursor.isMoving = true;
				}
				else if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightArrow))
				{
					if(m_EditorCursor.PositionInLine < m_Chunks[m_EditorCursor.LineNumber].size()) 
					{
						m_EditorCursor.PositionInLine = 
							std::min<size_t(m_EditorCursor.PositionInLine + 1, m_Chunks[m_EditorCursor.LineNumber].size() - 1);
					}

					m_EditorCursor.isMoving = true;
				}
				else if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_UpArrow))
				{
					if(m_EditorCursor.LineNumber > 0)
					{
						// move up
						--m_EditorCursor.LineNumber;

						// adjust postion
						if(m_EditorCursor.PositionInLine >= m_Chunks[m_EditorCursor.LineNumber].size())
							m_EditorCursor.PositionInLine = 0;

						m_EditorCursor.isMoving = true;
					}
				}
				else if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_DownArrow))
				{
					if(m_EditorCursor.LineNumber < m_Chunks.size() - 1)
					{
						// move down
						++m_EditorCursor.LineNumber;

						// adjust postion
						if(m_EditorCursor.PositionInLine >= m_Chunks[m_EditorCursor.LineNumber].size())
							m_EditorCursor.PositionInLine = 0;

						m_EditorCursor.isMoving = true;
					}
				}
				else
				{
					m_EditorCursor.isMoving = false;
				}
			}

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
					
					ImGui::GetWindowDrawList()->AddText(textLineBoundingRect.GetTL(), IM_COL32(0, 255, 0, 255), m_Chunks[lineNumber].c_str());
					ImGui::ItemSize(textLineBoundingRect.GetSize(), 0.0f);
					ImGui::ItemAdd(textLineBoundingRect, 0);

					// setup and draw cursor
					ImVec2 symbolOffset    = ImVec2(0.f, 0.f);
					bool   symbolIsHovered = false;

					for (size_t positionInLine = 0; positionInLine < m_Chunks[lineNumber].size(); positionInLine++)
					{
						ImVec2 symbolSize = TextEditor::calculate_text_size(&m_Chunks[lineNumber].c_str()[positionInLine], &m_Chunks[lineNumber].c_str()[positionInLine + 1]);
						ImRect symbolRect = ImRect(textLineBoundingRect.Min + symbolOffset, textLineBoundingRect.Min + symbolOffset + symbolSize);

						// highlight symbol
						if(symbolRect.Contains(ImGui::GetMousePos()))
							ImGui::GetWindowDrawList()->AddRect(symbolRect.Min, symbolRect.Max, IM_COL32(255, 0, 0, 255));

						// setup cursor position
						if(ImGui::IsWindowFocused() && 
							(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle)) && 
							symbolRect.Contains(ImGui::GetMousePos()))
						{
							m_EditorCursor.LineNumber     = lineNumber;
							m_EditorCursor.PositionInLine = positionInLine;
							symbolIsHovered               = true;
						}

						// draw cursor
						ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Cursor);

						if(m_EditorCursor.LineNumber == lineNumber && 
							m_EditorCursor.PositionInLine == positionInLine)
						{	
							// animated
							if(m_EditorCursorTimer.Elapsed > 500 || m_EditorCursor.isMoving)
							{
								ImGui::GetWindowDrawList()->AddText(
									symbolRect.Min - ImVec2(TextEditor::calculate_text_size("|").x, 0.f) * 0.5f, 
									IM_COL32(255, 0, 0, 255), 
									"|");

								// relaunch timer
								if(m_EditorCursorTimer.Elapsed > 1000) 
									m_EditorCursorTimer.LaunchTime = m_EditorCursorTimer.CurrentTime;
							}
						}

						symbolOffset = ImVec2(symbolOffset.x + symbolSize.x, 0.f);
					}

					// draw text line bounding rectangle
					ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Background);

					if(ImGui::IsWindowHovered() && 
						ImRect(textLineBoundingRect.Min, textLineBoundingRect.Max).Contains(ImGui::GetMousePos()))
					{
						ImGui::GetWindowDrawList()->AddRect(textLineBoundingRect.Min, textLineBoundingRect.Max, IM_COL32(255, 255, 255, 255));

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

ImVec2 TextEditor::calculate_text_size(const char* _Begin, const char* _End)
{
	return ImGui::GetCurrentContext()->Font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.f, _Begin, _End, NULL);
}