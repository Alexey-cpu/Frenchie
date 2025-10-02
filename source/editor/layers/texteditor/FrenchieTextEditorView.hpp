#pragma once

// Core
#include <FrenchieCoreReference.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationSynchronousTimer.hpp>

// TextEditor
#include <FrenchieTextEditorTextModel.hpp>
#include <FrenchieTextEditorSyntaxHighlighter.hpp>

using namespace Frenchie::TextEditor;

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

            enum Layers : int
            {
                BACKGROUND,
                TEXT,
                CURSOR,
                COUNT
            };

            std::mutex m_Mutex;
            ImVec2     m_ScrollPos;
            ImRect     m_TextRect;
            ImRect     m_LineNumbersRect;
            ImVec2     m_CursorPosition;
            int        m_CurrentlyHoveredLine = 0;

            SyntaxHighlighter m_SyntaxHighlighter;

            Frenchie::Core::Reference<
                Frenchie::Application::SynchronousTimer<std::chrono::milliseconds>> m_CursorTimer;

            std::shared_ptr<TextEditorModel> m_TextModel{std::make_shared<TextEditorModel>()};

            std::vector<RegexRule> m_Patterns = 
            {
                RegexRule(
                    L"alignas|alignof|and|and_eq|asm|auto|bitand|bitor|bool|break|case|catch|char|char8_t|char16_t|char32_t|class|compl|concept|const|consteval|constexpr|constinit|const_cast|continue|co_await|co_return|co_yield|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|not|not_eq|nullptr|operator|or|or_eq|private|protected|public|reflexpr|register|reinterpret_cast|requires|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|synchronized|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq", 
                    IM_COL32(0, 0, 255, 255)),
                RegexRule(
                    L"for", 
                    IM_COL32(255, 0, 0, 255)),

                RegexRule(
                    LR"([\(\)\{\}\[\]])", 
                    IM_COL32(0, 255, 0, 255)),

                // single line comment
                RegexRule(
                    L"//.*", 
                    IM_COL32(0, 255, 0, 255)),

                // multiline comment start
                RegexRule(
                    LR"(/\*)", 
                    IM_COL32(0, 255, 0, 255), 
                    RegexRule::MULTILINE_START),
                
                // multiline comment end
                RegexRule(
                    LR"(\*/)", 
                    IM_COL32(0, 255, 0, 255), 
                    RegexRule::MULTILINE_FINISH)
            };

            // commands
            void move_cursor_left_command();
            void move_cursor_right_command();
            void move_cursor_down_command();
            void move_cursor_up_command();
            void move_next_line_command();
            void move_back_commnad();
            void insert_symbol_command();

            // drawers
            void draw_text_line_numbers();
            void draw_text_contents();
            void draw_status_panel();

            // static API
            static ImVec2 calculate_text_size(const char* _Begin, const char* _End = nullptr);
            static ImVec2 calculate_text_size(const std::wstring&);
            static ImU32  calculate_color(const ImVec4&);
        };
    }
}