#pragma once

// Core
#include <FrenchieCoreProcess.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        class TextEditor : public Frenchie::Application::Layer
        {
        public:
            TextEditor();
            virtual ~TextEditor();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        protected:
            ImRect      m_TextContentsRect;
            ImRect      m_TextLineNumbersRect;
            std::string m_TextBuffer;

            // cursor attributes
            ImVec2   m_CursorPosition{ImVec2(0.f, 0.f)};
            uint64_t m_CursorShowStartTime{0};
            uint64_t m_CursorShowEndTime{0};
            uint64_t m_CursorShowElapsedTime{0};

            // service methods
            void handle_mouse_events();
        };
    }
}