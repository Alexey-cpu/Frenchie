#include <FrenchiePieceTableDrawer.hpp>

using namespace Frenchie::Editor;

// add sandbox elements into main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenPieceTableDrawerDebug : 
                public Frenchie::Application::Command::Registry<OpenPieceTableDrawerDebug, void*>
            {
            public:

                OpenPieceTableDrawerDebug(void* _Sender = nullptr) : 
                    Frenchie::Application::Command::Registry<OpenPieceTableDrawerDebug, void*>(_Sender){}
                virtual ~OpenPieceTableDrawerDebug(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<PieceTableDrawer>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Debug::PieceTableDrawer");
                }
            };

            const bool openPieceTableDrawerDebugActionRegistry = OpenPieceTableDrawerDebug::registerFactory();
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
};

// PieceTableDrawer
PieceTableDrawer::PieceTableDrawer() : Frenchie::Application::Layer(STRINGIFY(PieceTableDrawer)){}
PieceTableDrawer::~PieceTableDrawer(){}

//#define PIECE_TABLE_DRAWER_DEBUG

bool PieceTableDrawer::awake()
{
    std::u32string text;

    #ifndef PIECE_TABLE_DRAWER_DEBUG
    for (int i = 0; i < 1e6; i++)
    #else
    for (int i = 0; i < 1; i++)
    #endif
    {
        //text.append(std::to_wstring(i)).append(L"\t");
        
        for (int j = 0; j < 1; j++)
        {
            text.append(U"Всем привет !!!!");
        }

        text.append(U"\n");
    }

    m_Table = std::make_unique<Frenchie::Core::TextDocument>(text);

    m_Table->insert(6, U"123");
    m_Table->insert(12, U"\n456\n789");
    m_Table->insert(12, U"\n121314");

    return true;
}

#ifndef PIECE_TABLE_DRAWER_DEBUG

void PieceTableDrawer::frame_update()
{
    int start = 0;
    int end   = 0;

    ImGui::Begin(get_name().c_str(), &m_Opened);
    {
        ImGui::SetNextWindowContentSize(ImVec2(10000, (m_Table->lines_count() + 2) * ImGui::GetFontSize()));

        ImGui::BeginChild(
            "TextEditor",
            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 100.f),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking  |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoResize   |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoMove     |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings
        );
        {
            // execute commands here
            if(ImGui::IsWindowHovered())
            {
                document_insert_symbol_command();
            }

            ImVec2 scroll   = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());

            ImRect viewport = ImRect(ImGui::GetCursorScreenPos() + scroll, 
                                    ImGui::GetCursorScreenPos()  + scroll + ImGui::GetWindowSize());

            start = (int)(scroll.y / ImGui::GetFontSize());
            end   = start + std::min<int>((int)(viewport.GetSize().y / ImGui::GetFontSize()), m_Table->lines_count());

			ImRect symbolRectangle = ImRect(viewport.Min, viewport.Min);
			ImVec2 symbolPosition  = viewport.Min;
			ImVec2 symbolOrigin    = viewport.Min;

            for (int lineIndex = start; lineIndex < end; lineIndex++)
            {
                m_Table->line_begin(lineIndex);
                m_Table->line_end(lineIndex);

                for (auto it = m_Table->line_begin(lineIndex); it != m_Table->line_end(lineIndex); it++)
                {
                    // retrieve symbol
                    std::string symbol =
                        Frenchie::Core::UTF::utf32_to_utf8(
                            std::u32string(1, *it));

                    // draw symbol
                    ImGui::GetWindowDrawList()->AddText(
                            symbolPosition,
                            IM_COL32(0, 255, 0, 255),
                            symbol.c_str()
                        );

                    ImVec2 symbolSize =
                        ImGui::GetCurrentContext()->Font->CalcTextSizeA(
                            ImGui::GetFontSize(),
                            FLT_MAX,
                            0.f,
                            symbol.c_str(),
                            NULL,
                            NULL
                        );

                    // highlight symbol
                    symbolRectangle = ImRect(symbolPosition, symbolPosition + symbolSize);

                    // update symbol position
                    symbolPosition =
                        symbol[0] == '\n' ?
                            ImVec2(symbolOrigin.x, symbolPosition.y + ImGui::GetFontSize()) :
                                symbolPosition + ImVec2(symbolSize.x, 0.f);
                }
            }

            ImGui::EndChild();
        }

        ImGui::BeginChild("Status", ImVec2(ImGui::GetContentRegionAvail().x, 100.f));
        {
            ImGui::TextUnformatted(fmt::format("start {} end {} pieces count {}", start, end, m_Table->pieces_count()).c_str());

            ImGui::EndChild();
        }

        ImGui::End();
    }
}

#else

