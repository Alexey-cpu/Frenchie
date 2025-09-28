#pragma once

// Core
#include <FrenchieCoreProcess.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// STL
#include <optional>

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

            struct Cursor
            {
                int  LineNumber     = 0;
                int  PositionInLine = 0;
                bool isMoving       = false;
            };

            struct Timer
            {
                uint64_t LaunchTime {0};
                uint64_t CurrentTime{0};
                uint64_t Elapsed    {0};
            };

            std::mutex m_Mutex;

            std::vector<std::wstring> m_Chunks;
            float m_ScrollY{0.f};
            float m_ScrollX{0.f};

            // cursors
            Cursor m_EditorCursor;

            // timers
            Timer m_EditorCursorTimer;

            void handle_key_events();

            static ImRect calculate_row_rect(const char* _Begin, const char* _End = nullptr);
            static ImRect calculate_row_rect(const std::wstring&);

            static ImVec2 calculate_text_size(const char* _Begin, const char* _End = nullptr);
            static ImVec2 calculate_text_size(const std::wstring&);
        };
    }
}