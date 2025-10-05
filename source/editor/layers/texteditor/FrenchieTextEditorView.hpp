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

            SyntaxHighlighter m_SyntaxHighlighter;

            Frenchie::Core::Reference<
                Frenchie::Application::SynchronousTimer<std::chrono::milliseconds>> m_CursorTimer;

            std::shared_ptr<TextEditorModel> m_TextModel{std::make_shared<TextEditorModel>()};

            std::vector<RegexRule> m_Patterns =
            {
                // keywords
                RegexRule(
                    LR"(alignof|alignas|asm|auto|class|consteval|constinit|constexpr|const_cast|decltype|delete|dynamic_cast|enum|explicit|false|final|friend|inline|namespace|new|noexcept|nullptr|operator|override|private|protected|public|reinterpret_cast|sizeof|static_assert|static_cast|struct|template|this|true|typedef|typeid|typename|union|using|virtual|and|and_eq|bitand|bitor|compl|not|not_eq|or|or_eq|xor|xor_eq|concept|requires|import|module|export)", 
                    IM_COL32(10, 8, 156, 255)),

                // types
                RegexRule(
                    LR"(bool|char|char8_t|char16_t|char32_t|double|float|int|long|short|signed|unsigned|void|int8_t|int16_t|int32_t|int64_t|uint8_t|uint16_t|uint32_t|uint64_t|int_least8_t|int_least16_t|int_least32_t|int_least64_t|uint_least8_t|uint_least16_t|uint_least32_t|uint_least64_t|int_fast8_t|int_fast16_t|int_fast32_t|int_fast64_t|uint_fast8_t|uint_fast16_t|uint_fast32_t|uint_fast64_t|size_t|size_t|wchar_t|intptr_t|uintptr_t|intmax_t|uintmax_t|ptrdiff_t|sig_atomic_t|wint_t|va_list|FILE|fpos_t|time_t)", 
                    IM_COL32(10, 8, 156, 255)),

                // modifiers
                RegexRule(
                    LR"(const|extern|mutable|register|static|thread_local|volatile)", 
                    IM_COL32(10, 8, 156, 255)),

                // controlflow
                RegexRule(
                    LR"(break|case|catch|continue|default|do|else|for|goto|if|return|switch|throw|try|while|co_await|co_return|co_yield)", 
                    IM_COL32(146, 8, 156, 255)),

                // attributes
                RegexRule(
                    LR"(\[\[(.*)\]\])", 
                    IM_COL32(57, 247, 5, 255)),

                // preprocessor directives
                RegexRule(
                    LR"(\#.*)", 
                    IM_COL32(61, 45, 1, 255)),

                // single line comment
                RegexRule(
                    LR"(//.*)", 
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
            void clear_selection_command();

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