void PieceTableDrawer::frame_update()
{
    ImGui::Begin(get_name().c_str(), &m_Opened);

    ImGui::BeginChild(
        "Contents",
        ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 100.f));
    {
        if(ImGui::BeginTable("FileSystemContentTable",
            2,
            ImGuiTableFlags_::ImGuiTableFlags_ScrollY      | 
            ImGuiTableFlags_::ImGuiTableFlags_RowBg        | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersOuter | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersV     |
            ImGuiTableFlags_::ImGuiTableFlags_Resizable    |
            ImGuiTableFlags_::ImGuiTableFlags_Reorderable  |
            ImGuiTableFlags_::ImGuiTableFlags_Hideable,
            ImGui::GetContentRegionAvail()))
        {
            ImGui::TableNextRow();

            // draw tree
            ImGui::TableSetColumnIndex(0);

            float heigth = 64.f;

            ImGui::BeginChild(
                "TOOLBAR",
                ImVec2(ImGui::GetContentRegionAvail().x, heigth));
            {
                if(ImGui::Button("Undo", ImVec2(0.f, heigth))) m_Table->undo();
                ImGui::SameLine();
                if(ImGui::Button("Redo", ImVec2(0.f, heigth))) m_Table->redo();

                ImGui::EndChild();
            }

            ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - heigth));
            {
                // ImGui::BeginChild("Buttons", ImVec2(ImGui::GetContentRegionAvail().x, 100.f));
                // {
                //     if(ImGui::Button("Undo")) m_Table->undo();
                //     if(ImGui::Button("Redo")) m_Table->redo();

                //     ImGui::EndChild();
                // }

                if(ImGui::IsWindowHovered())
                {
                    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftArrow))
                    {
                        m_Table->move_cursor_left();
                    }
                    
                    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightArrow))
                    {
                        m_Table->move_cursor_right();
                    }
                    
                    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_UpArrow)) 
                        m_Table->move_cursor_up();
                    
                    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_DownArrow)) 
                        m_Table->move_cursor_down();

                    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter))
                    {
                        m_Table->insert(U"\n");
                    }

                    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Backspace))
                    {
                        m_Table->erase();
                    }

                    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) &&
                        ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Z))
                    {
                        m_Table->undo();
                    }

                    insert_symbol_command();
                }

                int globalIndex = 0;
                ImRect symbolRectangle = ImRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos());
                ImVec2 symbolPosition  = ImGui::GetCursorScreenPos();
                ImVec2 symbolOrigin    = ImGui::GetCursorScreenPos();

                for (auto it = m_Table->symbols_begin(); it != m_Table->symbols_end(); it++, globalIndex++)                
                {
                    // retrieve symbol
                    std::string symbol =
                        Frenchie::Core::UTF::utf32_to_utf8(
                            std::u32string(1, *it));

                    // draw symbol
                    ImGui::GetWindowDrawList()->AddText(
                            symbolPosition,
                            IM_COL32(0, 255, 0, 255),
                            symbol.c_str()
                        );

                    ImVec2 symbolSize =
                        ImGui::GetCurrentContext()->Font->CalcTextSizeA(
                            ImGui::GetFontSize(),
                            FLT_MAX,
                            0.f,
                            symbol.c_str(),
                            NULL,
                            NULL
                        );

                    // highlight symbol
                    symbolRectangle = ImRect(symbolPosition, symbolPosition + symbolSize);

                    if(symbolRectangle.Contains(ImGui::GetMousePos()))
                    {
                        ImGui::GetWindowDrawList()->AddRectFilled(
                            symbolRectangle.Min,
                            symbolRectangle.Max,
                            IM_COL32(255, 0, 0, 128));

                        // update cursor position
                        if((ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left)  || 
                            ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Right) || 
                            ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Middle)))
                        {
                            m_Table->set_cursor_position(globalIndex);
                        }
                    }

                    if(m_Table->get_cursor_position() == globalIndex)
                    {
                        m_CursorGeometricalPosition = symbolRectangle.Min;
                    }

                    // update symbol position
                    symbolPosition =
                        symbol[0] == '\n' ?
                            ImVec2(symbolOrigin.x, symbolPosition.y + ImGui::GetFontSize()) :
                                symbolPosition + ImVec2(symbolSize.x, 0.f);
                }

                if(m_Table->get_cursor_position() == globalIndex)
                {
                    m_CursorGeometricalPosition =
                        ImVec2(symbolRectangle.Max.x, symbolRectangle.Min.y);
                }

                // draw cursor rectangle
                ImGui::GetWindowDrawList()->AddText(
                        m_CursorGeometricalPosition + ImVec2(2.f, 0.f) -
                            ImVec2(ImGui::GetCurrentContext()->Font->CalcTextSizeA(
                            ImGui::GetFontSize(),
                            FLT_MAX,
                            0.f,
                            "|",
                            NULL,
                            NULL
                        ).x, 0.f) * 0.5f,
                        IM_COL32(0, 255, 0, 255),
                        "|"
                    );

                /*
                for (auto it = m_Table->begin(); it != m_Table->end(); it++)
                {
                    for (int index = it->Start; index < it->Start + it->Length; index++, globalIndex++)
                    {
                        // retrieve symbol
                        std::string symbol =
                            Frenchie::Core::String::as_utf8(
                                std::wstring(1, it->Buffer->at(index)));

                        // draw symbol
                        ImGui::GetWindowDrawList()->AddText(
                                symbolPosition,
                                IM_COL32(0, 255, 0, 255),
                                symbol.c_str()
                            );

                        ImVec2 symbolSize =
                            ImGui::GetCurrentContext()->Font->CalcTextSizeA(
                                ImGui::GetFontSize(),
                                FLT_MAX,
                                0.f,
                                symbol.c_str(),
                                NULL,
                                NULL
                            );

                        // highlight symbol
                        symbolRectangle = ImRect(symbolPosition, symbolPosition + symbolSize);

                        if(symbolRectangle.Contains(ImGui::GetMousePos()))
                        {
                            ImGui::GetWindowDrawList()->AddRectFilled(
                                symbolRectangle.Min,
                                symbolRectangle.Max,
                                IM_COL32(255, 0, 0, 128));

                            // update cursor position
                            if((ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left)  || 
                                ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Right) || 
                                ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Middle)))
                            {
                                m_CursorGlobalPosition = globalIndex;
                            }
                        }

                        if(m_CursorGlobalPosition == globalIndex)
                        {
                            m_CursorGeometricalPosition = symbolRectangle.Min;
                            m_CursorLocalPosition       = index - it->Start;
                        }

                        // update symbol position
                        symbolPosition =
                            symbol[0] == '\n' ?
                                ImVec2(symbolOrigin.x, symbolPosition.y + ImGui::GetFontSize()) :
                                    symbolPosition + ImVec2(symbolSize.x, 0.f);
                    }

                    if(m_CursorGlobalPosition == globalIndex)
                    {
                        m_CursorGeometricalPosition =
                            ImVec2(symbolRectangle.Max.x, symbolRectangle.Min.y);
                    }

                    // draw cursor rectangle
                    ImGui::GetWindowDrawList()->AddText(
                            m_CursorGeometricalPosition + ImVec2(2.f, 0.f) -
                                ImVec2(ImGui::GetCurrentContext()->Font->CalcTextSizeA(
                                ImGui::GetFontSize(),
                                FLT_MAX,
                                0.f,
                                "|",
                                NULL,
                                NULL
                            ).x, 0.f) * 0.5f,
                            IM_COL32(0, 255, 0, 255),
                           "|"
                        );
                }
                */

                ImGui::EndChild();
            }

            // draw table
            ImGui::TableSetColumnIndex(1);

            ImGui::BeginChild("Buffers");
            {
                /*
                for (int i = 0; i < m_Table->get_lines_count(); i++)
                {
                    ImGui::TextUnformatted(fmt::format("line {} starts {} ends {}",
                        std::to_string(i),
                        std::to_string(m_Table->get_line_start_index(i)),
                        std::to_string(m_Table->get_line_end_index(i))).c_str());
                }                

                for(auto it = m_Table->begin(); it != m_Table->end(); it++)
                {
                    std::string pieceText = Frenchie::Core::String::as_utf8(
                        std::wstring(
                            &it->Buffer->at(it->Start), 
                            &it->Buffer->at(it->Start + it->Length))
                    );
                    
                    ImGui::TextUnformatted(fmt::format("{} {}", pieceText, it->LineBreaksCount).c_str());
                }
                */

                int symbolsCount = m_Table->symbols_count();

                for (int index = 0; index < symbolsCount; index++)
                {
                    auto itr = m_Table->get_piece_iterator_by_global_index(index);

                    std::string pieceText = Frenchie::Core::UTF::utf32_to_utf8(
                        std::u32string(
                            &itr.Iterator->Buffer->at(itr.Iterator->Start), 
                            &itr.Iterator->Buffer->at(itr.Iterator->Start + itr.Iterator->Length))
                    );
                    
                    ImGui::TextUnformatted(fmt::format("index {} iterator {} offset {}", index, pieceText, itr.Offset).c_str());
                }

                ImGui::EndChild();
            }

            ImGui::EndTable();
        }

        ImGui::EndChild();
    }

    ImGui::BeginChild(
        "Cursor",
        ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 100.f));
    {
        ImGui::TextUnformatted(
            fmt::format("GlobalIndex: {} LinesCount: {}", m_Table->get_cursor_position(), m_Table->lines_count()).c_str());
        ImGui::EndChild();
    }

    ImGui::End();
}

#endif

void PieceTableDrawer::frame_finish() 
{
}

bool PieceTableDrawer::allows_multiple_instances() const
{
    return false;
}

void PieceTableDrawer::document_insert_symbol_command()
{
	ImGuiIO& io = ImGui::GetIO();
	
	auto onCharPressed = [this](unsigned int c)
	{
		if(m_Table == nullptr) 
			return;

		// insert user input into a given line as wide character string
		Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
			[this, c]()
			{
				// retrieve user input in UTF-8 codec
				char utf8[5];
				int  count = Helpers::ImTextCharToUtf8(utf8, c);

                // insert symbol
				m_Table->insert(Frenchie::Core::UTF::utf8_to_utf32(std::string(utf8, count)));
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