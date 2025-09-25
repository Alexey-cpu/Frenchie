#include <FrenchieEditorTextEditor.hpp>

using namespace Frenchie::Editor;

#include <string>

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
	Text,
	Cursor,
	Lines,
	Count
};

const char ENTER = '\n';
const char TAB   = '\t';

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

	//m_TextBuffer.append(" ");

	// 	setup text buffer
	// for (size_t j = 0; j < 100; j++)
	// {
	// 	for (size_t i = 0; i < 100; i++)
	// 	{
	// 		m_TextBuffer.append("TextEditor::TextEditor() : Frenchie::Application::Layer(STRINGIFY(TextEditor)){}");
	// 	}

	// 	m_TextBuffer.append("\n");
	// }

	// initialize navigation cursor
	m_NavigationCursor.PositionInBuffer = 0;
	m_NavigationCursor.LineNumber       = 0;
	m_NavigationCursor.PositionInView   = ImVec2(0.f, 0.f);

	// initialize editor cursor
	m_EditorCursor.PositionInBuffer = 0;
	m_EditorCursor.LineNumber       = 0;
	m_EditorCursor.PositionInView   = ImVec2(0.f, 0.f);

	// initialize navigation cursor timer
	m_EditorCursorTimer.LaunchTime  = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	m_EditorCursorTimer.CurrentTime = 0;
	m_EditorCursorTimer.Elapsed     = 0;

    return true;
}

void TextEditor::frame_update()
{
	// append on empty buffer
	if(m_TextBuffer.empty()) 
		m_TextBuffer.append(" ");

	// update carriage attributes
	ImVec2 lineNumberOffset = ImVec2(TextEditor::calculate_text_size(std::to_string(INT_MAX).c_str()).x, 0.f);
	ImVec2 nextLineOffset   = ImVec2(0.f, ImGui::GetFontSize());
	bool   mouseClicked     = ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)  || 
							  ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right) || 
							  ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle);

    ImGui::Begin("TextEditor", &m_Opened);
    {
		ImGui::SetNextWindowContentSize(TextEditor::calculate_text_size(m_TextBuffer.c_str()) + lineNumberOffset);

        ImGui::BeginChild("TextEditorContents", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.8f), ImGuiChildFlags_::ImGuiChildFlags_Borders, ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar);
        {
			// split draw list on channels
			ImGui::GetWindowDrawList()->ChannelsSplit(DrawLayers::Count);

			// draw cursor
			draw_navigation_cursor();

            // draw text line by line
			ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Text);

			// setup carriage
			for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < m_TextBuffer.size(); ++textEnd, textBegin = textEnd)
			{
				// identify text line buffer borders
				while(textEnd < m_TextBuffer.size() && m_TextBuffer[textEnd] != ENTER) ++textEnd;

				// draw text line number
				ImGui::GetWindowDrawList()->AddText(calculate_row_rect(lineNumber, nextLineOffset).GetTL(), IM_COL32(0, 255, 0, 255), std::to_string(++lineNumber).c_str());

				// draw text line buffer
				ImRect textLineRect = calculate_row_rect(lineNumber, nextLineOffset, lineNumberOffset);
				ImGui::GetWindowDrawList()->AddText(textLineRect.GetTL(), IM_COL32(0, 255, 0, 255), &m_TextBuffer[textBegin], &m_TextBuffer[textEnd]);

				// draw text line bounding rectangle
				if(ImGui::IsWindowHovered() && 
					ImRect(textLineRect.Min, textLineRect.Max).Contains(ImGui::GetMousePos()))
				{
					ImGui::GetWindowDrawList()->AddRect(textLineRect.Min, textLineRect.Max, IM_COL32(255, 255, 255, 255));

					ImVec2 symbolOffset = ImVec2(0.f, 0.f);
					Cursor symbolCursor = Cursor();

					for(size_t position = textBegin; position < textEnd; position++)
					{
						ImVec2 symbolSize = TextEditor::calculate_text_size(&m_TextBuffer[position], &m_TextBuffer[position + 1]);
						ImRect symbolRect = ImRect(textLineRect.Min + symbolOffset, textLineRect.Min + symbolOffset + symbolSize);

						// update editor cursor when the mosue is clicked
						if(mouseClicked && 
							(position == textBegin || symbolRect.Contains(ImGui::GetMousePos()))) 
						{
							m_EditorCursor.LineNumber       = lineNumber;
							m_EditorCursor.PositionInBuffer = position;
							m_EditorCursor.PositionInView   = symbolRect.Min - ImVec2(TextEditor::calculate_text_size("|").x, 0.f) * 0.5f;
						}

						// update navigation cursor
						if(symbolRect.Contains(ImGui::GetMousePos()))
						{
							// update navigation cursor
							ImGui::GetWindowDrawList()->AddRect(symbolRect.Min, symbolRect.Max, IM_COL32(255, 0, 0, 255));

							m_NavigationCursor.LineNumber       = lineNumber;
							m_NavigationCursor.PositionInBuffer = position;
							m_NavigationCursor.PositionInView   = symbolRect.Min - ImVec2(TextEditor::calculate_text_size("|").x, 0.f) * 0.5f;
						}

						symbolOffset = ImVec2(symbolOffset.x + symbolSize.x, 0.f);
					}
				}

				// update status
				m_Status.TextBufferLinesCount   = lineNumber;
				m_Status.TextBufferLength       = m_TextBuffer.size();
				m_Status.CursorPositionInBuffer = m_NavigationCursor.PositionInBuffer;
				m_Status.CursorPositionInView   = m_NavigationCursor.PositionInView;
				m_Status.CursorLineNumber       = m_NavigationCursor.LineNumber;
			}

			ImGui::EndChild();

			// draw line number rect
			ImGui::GetWindowDrawList()->AddRect(
				ImGui::GetItemRectMin() + lineNumberOffset,
				ImGui::GetItemRectMax(), 
				IM_COL32(255, 0, 0, 255));

			ImGui::GetWindowDrawList()->AddRect(
				ImGui::GetItemRectMin(),
				ImGui::GetItemRectMin() + lineNumberOffset, 
				IM_COL32(0, 255, 0, 255));
        }

		ImGui::BeginChild(
			"TextEditorStatusBar", 
			ImGui::GetContentRegionAvail(), 
			ImGuiChildFlags_::ImGuiChildFlags_Borders, 
			ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar
		);
		{
			ImGui::TextUnformatted(fmt::format(
				"Buffer length: {} | Text lines count: {} | Current line number: {} | Current position in buffer: {} | Current position X:{} Y:{}", 
				m_Status.TextBufferLength, 
				m_Status.TextBufferLinesCount, 
				m_Status.CursorLineNumber, 
				m_Status.CursorPositionInBuffer, 
				m_Status.CursorPositionInView.x, 
				m_Status.CursorPositionInView.y).c_str());

			ImGui::EndChild();
		}
    }
    ImGui::End();
}

