#pragma once

#include <FrenchieApplicationLayer.hpp>

#include <imgui.h>

namespace Frenchie
{
    namespace Application
    {
        namespace Imgui
        {
            class TestLayer : public Layer
            {
            public:

                TestLayer(const std::string& _Name = "TestLayer") : Layer(_Name){}
                virtual ~TestLayer(){}

                virtual bool awake() override
                {
                    return true;
                }
                
                virtual void frame_start() override
                {
                }
                
                virtual void frame_update() override
                {
                    ImGui::Begin(get_name().c_str());
                    ImGui::End();
                }
                
                virtual void frame_finish() override
                {
                }
                
                virtual void finish() override
                {
                }
            };
        }
    }
};