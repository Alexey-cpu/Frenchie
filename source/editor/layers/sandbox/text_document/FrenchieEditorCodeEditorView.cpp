#include <FrenchieEditorCodeEditorView.hpp>

using namespace Frenchie::Editor;

// add sandbox elements into main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenTextDocumentViewDebug : 
                public Frenchie::Application::Command::Registry<OpenTextDocumentViewDebug, void*>
            {
            public:

                OpenTextDocumentViewDebug(void* _Sender = nullptr) : 
                    Frenchie::Application::Command::Registry<OpenTextDocumentViewDebug, void*>(_Sender){}
                virtual ~OpenTextDocumentViewDebug(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<TextDocumentView>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Debug::TextDocumentView");
                }
            };

            const bool openTextDocumentViewDebugActionRegistry = OpenTextDocumentViewDebug::registerFactory();
        }
    }
}

// Helpers
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

    static ImU32 Helpers::calculate_color(const ImVec4& _Vector)
    {
        return IM_COL32(_Vector.x * 255.f, _Vector.y * 255.f, _Vector.z * 255.f, _Vector.w * 255.f);
    }

    static ImVec2 calculate_text_size(const std::string& _Text)
    {
        return ImGui::GetCurrentContext()->Font->CalcTextSizeA(
            ImGui::GetFontSize(), FLT_MAX, 0.f, _Text.c_str(), nullptr, nullptr);
    }
};

// TextDocumentView
TextDocumentView::TextDocumentView() :
    Frenchie::Application::Layer(STRINGIFY(TextDocumentView)){}

TextDocumentView::~TextDocumentView(){}

bool TextDocumentView::awake()
{
    std::u32string text;

    for (int i = 0; i < 1e6; i++)
    {
        //text.append(std::to_wstring(i)).append(L"\t");
        
        if(i % 2 > 0)
        {
            for (int j = 0; j < 3; j++)
            {
                text.append(U"Всем привет !!!!");
            }
        }
        else
        {
            for (int j = 0; j < 1; j++)
            {
                text.append(U"Всем привет !!!!");
            }
        }

        text.append(U"\n");
    }

    m_Table = std::make_unique<Frenchie::Core::TextDocument>(text);

    return true;
}

