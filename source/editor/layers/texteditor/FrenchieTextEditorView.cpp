#include <FrenchieTextEditorView.hpp>

#include <FrenchieCoreThreadPool.hpp>

#include <FrenchieCoreSerializationFormatJSON.hpp>

using namespace Frenchie::Editor;

#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include <map>

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

// view
TextEditor::TextEditor() : Frenchie::Application::Layer(STRINGIFY(TextEditor)){}
TextEditor::~TextEditor()
{
}

bool TextEditor::awake()
{
	// launch timers
	m_CursorTimer = Frenchie::Application::application()
		->push_layer<Frenchie::Application::SynchronousTimer<std::chrono::milliseconds>>(
			1000.0, true, "TextEditorCursorTimer");

	// Frenchie::Core::Serialization::Document document;
	// auto items = document.append_node("patterns", "", Frenchie::Core::Serialization::NodeType::ARRAY);

	// for(auto&& pattern : m_Patterns)
	// {
	// 	auto item = items.append_node("", "", Frenchie::Core::Serialization::NodeType::OBJECT);

	// 	item.append_node("match", Frenchie::Core::String::as_utf8(pattern.Pattern).c_str(), Frenchie::Core::Serialization::NodeType::STRING);
		
	// 	switch (pattern.Type)
	// 	{
	// 	case RegexRule::Type::DEFAULT:
	// 		item.append_node("type", "DEFAULT", Frenchie::Core::Serialization::NodeType::STRING);
	// 		break;
		
	// 	case RegexRule::Type::MULTILINE_START:
	// 		item.append_node("type", "MULTILINE_START", Frenchie::Core::Serialization::NodeType::STRING);
	// 		break;

	// 	case RegexRule::Type::MULTILINE_FINISH:
	// 		item.append_node("type", "MULTILINE_FINISH", Frenchie::Core::Serialization::NodeType::STRING);
	// 		break;

	// 	default:
	// 		item.append_node("type", "DEFAULT", Frenchie::Core::Serialization::NodeType::STRING);
	// 		break;
	// 	}
		
	// 	item.append_node("color", fmt::format("0x{:x}", (unsigned int)pattern.Color).c_str(), Frenchie::Core::Serialization::NodeType::STRING);
	// }

	// document.write<Frenchie::Core::Serialization::JSONBeautifulWriter>(
	// 	"C:/SDK/Qt_Projects/OpenGL/shared/cpp.json"
	// );

	//m_TextModel->set_dirty(false);

	//	setup text buffer
	std::string textBuffer;

	for (size_t j = 0; j < 1e6; j++)
	{
		for (size_t i = 0; i < 10; i++)
			textBuffer.append("for(int i = 0; i < 10; i++)");

		textBuffer.append("\n");
	}

	m_TextModel->append(textBuffer);

    return true;
}

void TextEditor::frame_update()
{
    ImGui::Begin("TextEditor", &m_Opened);
    {
		ImGui::BeginChild("TextEditorContents", 
			ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.9f),
			ImGuiChildFlags_::ImGuiChildFlags_Borders, 
			ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
		{
			draw_text_line_numbers();
			
			ImGui::SameLine();
			
			draw_text_contents();
		}

		ImGui::EndChild();

		draw_status_panel();

		ImGui::End();
    }
}

bool TextEditor::allows_multiple_instances() const 
{
    return false;
}

void TextEditor::finish()
{
	std::cout << "TextEditor::finish() \n";
}

void TextEditor::move_cursor_left_command()
{
	Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
		[this]()
		{
			if(m_TextModel != nullptr)
				m_TextModel->move_cursor_left();
		}
	);
}

void TextEditor::move_cursor_right_command()
{
	Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
		[this]()
		{
			if(m_TextModel != nullptr)
				m_TextModel->move_cursor_right();
		}
	);
}

void TextEditor::move_cursor_down_command()
{
	Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
		[this]()
		{
			if(m_TextModel != nullptr)
				m_TextModel->move_cursor_down();
		}
	);
}

void TextEditor::move_cursor_up_command()
{
	Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
		[this]()
		{
			if(m_TextModel != nullptr)
				m_TextModel->move_cursor_up();
		}
	);
}

void TextEditor::move_next_line_command()
{
	Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
		[this]()
		{
			if(m_TextModel != nullptr)
				m_TextModel->move_next_line();
		}
	);
}

