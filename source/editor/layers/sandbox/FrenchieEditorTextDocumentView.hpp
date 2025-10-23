#pragma once

// Core
#include <FrenchieCoreProcess.hpp>
#include <FrenchieCoreTextDocument.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationSynchronousTimer.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        class TextDocumentView : public Frenchie::Application::Layer
        {
        public:
            TextDocumentView();
            virtual ~TextDocumentView();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;
            virtual bool allows_multiple_instances() const override;

        protected:

            // info
            std::unique_ptr<Frenchie::Core::TextDocument> m_Table;
            ImVec2 m_CursorGeometricalPosition;

            // service methods
            ImVec2 calculate_text_size(const std::string&) const;

            // commands
            void document_insert_symbol_command();
            void document_erase_symbol_command();
            void document_move_cursor_left_command();
            void document_move_cursor_right_command();
            void document_move_cursor_down_command();
            void document_move_cursor_up_command();
            void document_undo_command();
            void document_redo_command();

            // callbacks
            void on_character_ressed(const unsigned int&);
        };
    }
}