void TextDocumentView::frame_update()
{
    // auxiliary lambdas
    auto drawCursor = [this](const ImVec2& _Position)
    {
        ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::CURSOR);

        ImGui::GetWindowDrawList()->AddText(
                _Position + ImVec2(2.f, 0.f) - ImVec2(Helpers::calculate_text_size("|").x, 0.f) * 0.5f,
                IM_COL32(0, 255, 0, 255),
                "|");
    };

    // draw window
    ImGui::Begin(get_name().c_str(), &m_Opened);
    {
        if(ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_RootAndChildWindows))
        {
            // edit
            document_insert_symbol_command();
            document_erase_symbol_command();

            // cursor
            document_move_cursor_left_command();
            document_move_cursor_right_command();
            document_move_cursor_down_command();
            document_move_cursor_up_command();

            // undo/redo
            document_undo_command();
            document_redo_command();
        }

        ImVec2 statusPanelSize = ImVec2(ImGui::GetContentRegionAvail().x, 100.f);

        ImGui::BeginChild(
            "TextEditor",
            ImVec2(ImGui::GetContentRegionAvail().x,  ImGui::GetContentRegionAvail().y - statusPanelSize.y),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking         |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar        |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoResize          |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoMove            |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar       |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings
        );
        {
            ImGui::GetWindowDrawList()->ChannelsSplit(Layers::COUNT);
            ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::BACKGROUND);

            ImGui::GetWindowDrawList()->AddRectFilled(
                ImGui::GetCursorScreenPos() - ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()), 
                ImGui::GetCursorScreenPos() + ImVec2((float)INT_MAX, (float)INT_MAX), 
                Helpers::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_FrameBg]));

            ImVec2 contentSize = ImVec2(
                m_MaxWidth * ImGui::GetFontSize(), // this is the room for text width, i.e this width if far greater than text itself
                (m_Table->lines_count() + 2) * ImGui::GetFontSize());

            // draw line numbers
            ImGui::SetNextWindowContentSize(contentSize);

            ImGui::BeginChild(
                "LineNumbers",
                ImVec2(Helpers::calculate_text_size(std::to_string(INT_MAX)).x, ImGui::GetContentRegionAvail().y),
                ImGuiChildFlags_::ImGuiChildFlags_Borders,
                ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking         |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar        |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoResize          |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoMove            |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar       |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings   |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoInputs
            );
            {
                ImGui::GetWindowDrawList()->ChannelsSplit(Layers::COUNT);

                // setup scroll position
                ImGui::SetScrollY(m_Scroll.y);

                // update meta info
                m_Scroll   = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
                m_ViewPort = ImRect(ImGui::GetCursorScreenPos() + m_Scroll, ImGui::GetCursorScreenPos() + m_Scroll + ImGui::GetWindowSize());
                m_Start    = (int)(m_Scroll.y / ImGui::GetFontSize());
                m_End      = m_Start + std::min<int>((int)(m_ViewPort.GetSize().y / ImGui::GetFontSize()), m_Table->lines_count());

                // draw line numbers
                for (int lineIndex = m_Start; lineIndex < m_End; lineIndex++)
                {
                    auto position = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + lineIndex * ImGui::GetFontSize());
                    auto size     = ImVec2(Helpers::calculate_text_size(std::to_string(INT_MAX)).x, ImGui::GetFontSize());

                    ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::BACKGROUND);

                    ImGui::GetWindowDrawList()->AddRectFilled(
                        position,
                        position + size,
                        lineIndex % 2 == 0 ?
                            Helpers::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_TableRowBg]) :
                            Helpers::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt]));

                    ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::DOCUMENT);

                    ImGui::GetWindowDrawList()->AddText(
                        position,
                        Helpers::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_Text]),
                        std::to_string(lineIndex).c_str()
                    );
                }
                
                ImGui::EndChild();
            }

            ImGui::SameLine();

            // draw text
            ImGui::SetNextWindowContentSize(contentSize);

            ImGui::BeginChild(
                "Text",
                ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
                ImGuiChildFlags_::ImGuiChildFlags_Borders,

                ImGuiWindowFlags_HorizontalScrollbar           |
                ImGuiWindowFlags_NoNavInputs                   |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking  |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoResize   |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoMove     |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings
            );
            {
                // draw text background
                ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ScrollbarBg, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
                ImGui::GetWindowDrawList()->ChannelsSplit(Layers::COUNT);
                ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::BACKGROUND);
                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImGui::GetCursorScreenPos() - ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()),
                    ImGui::GetCursorScreenPos() + ImVec2((float)INT_MAX, (float)INT_MAX),
                    Helpers::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_FrameBg]));

                // draw text
                m_Scroll   = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
                m_ViewPort = ImRect(ImGui::GetCursorScreenPos() + m_Scroll, ImGui::GetCursorScreenPos() + m_Scroll + ImGui::GetWindowSize());
                m_Start    = (int)(m_Scroll.y / ImGui::GetFontSize());
                m_End      = m_Start + std::min<int>((int)(m_ViewPort.GetSize().y / ImGui::GetFontSize()), m_Table->lines_count());

                for (int lineIndex = m_Start; lineIndex < m_End; lineIndex++)
                {
                    Frenchie::Core::TextDocumentSymbolIterator lineBeginIterator = m_Table->line_begin(lineIndex);
                    Frenchie::Core::TextDocumentSymbolIterator lineEndIterator   = m_Table->line_end(lineIndex);
                
                    m_MaxWidth = std::max<int>(m_MaxWidth, lineEndIterator.get_position() - lineBeginIterator.get_position());

                    int distance = (int)(m_Scroll.x / ImGui::GetFontSize());
                    lineBeginIterator.increment_by(distance);    

                    if(lineBeginIterator.get_position() >= lineEndIterator.get_position())
                        continue;

                    // highlight text
                    std::u32string text = m_Table->get_text(lineBeginIterator, lineEndIterator, 1024);

                    SyntaxHighlighter::regexEstimationResults matches =
                        m_Highlighter.highlight(
                            text,
                            m_Patterns,
                            Helpers::calculate_color(ImGui::GetStyle().Colors[ImGuiCol_Text]),
                            lineIndex);

                    ImVec2 symbolPosition = ImVec2(ImGui::GetCursorScreenPos().x + m_Scroll.x, ImGui::GetCursorScreenPos().y + lineIndex * ImGui::GetFontSize());
                    int    globalIndex    = lineBeginIterator.get_position();

					for(auto&& match : matches)
					{
                        for(int index = match.second.Match.Start; index < match.second.Match.Finish; index++, globalIndex++)
                        {
                            std::string symbol =
                                Frenchie::Core::String::convert_utf32_to_utf8(
                                    std::u32string(1, text[index]));

                            // draw symbol
                            ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::DOCUMENT);
                            ImGui::GetWindowDrawList()->AddText(symbolPosition, match.second.Color, symbol.c_str());

                            // highlight symbol
                            ImVec2 symbolSize      = Helpers::calculate_text_size(symbol);
                            ImRect symbolRectangle = ImRect(symbolPosition, symbolPosition + symbolSize);

                            if(symbolRectangle.Contains(ImGui::GetMousePos()))
                            {
                                ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::BACKGROUND);
                                ImGui::GetWindowDrawList()->AddRectFilled(symbolRectangle.Min, symbolRectangle.Max, IM_COL32(255, 0, 0, 128));

                                // update cursor position
                                if((ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)  || 
                                    ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right) || 
                                    ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle)))
                                {
                                    m_Table->set_cursor_position(globalIndex);
                                }
                            }

                            // draw cursor
                            if(symbol[0] != '\n' && globalIndex == m_Table->get_cursor_position())
                                drawCursor(symbolPosition);

                            // update position
                            symbolPosition += ImVec2(symbolSize.x, 0.f);
                        }
					}

                    // draw cursor if it's at the end of the line
                    if(lineEndIterator.get_position() == m_Table->get_cursor_position())
                        drawCursor(symbolPosition);
                }

                ImGui::PopStyleColor(1);
                ImGui::EndChild();
            }

            ImGui::EndChild();
        }

        ImGui::BeginChild(
            "StatusPanel",
            ImVec2(ImGui::GetContentRegionAvail().x,  ImGui::GetContentRegionAvail().y - statusPanelSize.y),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking  |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoResize   |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoMove     |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings
        );
        {
            ImGui::TextUnformatted(fmt::format("Cursor {}", m_Table->get_cursor_position()).c_str());

            ImGui::EndChild();
        }

        ImGui::End();
    }
}

