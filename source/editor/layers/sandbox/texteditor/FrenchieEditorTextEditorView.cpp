#include <FrenchieEditorTextEditorView.hpp>

#include <fstream>

// Editor
#include <FrenchieEditorTextEditorTreeSitterTextHighlighter.hpp>
#include <FrenchieEditorTextEditorRegexTextHighlighter.hpp>
#include <FrenchieEditorHelpers.hpp>

using namespace Frenchie::Application;
using namespace Frenchie::Editor;

// add sandbox elements into main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenTextDocumentViewDebug : 
                public Frenchie::Application::Command::Registry<OpenTextDocumentViewDebug, const CommandPayloads&>
            {
            public:

                OpenTextDocumentViewDebug(const CommandPayloads& _Sender = CommandPayloads()) : 
                    Frenchie::Application::Command::Registry<OpenTextDocumentViewDebug, const CommandPayloads&>(_Sender){}
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

TextDocumentSeclection::TextDocumentSeclection(){}
TextDocumentSeclection::~TextDocumentSeclection(){}

void TextDocumentSeclection::select(const int& _Index)
{
    m_First = std::min(m_First, _Index);
    m_Last  = std::max(m_Last, _Index);

    if(_Index < m_First)
        m_First = _Index;
    else if(_Index > m_First)
        m_Last = _Index;
}

void TextDocumentSeclection::clear()
{
    m_First = INT_MAX;
    m_Last  = INT_MIN;
}

int TextDocumentSeclection::first() const
{
    return m_First;
}

int TextDocumentSeclection::last() const
{
    return m_Last;
}

int TextDocumentSeclection::size() const
{
    return empty() ? 0 : m_Last - m_First + 1;
}

bool TextDocumentSeclection::empty() const
{
    return m_First == INT_MAX && m_Last == INT_MIN;
}

bool TextDocumentSeclection::is_selected(const int& _Index) const
{
    return _Index >= m_First && 
            _Index <= m_Last;
}

// TextDocumentView
TextDocumentView::TextDocumentView(const std::shared_ptr<Frenchie::Core::TextDocument>& _Document) :
    Frenchie::Application::Layer(STRINGIFY(TextDocumentView)),
    // we guarantee that text document is not nullptr
    m_TextDocument(_Document == nullptr ? std::make_shared<Frenchie::Core::TextDocument>(std::u32string()) : _Document)
{}

TextDocumentView::~TextDocumentView(){}

#include <FrenchieEditorCodeEditorTreeSitterCpp.hpp>

bool TextDocumentView::awake()
{
    m_CursorFrameCounter = 
        Frenchie::Application::application()->push_layer<FrameCounter>(80);

    m_Highlighter = std::make_shared<Frenchie::Editor::RegexTextHighlighter>(
        std::vector<RegexTextHighlighter::HighlightRule>(
        {
            // numbers
            RegexTextHighlighter::HighlightRule(
                U"[+-]?(\\d+(\\.\\d*)?|\\.\\d+)", 
                IM_COL32(156, 156, 82, 255)),

            // variables
            RegexTextHighlighter::HighlightRule(
                UR"(\b[A-Za-z_]\w*\b)",
                IM_COL32(255, 255, 255, 255)),

            // keywords
            RegexTextHighlighter::HighlightRule(
                UR"(alignof|alignas|asm|auto|class|consteval|constinit|constexpr|const_cast|decltype|delete|dynamic_cast|enum|explicit|false|final|friend|inline|namespace|new|noexcept|nullptr|operator|override|private|protected|public|reinterpret_cast|sizeof|static_assert|static_cast|struct|template|this|true|typedef|typeid|typename|union|using|virtual|and|and_eq|bitand|bitor|compl|not|not_eq|or|or_eq|xor|xor_eq|concept|requires|import|module|export)", 
                IM_COL32(10, 8, 156, 255)),

            // modifiers
            RegexTextHighlighter::HighlightRule(
                UR"(const|extern|mutable|register|static|thread_local|volatile)", 
                IM_COL32(10, 8, 156, 255)),

            // controlflow
            RegexTextHighlighter::HighlightRule(
                UR"(break|case|catch|continue|default|do|else|for|goto|if|return|switch|throw|try|while|co_await|co_return|co_yield)", 
                IM_COL32(146, 8, 156, 255)),

            // types
            RegexTextHighlighter::HighlightRule(
                UR"(bool|char|char8_t|char16_t|char32_t|double|float|int|long|short|signed|unsigned|void|int8_t|int16_t|int32_t|int64_t|uint8_t|uint16_t|uint32_t|uint64_t|int_least8_t|int_least16_t|int_least32_t|int_least64_t|uint_least8_t|uint_least16_t|uint_least32_t|uint_least64_t|int_fast8_t|int_fast16_t|int_fast32_t|int_fast64_t|uint_fast8_t|uint_fast16_t|uint_fast32_t|uint_fast64_t|size_t|size_t|wchar_t|intptr_t|uintptr_t|intmax_t|uintmax_t|ptrdiff_t|sig_atomic_t|wint_t|va_list|FILE|fpos_t|time_t)", 
                IM_COL32(10, 8, 156, 255)),

            // attributes
            RegexTextHighlighter::HighlightRule(
                UR"(\[\[(.*)\]\])", 
                IM_COL32(57, 247, 5, 255)),

            // preprocessor directives
            RegexTextHighlighter::HighlightRule(
                UR"(\#.*)", 
                IM_COL32(61, 45, 1, 255)),

            // single line comment
            RegexTextHighlighter::HighlightRule(
                UR"(//.*)", 
                IM_COL32(0, 255, 0, 255)),

            // multiline patterns

            // strings
            RegexTextHighlighter::HighlightRule(
                UR"(\"[^"])",
                IM_COL32(61, 45, 1, 255),
                RegexTextHighlighter::HighlightRule::MULTILINE_START),

            RegexTextHighlighter::HighlightRule(
                UR"([^"]\")",
                IM_COL32(61, 45, 1, 255),
                RegexTextHighlighter::HighlightRule::MULTILINE_FINISH),

            // comments
            RegexTextHighlighter::HighlightRule(
                UR"(/\*)",
                IM_COL32(0, 255, 0, 255),
                RegexTextHighlighter::HighlightRule::MULTILINE_START),

            RegexTextHighlighter::HighlightRule(
                UR"(\*/)",
                IM_COL32(0, 255, 0, 255),
                RegexTextHighlighter::HighlightRule::MULTILINE_FINISH)
        }),
        Helpers::imgui_calculate_color(ImGui::GetStyle().Colors[ImGuiCol_Text])
    );

    // m_Highlighter = std::make_shared<Frenchie::Editor::TreeSitterSyntaxHighlighter>(
    //     Frenchie::Editor::tree_sitter_cpp(),
    //     std::string(Frenchie::Editor::tree_sitter_cpp_query),
    //     TreeSitterSyntaxHighlighter::ThemeColors({
    //         {"keyword", IM_COL32(0,0,255,255)},
    //         {"string", IM_COL32(255,255,255,255)},
    //         {"number", IM_COL32(0,255,0,255)},
    //         {"comment", IM_COL32(255,255,255,255)},
    //         {"text", IM_COL32(255,255,255,255)},
    //         {"function", IM_COL32(255,0,0,255)},
    //         {"type", IM_COL32(255,255,255,255)},
    //         {"variable", IM_COL32(255,255,255,255)}
    //     })
    // );

    std::ifstream t("C:/SDK/Qt_Projects/PowerCAD/source/kernel/Common/DynamicGraph.cpp");
    t.seekg(0, std::ios::end);
    size_t size = t.tellg();
    std::string buffer(size, ' ');
    t.seekg(0);
    t.read(&buffer[0], size);

    m_TextDocument->insert(Frenchie::Core::String::convert_utf8_to_utf32(buffer));

    return true;
}

