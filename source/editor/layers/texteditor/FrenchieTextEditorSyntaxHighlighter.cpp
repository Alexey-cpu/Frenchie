#include <FrenchieTextEditorSyntaxHighlighter.hpp>

using namespace Frenchie::Core;

using namespace Frenchie::TextEditor;

void SyntaxHighlighter::preprocessTextBlock(
    const std::vector<std::wstring>& _Chunks, 
    int                              _Start, 
    int                              _End, 
    const std::vector<RegexRule>&    _Rules)
{
    for(int lineNumber = _Start; lineNumber < std::min<int>(_End, (int)_Chunks.size()); lineNumber++)
    {
        for(auto&& rule : _Rules)
        {
            if(rule.Type != RegexRule::Type::MULTILINE_START && 
                rule.Type != RegexRule::Type::MULTILINE_FINISH) continue;

            Frenchie::Core::Regex::Matches matches = 
                Frenchie::Core::Regex::match(_Chunks[lineNumber], rule.Pattern);

            //m_MultilineColor = rule.Color;

            int min = INT_MAX;
            int max = INT_MIN;

            for(auto&& match : matches)
            {
                min = std::min<int>(min, match.Start);
                max = std::max<int>(max, match.Start);
            }

            if(rule.Type == RegexRule::Type::MULTILINE_START)
            {
                m_MultilineStart[lineNumber] = 
                    RegexResult(Frenchie::Core::Regex::Match(min), rule.Color);

                if(matches.empty()) 
                    m_MultilineStart.erase(lineNumber);
            }

            if(rule.Type == RegexRule::Type::MULTILINE_FINISH)
            {
                m_MultilineFinish[lineNumber] = 
                    RegexResult(Frenchie::Core::Regex::Match(max), rule.Color);

                if(matches.empty()) 
                    m_MultilineFinish.erase(lineNumber);
            }
        }
    }
}

SyntaxHighlighter::regexEstimationResults SyntaxHighlighter::processTextLine(
    const std::wstring&           _Contents, 
    const std::vector<RegexRule>& _Rules,
    const unsigned int&           _DefaultColor, 
    const int&                    _LineNumber)
{
    bool resetMultilineRule = false;

    regexEstimationResults uniqueRanges;

    for(auto&& rule : _Rules)
    {
        auto matches = Frenchie::Core::Regex::match(_Contents, rule.Pattern);

        for(auto&& match : matches)
        {
            for(int i = match.Start; i < match.Finish; i++)
            {
                uniqueRanges[i] = RegexResult(
                    Frenchie::Core::Regex::Match(i, i + 1), 
                    rule.Color
                );
            }
        }
    }

    // add missing ranges
    for (int i = 0; i < (int)_Contents.size(); i++)
        uniqueRanges.insert({i, RegexResult(Frenchie::Core::Regex::Match(i, i + 1), _DefaultColor)});

    // multiline recoloring
    for(auto&& uniqueRange : uniqueRanges)
    {
        std::map<int, int> multilineRanges;

        for(auto multilineStart : m_MultilineStart)
        {
            multilineRanges[multilineStart.first] = INT_MAX;

            for(auto multilineFinish : m_MultilineFinish)
            {
                if(multilineFinish.first < multilineStart.first) 
                    continue;

                multilineRanges[multilineStart.first] = 
                    std::min<int>(multilineRanges[multilineStart.first], multilineFinish.first);
            }
        }

        for(auto&& multilineRange : multilineRanges)
        {
            auto color = m_MultilineStart[multilineRange.first].Color;

            if(_LineNumber == multilineRange.first && 
                uniqueRange.second.Match.Start >= m_MultilineStart[multilineRange.first].Match.Start)
            {
                uniqueRange.second.Color = color;
                break;
            }
            else if(_LineNumber == multilineRange.second && 
                    uniqueRange.second.Match.Start <= m_MultilineFinish[multilineRange.second].Match.Start)
            {
                uniqueRange.second.Color = color;
                break;
            }
            else if(_LineNumber > multilineRange.first && 
                    _LineNumber < multilineRange.second)
            {
                uniqueRange.second.Color = color;
                break;
            }
        }
    }

    // optimize
    regexEstimationResults optimized;

    int source = 0;
    int target = 0;

    for (source = 0; source < (int)_Contents.size(); source++)
    {
        auto sourceColor = uniqueRanges[source].Color;

        for (target = source + 1; target < (int)_Contents.size(); target++)
        {
            auto targetColor = uniqueRanges[target].Color;

            if(sourceColor != targetColor)
                break;
        }

        optimized.insert({source, RegexResult(Frenchie::Core::Regex::Match(source, target), sourceColor)});
        source = --target;
    }

    return optimized;
}