void TextDocumentView::frame_finish(){}

bool TextDocumentView::allows_multiple_instances() const
{
    return false;
}

void TextDocumentView::document_insert_symbol_command()
{
	ImGuiIO& io = ImGui::GetIO();

	if (ImGui::Shortcut(ImGuiKey::ImGuiKey_Tab, ImGuiInputFlags_::ImGuiInputFlags_Repeat))
	{
		on_character_ressed((unsigned int)'\t');
        return;
	}

    if (ImGui::Shortcut(ImGuiKey::ImGuiKey_Enter, ImGuiInputFlags_::ImGuiInputFlags_Repeat))
	{
		on_character_ressed((unsigned int)'\n');
        return;
	}

	if (io.InputQueueCharacters.Size > 0)
	{
		if (!((io.KeyCtrl && !io.KeyAlt) || (io.ConfigMacOSXBehaviors && io.KeyCtrl)))
		{
			for (int n = 0; n < io.InputQueueCharacters.Size; n++)
			{
				// Insert character if they pass filtering
				unsigned int c = (unsigned int)io.InputQueueCharacters[n];

				if (c == '\t' || c == '\n') // Skip Tab and Enter (see above)
					continue;

				on_character_ressed(c);
			}
		}

		// consume user input
		io.InputQueueCharacters.resize(0);
	}
}

void TextDocumentView::document_erase_symbol_command()
{
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Backspace))
    {
        Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                if(m_Table != nullptr)
                    m_Table->erase();
            }
        );
    }
}

void TextDocumentView::document_move_cursor_left_command()
{
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftArrow))
    {
        Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                if(m_Table != nullptr)
                    m_Table->move_cursor_left();
            }
        );
    }
}

void TextDocumentView::document_move_cursor_right_command()
{
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightArrow))
    {
        Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                if(m_Table != nullptr)
                    m_Table->move_cursor_right();
            }
        );
    }
}

void TextDocumentView::document_move_cursor_down_command()
{
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_DownArrow))
    {
        Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                if(m_Table != nullptr)
                    m_Table->move_cursor_down();
            }
        );
    }
}

void TextDocumentView::document_move_cursor_up_command()
{
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_UpArrow))
    {
        Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                if(m_Table != nullptr)
                    m_Table->move_cursor_up();
            }
        );
    }
}

void TextDocumentView::document_undo_command()
{
    if(ImGui::Shortcut(ImGuiKey::ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_Z))
    {
        Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                if(m_Table != nullptr)
                    m_Table->undo();
            }
        );
    }
}

void TextDocumentView::document_redo_command()
{
    if(ImGui::Shortcut(ImGuiKey::ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_Y))
    {
        Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                if(m_Table != nullptr)
                    m_Table->redo();
            }
        );
    }
}

void TextDocumentView::on_character_ressed(const unsigned int& _Char)
{
    Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
        [this, _Char]()
        {
            // retrieve user input in UTF-8 codec
            char utf8[5];
            int  count = Helpers::ImTextCharToUtf8(utf8, _Char);

            // insert symbol
            if(m_Table != nullptr)
                m_Table->insert(Frenchie::Core::String::convert_utf8_to_utf32(std::string(utf8, count)));
        }
    );
}