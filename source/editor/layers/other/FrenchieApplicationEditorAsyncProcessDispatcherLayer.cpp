#include <FrenchieApplicationEditorAsyncProcessDispatcherLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationProcessesLayer.hpp>
#include <FrenchieApplicationEditorConfigurationTranslatorLayer.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::Configuration;

ProcessDispatcher::ProcessDispatcher() : 
    Layer(Translator::translate("FRENCHIE_APPLICATION_EDITOR_ASYNC_PROCESS_DISPATCHER")){}
ProcessDispatcher::~ProcessDispatcher(){}

void ProcessDispatcher::frame_update()
{
    ImGui::Begin(get_name().c_str(), &m_Opened);
    {
        auto asyncProcessCount = 0;

        for(auto it = application()->begin(); it != application()->end(); ++it)
        {
            auto process = std::dynamic_pointer_cast<Process>(*it);

            if(process == nullptr)  // is not async process
                continue;

            asyncProcessCount++;

            if(ImGui::TreeNode(process->get_name().c_str()))
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
                    ImGui::TextUnformatted(Translator::translate("FRENCHIE_APPLICATION_EDITOR_ASYNC_PROCESS_DISPATCHER_NO_PROGRESS_DATA").c_str());
                }

                if(status != nullptr)
                {
                    ImGui::TextUnformatted(status->iprocess_status_request_status().c_str());
                }
                else 
                {
                    ImGui::TextUnformatted(Translator::translate("FRENCHIE_APPLICATION_EDITOR_ASYNC_PROCESS_DISPATCHER_NO_STATUS_DATA").c_str());
                }

                ImGui::TreePop();
            }
        }

        if(asyncProcessCount <= 0) 
            ImGui::TextUnformatted(Translator::translate("FRENCHIE_APPLICATION_EDITOR_ASYNC_PROCESS_DISPATCHER_NOTHING_IS_RUNNING_NOW").c_str());

        ImGui::End();
    }
}

bool ProcessDispatcher::allows_multiple_instances() const
{
    return false;
}