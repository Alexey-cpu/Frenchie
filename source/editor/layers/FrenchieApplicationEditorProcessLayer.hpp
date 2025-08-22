#pragma once

#include <FrenchieApplicationLayer.hpp>

// IMGUI
#include <imgui.h>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class Process : public Layer
            {
            public:
                Process(){}
                virtual ~Process(){}

                virtual void frame_update() override
                {   
                    // load something
                    float progress   = execute();
                    float percangate = (int)(progress * 100.f);

                    // show progress
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("100%").x);
                    ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "");
                    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
                    ImGui::TextUnformatted(fmt::format("{} %", percangate).c_str());

                    // close self when finished
                    if(percangate >= 100) 
                        close();
                }

                virtual float execute() = 0;
            };
        }
    }
}