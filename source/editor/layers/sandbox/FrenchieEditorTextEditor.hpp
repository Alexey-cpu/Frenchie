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

            struct Status
            {
                size_t TextBufferLength       = 0;
                size_t TextBufferLinesCount   = 0;
                size_t CursorPositionInBuffer = 0;
                size_t CursorLineNumber       = 0;
                ImVec2 CursorPositionInView   = ImVec2(0.f, 0.f);
            } m_Status;

            struct Cursor
            {
                size_t PositionInBuffer = 0;
                size_t LineNumber       = 0;
                ImVec2 PositionInView   = ImVec2(0.f, 0.f);
            };

            struct Timer
            {
                uint64_t LaunchTime {0};
                uint64_t CurrentTime{0};
                uint64_t Elapsed    {0};
            };

            // cursors
            Cursor m_NavigationCursor;
            Cursor m_EditorCursor;

            // timers
            Timer m_EditorCursorTimer;

            // buffer
            std::string m_TextBuffer;

            void draw_navigation_cursor();

            static ImRect calculate_row_rect(const size_t& _Row, const ImVec2& _NextLineOffset = ImVec2(), const ImVec2& _LineNumberOffset = ImVec2());
            static ImVec2 calculate_text_size(const char* _Begin, const char* _End = nullptr);
        };
    }
}