void TextDocumentView::frame_update()
{
    // auxiliary lambdas
    auto drawCursor = [this](const ImVec2& _Position)
    {
        if(m_CursorFrameCounter->get_frames_count() < 20)
            return;

        ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::CURSOR);

        ImGui::GetWindowDrawList()->AddText(
                _Position + ImVec2(2.f, 0.f) - ImVec2(Helpers::imgui_calculate_text_size("|").x, 0.f) * 0.5f,
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

            // selection
            editor_clear_selection_command();

            // copy / paste
            editor_copy_command();
            editor_paste_command();

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
                Helpers::imgui_calculate_color(ImGui::GetStyle().Colors[ImGuiCol_FrameBg]));

            ImVec2 contentSize = ImVec2(
                m_MaxWidth * ImGui::GetFontSize(), // this is the room for text width, i.e this width if far greater than text itself
                (m_TextDocument->lines_count() + 2) * ImGui::GetFontSize());

            // draw line numbers
            ImGui::SetNextWindowContentSize(contentSize);

            ImGui::BeginChild(
                "LineNumbers",
                ImVec2(Helpers::imgui_calculate_text_size(std::to_string(INT_MAX)).x, ImGui::GetContentRegionAvail().y),
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
                m_End      = m_Start + std::min<int>((int)(m_ViewPort.GetSize().y / ImGui::GetFontSize()), m_TextDocument->lines_count());

                // draw line numbers
                for (int lineIndex = m_Start; lineIndex < m_End; lineIndex++)
                {
                    auto position = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + lineIndex * ImGui::GetFontSize());
                    auto size     = ImVec2(Helpers::imgui_calculate_text_size(std::to_string(INT_MAX)).x, ImGui::GetFontSize());

                    ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::BACKGROUND);

                    ImGui::GetWindowDrawList()->AddRectFilled(
                        position,
                        position + size,
                        lineIndex % 2 == 0 ?
                            Helpers::imgui_calculate_color(ImGui::GetStyle().Colors[ImGuiCol_TableRowBg]) :
                            Helpers::imgui_calculate_color(ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt]));

                    ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::DOCUMENT);

                    ImGui::GetWindowDrawList()->AddText(
                        position,
                        Helpers::imgui_calculate_color(ImGui::GetStyle().Colors[ImGuiCol_Text]),
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
                    Helpers::imgui_calculate_color(ImGui::GetStyle().Colors[ImGuiCol_FrameBg]));

                // draw text
                m_Scroll   = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
                m_ViewPort = ImRect(ImGui::GetCursorScreenPos() + m_Scroll, ImGui::GetCursorScreenPos() + m_Scroll + ImGui::GetWindowSize());
                m_Start    = (int)(m_Scroll.y / ImGui::GetFontSize());
                m_End      = m_Start + std::min<int>((int)(m_ViewPort.GetSize().y / ImGui::GetFontSize()), m_TextDocument->lines_count());

                for (int lineIndex = m_Start; lineIndex < m_End; lineIndex++)
                {
                    Frenchie::Core::TextDocumentSymbolIterator lineBeginIterator = m_TextDocument->line_begin(lineIndex);
                    Frenchie::Core::TextDocumentSymbolIterator lineEndIterator   = m_TextDocument->line_end(lineIndex);
                
                    m_MaxWidth = std::max<int>(m_MaxWidth, lineEndIterator.get_position() - lineBeginIterator.get_position());

                    int distance = (int)(m_Scroll.x / ImGui::GetFontSize());
                    lineBeginIterator.increment_by(distance);    

                    if(lineBeginIterator.get_position() >= lineEndIterator.get_position())
                        continue;

                    // compute visible text width and maximum symbols count
                    int   maximumSymbolsCount = 0;
                    float visibleTextWidth    = 0.f;

                    for (auto it = lineBeginIterator;
                              it != lineEndIterator && visibleTextWidth < m_ViewPort.GetSize().x;
                              ++it, ++maximumSymbolsCount)
                    {
                        visibleTextWidth += Helpers::imgui_calculate_text_size(
                            Frenchie::Core::String::convert_utf32_to_utf8(std::u32string(1, *it))).x;
                    }
                    

                    // highlight text
                    std::u32string text =
                        m_TextDocument->get_text(lineBeginIterator, lineEndIterator, maximumSymbolsCount);

                    HighlightRulesEstimationResults matches = m_Highlighter->highlight(text, lineIndex);

                    ImVec2 symbolPosition = ImVec2(ImGui::GetCursorScreenPos().x + m_Scroll.x, ImGui::GetCursorScreenPos().y + lineIndex * ImGui::GetFontSize());
                    int    globalIndex    = lineBeginIterator.get_position();

					for(auto&& match : matches)
					{
                        for(int index = match.second.Match.Start; index < match.second.Match.Finish; ++index, ++globalIndex)
                        {
                            std::string symbol =
                                Frenchie::Core::String::convert_utf32_to_utf8(
                                    std::u32string(1, text[index]));

                            // draw symbol
                            ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::DOCUMENT);
                            ImGui::GetWindowDrawList()->AddText(symbolPosition, match.second.Color, symbol.c_str());

                            // highlight symbol
                            ImVec2 symbolSize      = Helpers::imgui_calculate_text_size(symbol);
                            ImRect symbolRectangle = ImRect(symbolPosition, symbolPosition + symbolSize);

                            if(symbolRectangle.Contains(ImGui::GetMousePos()))
                                editor_select_command(globalIndex);
                            
                            if(symbolRectangle.Contains(ImGui::GetMousePos()) || 
                                m_Selection.is_selected(globalIndex))
                            {
                                ImGui::GetWindowDrawList()->ChannelsSetCurrent(Layers::BACKGROUND);
                                ImGui::GetWindowDrawList()->AddRectFilled(symbolRectangle.Min, symbolRectangle.Max, IM_COL32(255, 0, 0, 128));

                                // update cursor position
                                if((ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)  || 
                                    ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right) || 
                                    ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle)))
                                {
                                    m_Selection.clear();
                                    m_TextDocument->set_cursor_position(globalIndex);
                                }
                            }

                            // draw cursor
                            if(symbol[0] != '\n' && globalIndex == m_TextDocument->get_cursor_position())
                                drawCursor(symbolPosition);

                            // update position
                            symbolPosition += ImVec2(symbolSize.x, 0.f);
                        }
					}

                    // draw cursor if it's at the end of the line
                    if(lineEndIterator.get_position() == m_TextDocument->get_cursor_position())
                        drawCursor(symbolPosition);
                }

                // draw cursor if it's at the end of the line
                if(m_TextDocument->get_cursor_position() <= 0)
                    drawCursor(ImVec2(ImGui::GetCursorScreenPos().x + m_Scroll.x, ImGui::GetCursorScreenPos().y));

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
            ImGui::TextUnformatted(fmt::format("Cursor {} Selection {} {} Symbols count {}",
                m_TextDocument->get_cursor_position(),
                m_Selection.first(),
                m_Selection.last(),
                m_TextDocument->symbols_count()).c_str());

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
		on_character_pressed((unsigned int)'\t');
        return;
	}

    if (ImGui::Shortcut(ImGuiKey::ImGuiKey_Enter, ImGuiInputFlags_::ImGuiInputFlags_Repeat))
	{
		on_character_pressed((unsigned int)'\n');
        on_character_pressed((unsigned int)'\0');
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

				on_character_pressed(c);
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
        Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                m_TextDocument->erase(std::max<int>(m_Selection.size(), 1));
                m_Selection.clear();
            }
        );
    }
}