void TextEditor::move_back_commnad()
{
	Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
		[this]()
		{
			if(m_TextModel != nullptr)
				m_TextModel->move_back();
		}
	);
}

void TextEditor::insert_symbol_command()
{
	ImGuiIO& io = ImGui::GetIO();
	
	auto onCharPressed = [this](unsigned int c)
	{
		if(m_TextModel == nullptr) 
			return;

		// insert user input into a given line as wide character string
		Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
			[this, c]()
			{
				// retrieve user input in UTF-8 codec
				char utf8[5];
				int  count = Helpers::ImTextCharToUtf8(utf8, c);

				m_TextModel->insert(m_TextModel->get_cursros_line(), m_TextModel->get_cursros_column(), Frenchie::Core::String::as_wide(std::string(utf8, count)));
				m_TextModel->move_cursor_right();
			}
		);
	};

	if (ImGui::Shortcut(ImGuiKey_Tab, ImGuiInputFlags_Repeat))
	{
		unsigned int c = '\t'; // Insert TAB
		onCharPressed(c);
	}
	else if (io.InputQueueCharacters.Size > 0)
	{
		if (!((io.KeyCtrl && !io.KeyAlt) || (io.ConfigMacOSXBehaviors && io.KeyCtrl)))
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

		// consume user input
		io.InputQueueCharacters.resize(0);
	}
}

void TextEditor::clear_selection_command()
{
	Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
		[this]()
		{
			if(m_TextModel != nullptr)
				m_TextModel->clear_selection();
		}
	);
}

void TextEditor::copy_command()
{
	if(m_TextModel != nullptr)
		ImGui::SetClipboardText(Frenchie::Core::String::as_utf8(m_TextModel->get_selection()).c_str());
}

void TextEditor::paste_command()
{
	if(m_TextModel == nullptr) return;

	std::string clipboard = ImGui::GetClipboardText();

	m_TextModel->insert(m_TextModel->get_cursros_line(), m_TextModel->get_cursros_column(), clipboard);
}

void TextEditor::draw_text_line_numbers()
{
	ImGui::BeginChild("TextBufferLineNumbers", 
		ImVec2(TextEditor::calculate_text_size(std::to_string(INT_MAX).c_str()).x, ImGui::GetContentRegionAvail().y), 
		ImGuiChildFlags_::ImGuiChildFlags_Borders, 
		ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoInputs);
	
	// get ready to draw
	ImGui::GetWindowDrawList()->ChannelsSplit(Layers::COUNT);

	// draw background
	ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::BACKGROUND);

	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetCursorScreenPos() - ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()), 
		ImGui::GetCursorScreenPos() + ImVec2((float)INT_MAX, (float)INT_MAX), 
		TextEditor::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_FrameBg]));

	if(m_TextModel != nullptr)
	{
		ImGuiListClipper clipper;
		clipper.Begin(m_TextModel->get_text_lines_count(), ImGui::GetFontSize());

		while(clipper.Step())
		{
			for (int lineNumber = clipper.DisplayStart; lineNumber < clipper.DisplayEnd; lineNumber++)
			{
				ImRect rowRect = ImRect(
					ImGui::GetCursorScreenPos(), 
					ImGui::GetCursorScreenPos() + ImVec2(ImGui::GetContentRegionAvail().x, 
					ImGui::GetFontSize()));

				ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::BACKGROUND);

				if(lineNumber % 2 == 0)
				{
					ImGui::GetWindowDrawList()->AddRectFilled(
						rowRect.Min,
						rowRect.Max,
						TextEditor::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_TableRowBg]));
				}
				else 
				{
					ImGui::GetWindowDrawList()->AddRectFilled(
						rowRect.Min,
						rowRect.Max,
						TextEditor::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt]));
				}

				// draw text
				ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::TEXT);
				ImGui::GetWindowDrawList()->AddText(
					rowRect.GetTL(), 
					TextEditor::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_Text]), 
					std::to_string(lineNumber).c_str());
				
				ImGui::ItemSize(rowRect.GetSize(), 0.0f);
				ImGui::ItemAdd(rowRect, 0);
			}
		}

		ImGui::SetScrollY(m_ScrollPos.y);
	}

	ImGui::EndChild();
}

