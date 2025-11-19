#pragma once

#include <FrenchieApplicationLayer.hpp>

#include <imgui.h>

namespace Frenchie
{
    namespace Application
    {
        class ImguiDemo : public Layer
        {
        public:
            ImguiDemo() : Layer("ImguiDemo"){}
            virtual ~ImguiDemo(){}

            virtual void frame_update()override
            {
                ImGuiIO& io = ImGui::GetIO();

                if (show_demo_window)
                    ImGui::ShowDemoWindow(&show_demo_window);

                {
                    static float f = 0.0f;
                    static int counter = 0;

                    ImGui::Begin("Hello, world!");

                    ImGui::Text("This is some useful text.");
                    ImGui::Checkbox("Demo Window", &show_demo_window);
                    ImGui::Checkbox("Another Window", &show_another_window);

                    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
                    ImGui::ColorEdit3("clear color", (float*)&clear_color);

                    if (ImGui::Button("Button"))
                        counter++;
                    ImGui::SameLine();
                    ImGui::Text("counter = %d", counter);

                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                    ImGui::End();
                }

                if (show_another_window)
                {
                    ImGui::Begin("Another Window", &show_another_window);
                    ImGui::Text("Hello from another window!");
                    if (ImGui::Button("Close Me"))
                        show_another_window = false;
                    ImGui::End();
                }
            }

        protected:
            bool show_another_window = false;
            bool show_demo_window    = false;

            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        };
    }
}