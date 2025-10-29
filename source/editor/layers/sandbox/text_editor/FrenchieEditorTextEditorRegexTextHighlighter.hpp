#pragma once

#include <FrenchieEditorTextEditorITextHighlighter.hpp>

namespace Frenchie
{
    namespace Editor
    {
        class RegexTextHighlighter : public ITextHighlighter
		{
		public:

            struct HighlightRule
            {
                enum Type
                {
                    DEFAULT,
                    MULTILINE_START,
                    MULTILINE_FINISH,
                };

                HighlightRule(
                    const std::u32string& _Pattern = std::u32string(), 
                    const unsigned int&   _Color   = 0, 
                    Type                  _Type    = Type::DEFAULT) : 
                    Pattern(_Pattern), Color(_Color), Type(_Type){}

                std::u32string Pattern = std::u32string();
                unsigned int   Color   = 0;
                Type           Type    = Type::DEFAULT;
            };

            RegexTextHighlighter(
                const std::vector<HighlightRule>& _Rules,
                const unsigned int&               _DefaultColor);
            virtual ~RegexTextHighlighter();

			HighlightRulesEstimationResults highlight(
				const std::u32string& _Contents,
                const int&            _LineNumber) override;

        protected:

            const std::vector<HighlightRule> m_Rules;
            const unsigned int&              m_DefaultColor;


            std::map<int, HighlightRuleEstimationResult> m_MultilineStart;
            std::map<int, HighlightRuleEstimationResult> m_MultilineFinish;
		};
    }
}