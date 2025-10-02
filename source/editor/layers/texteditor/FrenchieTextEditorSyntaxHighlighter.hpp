#pragma once

// Core
#include <FrenchieCoreRegex.hpp>

// STL
#include <map>
#include <vector>
#include <string>

namespace Frenchie
{
    namespace TextEditor
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
                const std::wstring& _Pattern = std::wstring(), 
                const unsigned int& _Color   = 0, 
                Type                _Type    = Type::DEFAULT) : 
                Pattern(_Pattern), Color(_Color), Type(_Type){}

            std::wstring Pattern = std::wstring();
            unsigned int Color   = 0;
            Type         Type    = Type::DEFAULT;
        };

        struct RegexResult
        {
            RegexResult(
                const Frenchie::Core::Regex::Match& _Match = Frenchie::Core::Regex::Match(), 
                const unsigned int&                 _Color = 0) : 
                Match(_Match), Color(_Color){}

            Frenchie::Core::Regex::Match Match;
            unsigned int                 Color  = 0;
        };

        class SyntaxHighlighter
		{
		public:

			typedef std::map<int, RegexResult> regexEstimationResults;

            void preprocessTextBlock(
                const std::vector<std::wstring>& _Chunks, 
                int                              _Start, 
                int                              _End, 
                const std::vector<RegexRule>&    _Rules);

			regexEstimationResults processTextLine(
				const std::wstring&           _Contents, 
				const std::vector<RegexRule>& _Rules,
				const unsigned int&           _DefaultColor, 
                const int&                    _LineNumber);

        protected:
            std::map<int, RegexResult> m_MultilineStart;
            std::map<int, RegexResult> m_MultilineFinish;
		};
    }
}