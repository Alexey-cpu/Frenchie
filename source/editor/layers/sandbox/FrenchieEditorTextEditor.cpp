#include <FrenchieEditorTextEditor.hpp>

using namespace Frenchie::Editor;

#include <string>

static inline int ImTextCharToUtf8(char* buf, int buf_size, unsigned int c)
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
	if (c >= 0xdc00 && c < 0xe000)
	{
		return 0;
	}
	if (c >= 0xd800 && c < 0xdc00)
	{
		if (buf_size < 4) return 0;
		buf[0] = (char)(0xf0 + (c >> 18));
		buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
		buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[3] = (char)(0x80 + ((c) & 0x3f));
		return 4;
	}
	//else if (c < 0x10000)
	{
		if (buf_size < 3) return 0;
		buf[0] = (char)(0xe0 + (c >> 12));
		buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[2] = (char)(0x80 + ((c) & 0x3f));
		return 3;
	}
}

static void HandleKeyboardInputs()
{
	ImGuiIO& io = ImGui::GetIO();
	auto shift = io.KeyShift;
	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

	if (ImGui::IsWindowFocused())
	{
		if (ImGui::IsWindowHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		//ImGui::CaptureKeyboardFromApp(true);

		io.WantCaptureKeyboard = true;
		io.WantTextInput = true;

		// if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
		// 	Undo();
		// else if (!IsReadOnly() && !ctrl && !shift && alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
		// 	Undo();
		// else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Y)))
		// 	Redo();
		// else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
		// 	MoveUp(1, shift);
		// else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
		// 	MoveDown(1, shift);
		// else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)))
		// 	MoveLeft(1, shift, ctrl);
		// else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)))
		// 	MoveRight(1, shift, ctrl);
		// else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageUp)))
		// 	MoveUp(GetPageSize() - 4, shift);
		// else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageDown)))
		// 	MoveDown(GetPageSize() - 4, shift);
		// else if (!alt && ctrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
		// 	MoveTop(shift);
		// else if (ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
		// 	MoveBottom(shift);
		// else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
		// 	MoveHome(shift);
		// else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
		// 	MoveEnd(shift);
		// else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
		// 	Delete();
		// else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
		// 	Backspace();
		// else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
		// 	mOverwrite ^= true;
		// else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
		// 	Copy();
		// else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)))
		// 	Copy();
		// else if (!IsReadOnly() && !ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
		// 	Paste();
		// else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)))
		// 	Paste();
		// else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
		// 	Cut();
		// else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
		// 	Cut();
		// else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
		// 	SelectAll();
		// else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
		// 	EnterCharacter('\n', false);
		// else if (!IsReadOnly() && !ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)))
		// 	EnterCharacter('\t', shift);

		// if (!io.InputQueueCharacters.empty())
		// {
		// 	for (int i = 0; i < io.InputQueueCharacters.Size; i++)
		// 	{
		// 		auto c = io.InputQueueCharacters[i];
		// 		if (c != 0 && (c == '\n' || c >= 32)) 
        //             std::cout << ImTextCharToUtf8(с, 1) << "\n";
		// 	}
		// 	io.InputQueueCharacters.resize(0);
		// }
	}
}

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

class TextEditorGeometry
{
public:
protected:
	ImRect m_TextBufferRect;
	ImRect m_LineNumbersRect;
};

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

	for (size_t j = 0; j < 100; j++)
	{
		for (size_t i = 0; i < 100; i++)
		{
			m_TextBuffer.append("TextEditor::TextEditor() : Frenchie::Application::Layer(STRINGIFY(TextEditor)){}");
		}

		m_TextBuffer.append("\n");
	}

    return true;
}

void TextEditor::frame_update()
{

    // constants
    const char ENTER = '\n';
    const char TAB   = '\t';

    ImGui::Begin("TextEditor", &m_Opened);
    {
		// setup next window content size
		ImGui::SetNextWindowContentSize(m_TextBufferRect.GetSize());

        ImGui::BeginChild(
			"TextEditorContents", 
			ImVec2(0, 0), 
			ImGuiChildFlags_::ImGuiChildFlags_None, 
			ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar);
        {
            // retrieve io and draw list
            ImGuiIO& io = ImGui::GetIO();
            auto     dl = ImGui::GetWindowDrawList();

            // paddings
            auto textInterLinesPadding = ImGui::GetFontSize();

            // draw cursor
            {
                auto mouse = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();

                dl->AddText(
                    ImGui::GetMousePos(),
                    IM_COL32(255, 0, 0, 255),
                    fmt::format("X:{} Y:{}", mouse.x, mouse.y).c_str()
                );
            }

            // draw text line by line
            ImVec2 inputTextLineOffset = ImVec2(0.f, 0.f);

			for (size_t textBegin = 0, textEnd = 0, lineNumber = 0; textBegin < m_TextBuffer.size(); ++textEnd, textBegin = textEnd)       
			{
				// identify text line
				while (textEnd < m_TextBuffer.size() && m_TextBuffer[textEnd] != ENTER) ++textEnd;

				// draw line number
				{
					dl->AddText(
						m_LineNumbersRect.GetTL() + ImVec2(0.f, inputTextLineOffset.y),
						IM_COL32(0, 255, 0, 255),
						std::to_string(++lineNumber).c_str()
					);
				}

				// draw colorified text line
				{

					dl->AddText(
						m_TextBufferRect.GetTL() + ImVec2(0.f, inputTextLineOffset.y),
						IM_COL32(0, 255, 0, 255),
						&m_TextBuffer[textBegin],
						&m_TextBuffer[textEnd]
					);
				}

				inputTextLineOffset = ImVec2(
					std::max<float>(inputTextLineOffset.x, ImGui::CalcTextSize(&m_TextBuffer[textBegin], &m_TextBuffer[textEnd]).x), 
					inputTextLineOffset.y + textInterLinesPadding);

				// compute input text bounding rectangle
				m_TextBufferRect = ImRect(
					ImGui::GetCursorScreenPos() + ImVec2(m_LineNumbersRect.GetSize().x, 0.f), 
					ImGui::GetCursorScreenPos() + ImVec2(m_LineNumbersRect.GetSize().x, 0.f) + inputTextLineOffset);

				// compute line number rect
				m_LineNumbersRect = ImRect(
					ImGui::GetCursorScreenPos(), 
					ImGui::GetCursorScreenPos() + ImVec2(ImGui::CalcTextSize(std::to_string(INT_MAX).append("\t\t").c_str()).x, inputTextLineOffset.y));
			}

			// draw text bounding rectangle
			{
				dl->AddRect(m_TextBufferRect.Min, m_TextBufferRect.Max, IM_COL32(255, 0, 0, 255));
			}

			// draw line number rect
			{
				dl->AddRect(m_LineNumbersRect.Min, m_LineNumbersRect.Max, IM_COL32(0, 255, 0, 255));
			}
        }

        ImGui::EndChild();
    }
    ImGui::End();

	// recompute text
	// auto timeEnd = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	// auto elapsed = timeEnd - mStartTime;

	// if(elapsed > 2000 && elapsed > 3000)
	// {
	// 	m_TextBuffer.append(R"(#include <iostream>
	// 	int main(int, int){})");
	// }
}

bool TextEditor::TextEditor::allows_multiple_instances() const 
{
    return false;
}