#pragma once

// Core
#include <FrenchieCoreProcess.hpp>
#include <FrenchieCoreTextDocument.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieEditorTextEditorITextHighlighter.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        class FrameCounter : public Frenchie::Application::Layer
        {
        public:
            
            FrameCounter(
                const int&         _Threshold, 
                const std::string& _Name = STRINGIFY(FrameCounter)) :
                Frenchie::Application::Layer(_Name),
                m_Threshold(_Threshold){}
            
            virtual ~FrameCounter(){}

            // getters
            int get_frames_count() const
            {
                return m_FramesCount;
            }

            virtual bool awake() override
            {
                m_FramesCount = 0;
                return true;
            }

            virtual void frame_update() override
            {
                m_FramesCount = ++m_FramesCount % m_Threshold;
            }

        protected:
            int  m_Threshold  {0};
            int  m_FramesCount{0};
        };

        class TextDocumentSeclection final
        {
        public:
            TextDocumentSeclection();
            ~TextDocumentSeclection();

            void select(const int& _Index);
            void clear();

            int first() const;
            int last() const;
            int size() const;

            bool empty() const;
            bool is_selected(const int& _Index) const;

        private:
            int m_First{INT_MAX};
            int m_Last {INT_MIN};
        };

        class TextDocumentView : public Frenchie::Application::Layer
        {
        public:
            TextDocumentView(const std::shared_ptr<Frenchie::Core::TextDocument>& = nullptr);
            virtual ~TextDocumentView();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;
            virtual bool allows_multiple_instances() const override;

        protected:

            enum Layers : int
            {
                BACKGROUND,
                DOCUMENT,
                CURSOR,
                COUNT
            };

            // info
            std::shared_ptr<Frenchie::Core::TextDocument> m_TextDocument      {nullptr};
            std::shared_ptr<FrameCounter>                 m_CursorFrameCounter{nullptr};
            std::shared_ptr<ITextHighlighter>             m_Highlighter       {nullptr};
            TextDocumentSeclection                        m_Selection;

            ImVec2 m_Scroll{ImVec2()};
            ImRect m_ViewPort{ImRect()};
            int    m_MaxWidth{0};
            int    m_Start{0};
            int    m_End{0};

            // commands
            void document_insert_symbol_command();
            void document_erase_symbol_command();
            void document_move_cursor_left_command();
            void document_move_cursor_right_command();
            void document_move_cursor_down_command();
            void document_move_cursor_up_command();
            void document_undo_command();
            void document_redo_command();

            void editor_copy_command();
            void editor_paste_command();

            void editor_clear_selection_command();
            void editor_select_command(const int& _Index);

            // callbacks
            void on_character_pressed(const unsigned int&);
        };
    }
}