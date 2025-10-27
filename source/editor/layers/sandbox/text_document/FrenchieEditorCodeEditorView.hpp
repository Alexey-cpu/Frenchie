#pragma once

// Core
#include <FrenchieCoreProcess.hpp>
#include <FrenchieCoreTextDocument.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationFrameCounter.hpp>

// Editor
#include <FrenchieEditorSyntaxHighlighter.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
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

            bool is_selected(const int& _Index) const;
            bool empty() const;

        private:
            int m_First = INT_MAX;
            int m_Last  = INT_MIN;
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
            std::shared_ptr<Frenchie::Core::TextDocument>        m_TextDocument{nullptr};
            std::shared_ptr<Frenchie::Application::FrameCounter> m_CursorFrameCounter{nullptr};

            TextDocumentSeclection m_Selection;
            SyntaxHighlighter      m_Highlighter;

            std::vector<RegexRule> m_HighlighterRules =
            {
                // numbers
                RegexRule(
                    U"[+-]?(\\d+(\\.\\d*)?|\\.\\d+)", 
                    IM_COL32(156, 156, 82, 255)),

                // variables
                RegexRule(
                    UR"(\b[A-Za-z_]\w*\b)",
                    IM_COL32(66, 122, 168, 255)),

                // keywords
                RegexRule(
                    UR"(alignof|alignas|asm|auto|class|consteval|constinit|constexpr|const_cast|decltype|delete|dynamic_cast|enum|explicit|false|final|friend|inline|namespace|new|noexcept|nullptr|operator|override|private|protected|public|reinterpret_cast|sizeof|static_assert|static_cast|struct|template|this|true|typedef|typeid|typename|union|using|virtual|and|and_eq|bitand|bitor|compl|not|not_eq|or|or_eq|xor|xor_eq|concept|requires|import|module|export)", 
                    IM_COL32(10, 8, 156, 255)),

                // modifiers
                RegexRule(
                    UR"(const|extern|mutable|register|static|thread_local|volatile)", 
                    IM_COL32(10, 8, 156, 255)),

                // controlflow
                RegexRule(
                    UR"(break|case|catch|continue|default|do|else|for|goto|if|return|switch|throw|try|while|co_await|co_return|co_yield)", 
                    IM_COL32(146, 8, 156, 255)),

                // types
                RegexRule(
                    UR"(bool|char|char8_t|char16_t|char32_t|double|float|int|long|short|signed|unsigned|void|int8_t|int16_t|int32_t|int64_t|uint8_t|uint16_t|uint32_t|uint64_t|int_least8_t|int_least16_t|int_least32_t|int_least64_t|uint_least8_t|uint_least16_t|uint_least32_t|uint_least64_t|int_fast8_t|int_fast16_t|int_fast32_t|int_fast64_t|uint_fast8_t|uint_fast16_t|uint_fast32_t|uint_fast64_t|size_t|size_t|wchar_t|intptr_t|uintptr_t|intmax_t|uintmax_t|ptrdiff_t|sig_atomic_t|wint_t|va_list|FILE|fpos_t|time_t)", 
                    IM_COL32(10, 8, 156, 255)),

                // attributes
                RegexRule(
                    UR"(\[\[(.*)\]\])", 
                    IM_COL32(57, 247, 5, 255)),

                // preprocessor directives
                RegexRule(
                    UR"(\#.*)", 
                    IM_COL32(61, 45, 1, 255)),

                // single line comment
                RegexRule(
                    UR"(//.*)", 
                    IM_COL32(0, 255, 0, 255)),

                // multiline patterns

                // comments
                RegexRule(
                    UR"(/\*)",
                    IM_COL32(0, 255, 0, 255),
                    RegexRule::MULTILINE_START),

                RegexRule(
                    UR"(\*/)",
                    IM_COL32(0, 255, 0, 255),
                    RegexRule::MULTILINE_FINISH),

                // strings
                RegexRule(
                    UR"(\"[^"])",
                    IM_COL32(61, 45, 1, 255),
                    RegexRule::MULTILINE_START),

                RegexRule(
                   UR"([^"]\")",
                    IM_COL32(61, 45, 1, 255),
                    RegexRule::MULTILINE_FINISH)
            };

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