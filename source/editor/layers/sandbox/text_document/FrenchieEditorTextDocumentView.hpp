#pragma once

// Core
#include <FrenchieCoreProcess.hpp>
#include <FrenchieCoreTextDocument.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieEditorTextDocumentHighlighter.hpp>

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

            enum Layers : int
            {
                BACKGROUND,
                DOCUMENT,
                CURSOR,
                COUNT
            };

            // info
            std::unique_ptr<Frenchie::Core::TextDocument> m_Table;

            SyntaxHighlighter m_Highlighter;

            std::vector<RegexRule> m_Patterns =
            {
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

                // multiline comment start
                RegexRule(
                    UR"(/\*)", 
                    IM_COL32(0, 255, 0, 255), 
                    RegexRule::MULTILINE_START),
                
                // multiline comment end
                RegexRule(
                    UR"(\*/)", 
                    IM_COL32(0, 255, 0, 255), 
                    RegexRule::MULTILINE_FINISH)
            };

            ImVec2 m_Scroll;
            ImRect m_ViewPort;
            int    m_Start;
            int    m_End;

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
            ImU32 calculate_color(const ImVec4& _Vector)
            {
                return IM_COL32(_Vector.x * 255.f, _Vector.y * 255.f, _Vector.z * 255.f, _Vector.w * 255.f);
            }
        };
    }
}