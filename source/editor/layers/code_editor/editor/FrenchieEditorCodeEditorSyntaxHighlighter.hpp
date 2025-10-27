#pragma once

// Core
#include <FrenchieCoreStringRegex.hpp>

// STL
#include <map>
#include <vector>
#include <string>

namespace Frenchie
{
    namespace Editor
    {
        struct RegexRule
        {
            enum Type
            {
                DEFAULT,
                MULTILINE_START,
                MULTILINE_FINISH,
            };

            RegexRule(
                const std::u32string& _Pattern = std::u32string(), 
                const unsigned int&   _Color   = 0, 
                Type                  _Type    = Type::DEFAULT) : 
                Pattern(_Pattern), Color(_Color), Type(_Type){}

            std::u32string Pattern = std::u32string();
            unsigned int   Color   = 0;
            Type           Type    = Type::DEFAULT;
        };

        struct RegexRuleEstimationResult
        {
            RegexRuleEstimationResult(
                const Frenchie::Core::String::Match& _Match = Frenchie::Core::String::Match(), 
                const unsigned int&                  _Color = 0) : 
                Match(_Match), Color(_Color){}

            Frenchie::Core::String::Match Match;
            unsigned int                  Color  = 0;
        };

        class SyntaxHighlighter
		{
		public:

			typedef std::map<int, RegexRuleEstimationResult> regexRulesEstimationResults;

			regexRulesEstimationResults highlight(
				const std::u32string&         _Contents, 
				const std::vector<RegexRule>& _Rules,
				const unsigned int&           _DefaultColor, 
                const int&                    _LineNumber);

        protected:
            std::map<int, RegexRuleEstimationResult> m_MultilineStart;
            std::map<int, RegexRuleEstimationResult> m_MultilineFinish;
		};
    }
}