void TextEditor::draw_text_contents()
{
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ScrollbarBg, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);

	ImGui::BeginChild("TextBufferContents", 
		ImGui::GetContentRegionAvail(), 
		ImGuiChildFlags_::ImGuiChildFlags_Borders, 
		ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoNavInputs);
	
	// get ready for drawing
	ImGui::GetWindowDrawList()->ChannelsSplit(Layers::COUNT);

	// draw background
	ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::BACKGROUND);

	ImRect backgroundRect = ImRect(
		ImGui::GetCursorScreenPos() - ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()),
		ImGui::GetCursorScreenPos() + ImVec2((float)INT_MAX, (float)INT_MAX));

	ImGui::GetWindowDrawList()->AddRectFilled(
		backgroundRect.Min,
		backgroundRect.Max,
		TextEditor::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_FrameBg]));

	if(m_TextModel != nullptr)
	{
		// calculate text viewport
		m_TextRect = ImRect(
			ImGui::GetCursorScreenPos() + m_ScrollPos, 
			ImGui::GetCursorScreenPos() + m_ScrollPos + ImGui::GetContentRegionAvail());

		// process commands
		if(ImGui::IsWindowHovered())
		{
			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftArrow)) 
				move_cursor_left_command();
			
			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightArrow)) 
				move_cursor_right_command();
			
			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_UpArrow)) 
				move_cursor_up_command();
			
			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_DownArrow)) 
				move_cursor_down_command();

			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter)) 
				move_next_line_command();

			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Backspace)) 
				move_back_commnad();

			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
				clear_selection_command();

			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C) && 
				(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || 
				ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl)))
				copy_command();

			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_V) && 
				(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || 
				ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl)))
				paste_command();

			insert_symbol_command();
		}

		// draw text
		ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::TEXT);

		ImGuiListClipper clipper;
		clipper.Begin(m_TextModel->get_text_lines_count(), ImGui::GetFontSize());

		while(clipper.Step())
		{
			for (int lineNumber = clipper.DisplayStart; lineNumber < clipper.DisplayEnd; lineNumber++)
			{
				auto text = m_TextModel->get_text_line(lineNumber);
				auto utf8 = Frenchie::Core::String::as_utf8(text);

				// highlight and draw text
				ImRect rowRect = ImRect(
					ImGui::GetCursorScreenPos(), 
					ImGui::GetCursorScreenPos() + 
						ImVec2(std::max(TextEditor::calculate_text_size(utf8.c_str()).x, ImGui::GetContentRegionAvail().x), ImGui::GetFontSize()));

				ImGui::ItemSize(rowRect.GetSize(), 0.0f);
				ImGui::ItemAdd(rowRect, 0);

				if(false)
				{
					ImGui::GetWindowDrawList()->AddText(
						rowRect.Min,
						TextEditor::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_Text]), 
						utf8.c_str());
				}
				else
				{
					SyntaxHighlighter::regexEstimationResults matches = 
						m_SyntaxHighlighter.highlight(
							text, 
							m_Patterns,
							TextEditor::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_Text]), lineNumber);

					ImVec2 offset = ImVec2(0.f, 0.f);

					for(auto&& match : matches)
					{
						std::wstring highlightedText = 
							Frenchie::Core::Regex::substring(text, match.second.Match);

						ImGui::GetWindowDrawList()->AddText(
							rowRect.Min + offset,
							match.second.Color, 
							Frenchie::Core::String::as_utf8(highlightedText).c_str());

						offset.x += TextEditor::calculate_text_size(Frenchie::Core::String::as_utf8(highlightedText).c_str()).x;
					}
				}

				// highlight current symbol and calculate cursor position
				ImVec2 symbolOffset    = ImVec2(0.f, 0.f);
				ImVec2 symbolSize      = ImVec2(0.f, 0.f);
				ImRect symbolRect      = ImRect();
				bool   symbolIsHovered = false;

				for(int positionInLine = 0; positionInLine < (int)text.size(); positionInLine++)
				{
					symbolSize = TextEditor::calculate_text_size(
						Frenchie::Core::String::as_utf8(std::wstring(1, text[positionInLine])).c_str());
					
					symbolRect = ImRect(rowRect.Min + symbolOffset, rowRect.Min + symbolOffset + symbolSize);

					// highlight symbol
					if(symbolRect.Contains(ImGui::GetMousePos()))
					{
						ImGui::GetWindowDrawList()->AddRectFilled(
							symbolRect.Min,
							symbolRect.Max,
							TextEditor::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg]));
					}

					// update cursor position
					if(ImGui::IsWindowFocused() &&
						m_TextRect.Contains(ImGui::GetMousePos()) &&
						symbolRect.Contains(ImGui::GetMousePos()) &&
						(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) || 
						ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right) || 
						ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle)))
					{
						m_TextModel->set_cursor_line(lineNumber);
						m_TextModel->set_cursor_column(positionInLine);
						symbolIsHovered = true;
					}

					// select symbol
					if(ImGui::IsWindowFocused() &&
						m_TextRect.Contains(ImGui::GetMousePos()) &&
						symbolRect.Contains(ImGui::GetMousePos()) &&
						(ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left) || 
						ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Right) || 
						ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Middle)))
					{
						m_TextModel->select(lineNumber, positionInLine);
					}

					// draw selection
					if(m_TextModel->is_selected(lineNumber, positionInLine))
					{
						ImGui::GetWindowDrawList()->AddRectFilled(
							symbolRect.Min,
							symbolRect.Max,
							TextEditor::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg]));
					}

					if(m_TextModel->get_cursros_line() == lineNumber && m_TextModel->get_cursros_column() == positionInLine) 
						m_CursorPosition = symbolRect.Min;

					symbolOffset = ImVec2(symbolOffset.x + symbolSize.x, 0.f);
				}

				// move cursor at the very end
				if(m_TextModel->get_cursros_column() >= text.size()) 
					m_CursorPosition = ImVec2(symbolRect.Max.x, symbolRect.Min.y);

				// move cursor at the very start
				if(m_TextModel->get_cursros_column() <= 0) 
					m_CursorPosition = rowRect.Min;

				// move cursor at the very start
				if(!symbolIsHovered &&
					m_TextRect.Contains(ImGui::GetMousePos()) &&
					ImGui::IsWindowHovered() &&
					rowRect.Contains(ImGui::GetMousePos()) &&
					(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) || 
					ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right) || 
					ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle)))
				{
					m_TextModel->set_cursor_line(lineNumber);
					m_TextModel->set_cursor_column(0);
				}

				// draw cursor
				if(ImGui::IsWindowHovered() && m_TextModel->get_cursros_line() == lineNumber)
				{	
					ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::CURSOR);

					// animated
					if(m_CursorTimer->get_elapsed_time() > 500)
					{
						ImGui::GetWindowDrawList()->AddText(
							m_CursorPosition - ImVec2(TextEditor::calculate_text_size("|").x, 0.f) * 0.5f, 
							TextEditor::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_InputTextCursor]),
							"|"
						);
					}
				}

				// draw text line bounding rectangle
				if(m_TextModel->get_cursros_line() == lineNumber)
				{
					ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::BACKGROUND);

					ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg];

					ImGui::GetWindowDrawList()->AddRectFilled(
						rowRect.Min, 
						rowRect.Max, 
						TextEditor::calculate_color(ImVec4(color.x, color.y, color.z, 0.1f)));
				}
			}
		}

		// adjust scroll bar
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
				if(m_TextModel->get_cursros_line() > clipper.DisplayEnd - 1)
					ImGui::SetScrollY(ImGui::GetScrollY() + ImGui::GetFontSize() * 4.f);
				else if(m_TextModel->get_cursros_line() < clipper.DisplayStart + 1)
					ImGui::SetScrollY(ImGui::GetScrollY() - ImGui::GetFontSize() * 4.f);

				// adjust X scroll
				if(!m_TextRect.Contains(m_CursorPosition))
					ImGui::SetScrollX(ImGui::GetScrollX() + m_CursorPosition.x - m_TextRect.Max.x);

				// on enter we move cursor back onto it's position
				if((ImGuiKey)key == ImGuiKey::ImGuiKey_Enter)
					ImGui::SetScrollX(0.f);
			}
		}

		m_ScrollPos = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
	}

	ImGui::EndChild();

	ImGui::PopStyleColor();
}

void TextEditor::draw_status_panel()
{
	ImGui::BeginChild("StatusBar", 
		ImGui::GetContentRegionAvail(), 
		ImGuiChildFlags_::ImGuiChildFlags_Borders, 
		ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoInputs);
	
	if(m_TextModel != nullptr)
	{
		ImGui::TextUnformatted(
			fmt::format("Line: {} Column: {}", m_TextModel->get_cursros_line(), m_TextModel->get_cursros_column()).c_str()
		);
	}

	ImGui::EndChild();
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