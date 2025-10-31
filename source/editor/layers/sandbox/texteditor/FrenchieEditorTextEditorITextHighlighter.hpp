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
        struct HighlightRuleEstimationResult
        {
            HighlightRuleEstimationResult(
                const Frenchie::Core::String::IndexRange& _Match = Frenchie::Core::String::IndexRange(), 
                const unsigned int&                  _Color = 0) : 
                Match(_Match), Color(_Color){}

            Frenchie::Core::String::IndexRange Match;
            unsigned int                  Color  = 0;
        };

        typedef std::map<int, HighlightRuleEstimationResult> HighlightRulesEstimationResults;

        class ITextHighlighter
        {
        public:

            ITextHighlighter(){}
            virtual ~ITextHighlighter(){}

            virtual HighlightRulesEstimationResults highlight(
				const std::u32string& _Contents,
                const int&            _LineNumber) = 0;
        };
    }
}