bool TextEditor::allows_multiple_instances() const 
{
    return false;
}

void TextEditor::draw_navigation_cursor()
{
	ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Cursor);

	// update timer
	m_EditorCursorTimer.CurrentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	m_EditorCursorTimer.Elapsed     = m_EditorCursorTimer.CurrentTime - m_EditorCursorTimer.LaunchTime;

	if(m_EditorCursorTimer.Elapsed > 500)
	{
		// draw cursor
		ImGui::GetWindowDrawList()->AddText(m_EditorCursor.PositionInView, IM_COL32(255, 0, 0, 255), "|");

		// relaunch timer
		if(m_EditorCursorTimer.Elapsed > 1000) 
			m_EditorCursorTimer.LaunchTime = m_EditorCursorTimer.CurrentTime;
	}
}

ImRect TextEditor::calculate_row_rect(const size_t& _Row, const ImVec2& _NextLineOffset, const ImVec2& _LineNumberOffset)
{
	auto sourcePoint = ImGui::GetCursorScreenPos() + _LineNumberOffset;

	return ImRect(sourcePoint + _NextLineOffset * (float)(_Row > 0 ? _Row - 1 : _Row), 
					sourcePoint + _NextLineOffset * (float)_Row + ImVec2(ImGui::GetContentRegionAvail().x, 0.f));
}

ImVec2 TextEditor::calculate_text_size(const char* _Begin, const char* _End)
{
	return ImGui::GetCurrentContext()->Font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.f, _Begin, _End, NULL);
}