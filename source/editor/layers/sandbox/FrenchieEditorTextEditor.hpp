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
                int    Line     = 0;
                int    Column   = 0;
                ImVec2 Position = ImVec2(0.f, 0.f);
            };

            struct Timer
            {
                uint64_t LaunchTime {0};
                uint64_t CurrentTime{0};
                uint64_t Elapsed    {0};
            };

            std::mutex                m_Mutex;
            std::vector<std::wstring> m_Chunks;
            Cursor                    m_Cursor;
            Timer                     m_Timer;
            ImVec2                    m_Scroll;
            ImVec2                    m_RowSize;
            ImRect                    m_TextViewPort;
            int                       m_CurrentlyHoveredLine = 0;

            // commands
            void moveCursorLeft();
            void moveCursorRight();
            void moveCursorDown();
            void moveCursorUp();
            void moveNextLine();
            void removeSymbol();
            void handleUserInput();

            // drawers
            void drawTextLineNumbers();
            void drawTextContents();

            // static API
            static ImVec2 calculate_text_size(const char* _Begin, const char* _End = nullptr);
            static ImVec2 calculate_text_size(const std::wstring&);
        };
    }
}