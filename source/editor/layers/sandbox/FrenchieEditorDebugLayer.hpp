#pragma once

// Core
#include <FrenchieCoreProcess.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>

namespace Frenchie
{
    namespace Editor
    {
        class DebugIPC : public Frenchie::Application::Layer
        {
        public:
            DebugIPC();
            virtual ~DebugIPC();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        protected:

            std::shared_ptr<Frenchie::Core::Process> m_Process = nullptr;

            std::string m_Command   = std::string();
            std::string m_Arguments = std::string();
            std::string m_Status    = std::string();
        };
    }
}