void TextDocumentView::document_move_cursor_left_command()
{
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftArrow))
    {
        Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                m_Selection.clear();
                m_TextDocument->move_cursor_left();
            }
        );
    }
}

void TextDocumentView::document_move_cursor_right_command()
{
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightArrow))
    {
        Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                m_Selection.clear();
                m_TextDocument->move_cursor_right();
            }
        );
    }
}

void TextDocumentView::document_move_cursor_down_command()
{
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_DownArrow))
    {
        Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                m_Selection.clear();
                m_TextDocument->move_cursor_down();
            }
        );
    }
}

void TextDocumentView::document_move_cursor_up_command()
{
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_UpArrow))
    {
        Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                m_Selection.clear();
                m_TextDocument->move_cursor_up();
            }
        );
    }
}

void TextDocumentView::document_undo_command()
{
    if(ImGui::Shortcut(ImGuiKey::ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_Z))
    {
        Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                m_Selection.clear();
                m_TextDocument->undo();
            }
        );
    }
}

void TextDocumentView::document_redo_command()
{
    if(ImGui::Shortcut(ImGuiKey::ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_Y))
    {
        Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                m_Selection.clear();
                m_TextDocument->redo();
            }
        );
    }
}

void TextDocumentView::editor_copy_command()
{
	if(ImGui::Shortcut(ImGuiKey::ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_C))
	{
        Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                if(m_Selection.empty())
                    return;

                auto selectionText = 
                    m_TextDocument->get_text(
                        m_TextDocument->symbol_begin(m_Selection.first()),
                        m_TextDocument->symbol_begin(m_Selection.last() + 1));

                if(!selectionText.empty())
                    ImGui::SetClipboardText(Frenchie::Core::String::convert_utf32_to_utf8(selectionText).c_str());
            }
        );
	}
}

