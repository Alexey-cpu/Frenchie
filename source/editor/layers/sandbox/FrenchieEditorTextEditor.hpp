#pragma once

// Core
#include <FrenchieCoreProcess.hpp>
#include <FrenchieCoreRegex.hpp>

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
		class SyntaxHighlighter
		{
		public:
			struct RegularExpressionRule
			{
				RegularExpressionRule(const std::wstring& Pattern = std::wstring(), const unsigned int& _Color = 0) : 
					Pattern(Pattern), Color(_Color){}

				std::wstring Pattern = std::wstring();
				unsigned int Color   = 0;
			};

			struct RegularExpressionEstimationResult
			{
				RegularExpressionEstimationResult(
					const Frenchie::Core::Regex::Match& _Match = Frenchie::Core::Regex::Match(), 
					const unsigned int&                 _Color = 0) : 
					Match(_Match), Color(_Color){}

				Frenchie::Core::Regex::Match Match;
				unsigned int                 Color  = 0;
			};

			typedef std::map<int, RegularExpressionEstimationResult> regexEstimationResults;

			static regexEstimationResults highlight(
				const std::wstring&                       _Contents, 
				const std::vector<RegularExpressionRule>& _Rules, 
				unsigned int                              _DefaultColor = 1)
			{
				regexEstimationResults colors = 
					{{0, RegularExpressionEstimationResult(Frenchie::Core::Regex::Match(), _DefaultColor)}};

				for(auto&& rule : _Rules)
				{
					auto matches = Frenchie::Core::Regex::match(_Contents, rule.Pattern);

					for(auto&& match : matches)
					{
						colors[match.Start] = 
							RegularExpressionEstimationResult(match, rule.Color);
					}
				}

				regexEstimationResults results;

				for(auto&& color : colors)
				{
					int source = color.second.Match.Finish;
					int target = color.second.Match.Finish;

					do
					{
						target++;
					} 
					while(colors.find(target) == colors.end() && target < (int)_Contents.size());

					if(source < (int)_Contents.size())
					{
						results[source] = 
							RegularExpressionEstimationResult(
								Frenchie::Core::Regex::Match(source, target),
								_DefaultColor
							);
					}
				}

				for(auto&& result : results) 
					colors[result.first] = result.second;

				return colors;
			}
		};

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
            ImRect                    m_TextViewPort;
            ImRect                    m_LineNumbersViewPort;
            int                       m_CurrentlyHoveredLine = 0;

            std::vector<SyntaxHighlighter::RegularExpressionRule> m_Patterns = 
            {
                SyntaxHighlighter::RegularExpressionRule(
                    L"alignas|alignof|and|and_eq|asm|auto|bitand|bitor|bool|break|case|catch|char|char8_t|char16_t|char32_t|class|compl|concept|const|consteval|constexpr|constinit|const_cast|continue|co_await|co_return|co_yield|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|not|not_eq|nullptr|operator|or|or_eq|private|protected|public|reflexpr|register|reinterpret_cast|requires|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|synchronized|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq", 
                    IM_COL32(255, 0, 0, 255))
            };

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
            static ImU32  calculate_color(const ImVec4&);
        };
    }
}