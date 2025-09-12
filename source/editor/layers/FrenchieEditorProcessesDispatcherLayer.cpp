#include <FrenchieEditorProcessesDispatcherLayer.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationAsynchronousProcessesLayer.hpp>

// Editor
#include <FrenchieEditorConfigurationTranslatorLayer.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::Configuration;

// add to main menu
class OpenDispatcherAction : 
    public Frenchie::Application::Command::Registry<OpenDispatcherAction, void*>
{
public:

    OpenDispatcherAction(void* _Sender = nullptr) : 
        Frenchie::Application::Command::Registry<OpenDispatcherAction, void*>(_Sender){}
    virtual ~OpenDispatcherAction(){}

    // Frenchie::Application::Command
    virtual void execute() override
    {
        Frenchie::Application::application()->push_layer<ProcessesDispatcher>()->show();
    }

    // Command::TRegistryType
    static std::string factory_id()
    {
        return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Windows::Asynchronous process dispatcher");
    }
};

ProcessesDispatcher::ProcessesDispatcher() : 
    Layer(Translator::translate("Asynchronous process dispatcher")){}
ProcessesDispatcher::~ProcessesDispatcher(){}

void ProcessesDispatcher::frame_update()
{
    // update name
    set_name(Translator::translate("Asynchronous process dispatcher"));

    // draw
    ImGui::Begin(fmt::format("{}###Asynchronous process dispatcher", get_name()).c_str(), &m_Opened);
    {
        auto asyncProcessCount = 0;
        auto asyncProcessQueue = Frenchie::Application::ProcessQueue::instance();


        for(auto it = asyncProcessQueue->begin(); it != asyncProcessQueue->end(); ++it)
        {
            auto process = *it;

            if(process == nullptr)  // is not async process
                continue;

            asyncProcessCount++;

            ImGui::PushID(process->get_uuid().to_string().c_str());

            if(ImGui::TreeNode(process->get_name().empty() ? process->get_uuid().to_string().c_str() : process->get_name().c_str()))
            {
                auto status   = std::dynamic_pointer_cast<IProcessStatus>(process);
                auto progress = std::dynamic_pointer_cast<IProcessProgress>(process);

                if(progress != nullptr)
                {
                    // calculate progress percantage
                    float progressData = progress->iprocess_progress_request_progress();
                    int   percantage   = (int)(progressData * 100.f);

                    // show progress
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("100%").x);
                    ImGui::ProgressBar(progressData, ImVec2(0.0f, 0.0f), "");
                    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
                    ImGui::TextUnformatted(fmt::format("{} %", percantage).c_str());
                }
                else
                {
                    ImGui::TextUnformatted(Translator::translate("There is no information about the process progress...").c_str());
                }

                if(status != nullptr)
                {
                    ImGui::TextUnformatted(status->iprocess_status_request_status().c_str());
                }
                else 
                {
                    ImGui::TextUnformatted(Translator::translate("There is no information about the process status...").c_str());
                }

                ImGui::TreePop();
            }

            ImGui::PopID();
        }

        if(asyncProcessCount <= 0) 
            ImGui::TextUnformatted(Translator::translate("Nothing is processing asynchronously now ...").c_str());

        ImGui::End();
    }
}

bool ProcessesDispatcher::allows_multiple_instances() const
{
    return false;
}