void TextDocumentView::editor_paste_command()
{
	if(ImGui::Shortcut(ImGuiKey::ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_V))
	{
        Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                std::string clipBoardText =
                    std::string(ImGui::GetClipboardText());

                if(!clipBoardText.empty())
                    m_TextDocument->insert(Frenchie::Core::String::convert_utf8_to_utf32(clipBoardText));
            }
        );
	}
}

void TextDocumentView::editor_clear_selection_command()
{
    if(ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
            [this]()
            {
                m_Selection.clear();
            }
        );
    }
}

void TextDocumentView::editor_select_command(const int& _Position)
{
    if(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
        ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
    {
        Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
            [this, _Position]()
            {
                m_Selection.select(_Position);
                m_TextDocument->set_cursor_position(_Position + 1);
            }
        );
    }
}

void TextDocumentView::on_character_pressed(const unsigned int& _Char)
{
    Frenchie::Application::commands()->push<Frenchie::Application::CallbackCommand>(
        [this, _Char]()
        {
            // remove selection
            m_TextDocument->erase(m_Selection.size());
            m_Selection.clear();

            // insert symbol
            m_TextDocument->insert(Frenchie::Core::String::convert_utf8_to_utf32(
                Helpers::imgui_convert_text_char_to_utf8(_Char)
            ));
        }
    );
}