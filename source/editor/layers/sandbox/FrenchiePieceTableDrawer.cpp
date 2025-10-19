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

// PieceTableDrawer
PieceTableDrawer::PieceTableDrawer() : Frenchie::Application::Layer(STRINGIFY(PieceTableDrawer)){}
PieceTableDrawer::~PieceTableDrawer(){}

bool PieceTableDrawer::awake()
{
    std::wstring textBuffer = L"HelloWorld";

	for (size_t j = 0; j < 1e5; j++)
	{
		for (size_t i = 0; i < 1; i++)
			textBuffer.append(std::to_wstring(j)).append(L"\t").append(L"for(int i = 0; i < 10; i++)");

		textBuffer.append(L"\n");
	}

    table = std::make_unique<TextModel>(textBuffer);

    return true;
}

void PieceTableDrawer::frame_update()
{
    ImGui::Begin(get_name().c_str());

    ImGui::SetNextWindowContentSize(ImVec2(32000, m_Height));

    int start = 0;
    int end   = 0;

    if(ImGui::BeginChild(
        "Content",
        ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 100.f),
        ImGuiChildFlags_::ImGuiChildFlags_None,
        ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysVerticalScrollbar |
        ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysHorizontalScrollbar))
    {
        ImVec2 scroll   = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
		ImRect viewport = ImRect(ImGui::GetCursorScreenPos()  + scroll, 
                                  ImGui::GetCursorScreenPos() + scroll + ImGui::GetWindowSize());

        start = (int)(scroll.y / ImGui::GetFontSize());
        end   = start + (int)(viewport.GetSize().y / ImGui::GetFontSize());

        table->request(start, end);

        m_Height = (table->get_data().Rows + 2) * ImGui::GetFontSize();

        ImGui::SetCursorPos(ImVec2(0.f, ImGui::GetScrollY()));

        ImGui::TextUnformatted(Frenchie::Core::String::as_utf8(table->get_data().Text).c_str());

        ImGui::EndChild();
    }

    if(ImGui::BeginChild(
    "Status",
    ImVec2(ImGui::GetContentRegionAvail().x, 100.f),
    ImGuiChildFlags_::ImGuiChildFlags_None,
    ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysVerticalScrollbar |
    ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysHorizontalScrollbar))
    {
        ImGui::TextUnformatted(
            fmt::format("Start: {} End: {} Length: {}", start, end, (end - start) * 4096).c_str()
        );

        ImGui::EndChild();
    }

    ImGui::End();
}

void PieceTableDrawer::frame_finish() 
{
}

bool PieceTableDrawer::allows_multiple_instances() const
{
    return false;
}