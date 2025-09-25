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

	// // 	setup text buffer
	// for (size_t j = 0; j < 100; j++)
	// {
	// 	for (size_t i = 0; i < 100; i++)
	// 	{
	// 		m_TextBuffer.append("TextEditor::TextEditor() : Frenchie::Application::Layer(STRINGIFY(TextEditor)){}");
	// 	}

	// 	m_TextBuffer.append("\n");
	// }

	// load text
	std::ifstream ifsream(std::filesystem::path(L"C:/SDK/Qt_Projects/PowerCAD/tests/models/rastrWin3/computable/Центр/Центр неопознанное/1_Летний минимум_2027_ГОСТ_п.5.3_Г.pwrct"));

	m_TextBuffer = 
		std::string(
			(std::istreambuf_iterator<char>(ifsream)), 
			(std::istreambuf_iterator<char>()));

	// fill buffer
	for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < m_TextBuffer.size(); textBegin = ++textEnd, ++lineNumber)
	{
		while(textEnd < m_TextBuffer.size() && m_TextBuffer[textEnd] != ENTER) ++textEnd;

		m_Chunks.push_back(std::string().append(&m_TextBuffer[textBegin], &m_TextBuffer[textEnd]));
	}

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
    ImGui::Begin("TextEditor", &m_Opened);
    {
		float maximumTextWidth = 0.f;

		ImGui::BeginChild(
			"LineNumbers",
			ImVec2(TextEditor::calculate_text_size(std::to_string(INT_MAX).c_str()).x, ImGui::GetContentRegionAvail().y), 
			ImGuiChildFlags_::ImGuiChildFlags_Borders, 
			ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
		{
			ImGui::GetWindowDrawList()->ChannelsSetCurrent(DrawLayers::Text);
			
			ImGuiListClipper clipper;
			clipper.Begin((int)m_Chunks.size());

			while(clipper.Step())
			{
				for(int n = clipper.DisplayStart; n < clipper.DisplayEnd; n++)
				{
					ImRect boundingRect = TextEditor::calculate_row_rect(m_Chunks[n].c_str());
					ImGui::GetWindowDrawList()->AddText(boundingRect.GetTL(), IM_COL32(0, 255, 0, 255), std::to_string(n).c_str());
					ImGui::ItemSize(boundingRect.GetSize(), 0.0f);
					ImGui::ItemAdd(boundingRect, 0);

					maximumTextWidth = std::max<float>(maximumTextWidth, TextEditor::calculate_text_size(m_Chunks[n].c_str()).x);
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
			ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar);
		{
			ImGuiListClipper clipper;
			clipper.Begin((int)m_Chunks.size());

			while(clipper.Step())
			{
				for (int n = clipper.DisplayStart; n < clipper.DisplayEnd; n++)
				{
					// draw text
					ImRect boundingRect = TextEditor::calculate_row_rect(m_Chunks[n].c_str());
					ImGui::GetWindowDrawList()->AddText(boundingRect.GetTL(), IM_COL32(0, 255, 0, 255), m_Chunks[n].c_str());
					ImGui::ItemSize(boundingRect.GetSize(), 0.0f);
					ImGui::ItemAdd(boundingRect, 0);

					// draw text line bounding rectangle
					if(ImGui::IsWindowHovered() && ImRect(boundingRect.Min, boundingRect.Max).Contains(ImGui::GetMousePos()))
					{
						ImGui::GetWindowDrawList()->AddRect(boundingRect.Min, boundingRect.Max, IM_COL32(255, 255, 255, 255));

						ImVec2 symbolOffset = ImVec2(0.f, 0.f);

						for (size_t position = 0; position < m_Chunks[n].size(); position++)
						{
							ImVec2 symbolSize = TextEditor::calculate_text_size(&m_Chunks[n].c_str()[position], &m_Chunks[n].c_str()[position + 1]);
							ImRect symbolRect = ImRect(boundingRect.Min + symbolOffset, boundingRect.Min + symbolOffset + symbolSize);

							if(symbolRect.Contains(ImGui::GetMousePos()))
							{
								ImGui::GetWindowDrawList()->AddRect(symbolRect.Min, symbolRect.Max, IM_COL32(255, 0, 0, 255));
							}

							symbolOffset = ImVec2(symbolOffset.x + symbolSize.x, 0.f);
						}
					}

				}
			}

			m_ScrollY = ImGui::GetScrollY();

			ImGui::EndChild();
		}

		// draw text rows
		//ImGui::SameLine();

		// ImGui::BeginChild(
		// 	"TextBufferContents", 
		// 	ImGui::GetContentRegionAvail(), 
		// 	ImGuiChildFlags_::ImGuiChildFlags_Borders, 
		// 	ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar);
		// {
		// 	for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < m_TextBuffer.size(); textBegin = ++textEnd, ++lineNumber)
		// 	{
		// 		while(textEnd < m_TextBuffer.size() && m_TextBuffer[textEnd] != ENTER) ++textEnd;

		// 		ImRect textLineRect = calculate_row_rect(lineNumber);

		// 		ImGui::GetWindowDrawList()->AddText(
		// 			textLineRect.GetTL(), IM_COL32(0, 255, 0, 255), &m_TextBuffer[textBegin], &m_TextBuffer[textEnd]);

		// 		// draw text line bounding rectangle
		// 		if(ImGui::IsWindowHovered() && ImRect(textLineRect.Min, textLineRect.Max).Contains(ImGui::GetMousePos()))
		// 		{
		// 			ImGui::GetWindowDrawList()->AddRect(textLineRect.Min, textLineRect.Max, IM_COL32(255, 255, 255, 255));

		// 			ImVec2 symbolOffset = ImVec2(0.f, 0.f);
		// 			Cursor symbolCursor = Cursor();

		// 			for(size_t position = textBegin; position < textEnd; position++)
		// 			{
		// 				ImVec2 symbolSize = TextEditor::calculate_text_size(&m_TextBuffer[position], &m_TextBuffer[position + 1]);
		// 				ImRect symbolRect = ImRect(textLineRect.Min + symbolOffset, textLineRect.Min + symbolOffset + symbolSize);

		// 				// update navigation cursor
		// 				if(symbolRect.Contains(ImGui::GetMousePos()))
		// 				{
		// 					// update navigation cursor
		// 					ImGui::GetWindowDrawList()->AddRect(symbolRect.Min, symbolRect.Max, IM_COL32(255, 0, 0, 255));

		// 					m_NavigationCursor.LineNumber       = lineNumber;
		// 					m_NavigationCursor.PositionInBuffer = position;
		// 					m_NavigationCursor.PositionInView   = symbolRect.Min - ImVec2(TextEditor::calculate_text_size("|").x, 0.f) * 0.5f;
		// 				}

		// 				symbolOffset = ImVec2(symbolOffset.x + symbolSize.x, 0.f);
		// 			}
		// 		}
		// 	}

		// 	m_ScrollY = ImGui::GetScrollY();
		// 	m_ScrollX = ImGui::GetScrollX();

		// 	ImGui::EndChild();
		// }
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

ImRect TextEditor::calculate_row_rect(const char* _Begin, const char* _End)
{
	return ImRect(
		ImGui::GetCursorScreenPos(), 
		ImGui::GetCursorScreenPos() + ImVec2(std::max<float>(ImGui::GetContentRegionAvail().x, calculate_text_size(_Begin, _End).x) + ImGui::GetFontSize() * 4.f, ImGui::GetFontSize()));
}

ImVec2 TextEditor::calculate_text_size(const char* _Begin, const char* _End)
{
	return ImGui::GetCurrentContext()->Font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.f, _Begin, _End, NULL);
}