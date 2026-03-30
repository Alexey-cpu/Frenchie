#include <FrenchieEditorDebugLayer.hpp>

using namespace Frenchie::Editor;
using namespace Frenchie::Application;

// add sandbox elements into main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenIPCDebug : 
                public Frenchie::Application::Command::Registry<OpenIPCDebug, const CommandPayloads&>
            {
            public:

                OpenIPCDebug(const CommandPayloads& _Sender = CommandPayloads()) : 
                    Frenchie::Application::Command::Registry<OpenIPCDebug, const CommandPayloads&>(_Sender){}
                virtual ~OpenIPCDebug(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<DebugIPC>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Debug::IPC");
                }
            };

            const bool openIPCActionRegistry = OpenIPCDebug::registerFactory();
        }
    }
}

// DebugIPC
DebugIPC::DebugIPC() : Frenchie::Application::Layer(STRINGIFY(DebugIPC)){}
DebugIPC::~DebugIPC(){}

bool DebugIPC::awake()
{
    return true;
}

void DebugIPC::frame_update()
{
    ImGui::Begin(get_name().c_str());

    ImGui::InputText("Command", &m_Command);
    ImGui::InputText("Arguments", &m_Arguments);
    
    if(m_Process != nullptr) 
    {
        if(m_Process->alive()) 
            ImGui::TextUnformatted("Alive");
        else 
            ImGui::TextUnformatted("Dead");
    }
    else 
        ImGui::TextUnformatted("Nothing is running now...");
    
    if(ImGui::Button("Launch"))
    {
        m_Process = std::make_shared<Frenchie::Core::Process>(m_Command, m_Arguments);
    }

    ImGui::SameLine();
    
    if(ImGui::Button("Pause"))
    {
        if(m_Process != nullptr) m_Process->pause();
    }

    ImGui::SameLine();

    if(ImGui::Button("Continue"))
    {
        if(m_Process != nullptr) m_Process->resume();
    }

    ImGui::SameLine();
    
    if(ImGui::Button("Stop"))
    {
        if(m_Process != nullptr) m_Process->stop();
    }

    if(m_Process != nullptr) 
        m_Status = m_Process->status();

    ImGui::InputTextMultiline(
        "Status", 
        &m_Status, 
        ImGui::GetContentRegionAvail());

    ImGui::End();
}

bool DebugIPC::allows_multiple_instances() const
{
    return false;
}