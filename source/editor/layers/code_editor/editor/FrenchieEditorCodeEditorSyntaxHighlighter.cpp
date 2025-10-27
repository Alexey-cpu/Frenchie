#include <FrenchieEditorCodeEditorSyntaxHighlighter.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;

#include <set>

SyntaxHighlighter::regexRulesEstimationResults SyntaxHighlighter::highlight(
    const std::u32string&         _Contents, 
    const std::vector<RegexRule>& _Rules,
    const unsigned int&           _DefaultColor, 
    const int&                    _LineNumber)
{
    regexRulesEstimationResults uniqueRanges;

    std::set<int> multilineOccurences;

    for(auto&& rule : _Rules)
    {
        auto matches = Frenchie::Core::String::regex_match(_Contents, rule.Pattern);

        int min = INT_MAX;
        int max = INT_MIN;

        for(auto&& match : matches)
        {
            for(int i = match.Start; i < match.Finish; i++)
            {
                uniqueRanges[i] = RegexRuleEstimationResult(
                    Frenchie::Core::String::Match(i, i + 1), 
                    rule.Color
                );
            }

            min = std::min<int>(min, match.Start);
            max = std::max<int>(max, match.Start);
        }

        if(rule.Type == RegexRule::Type::MULTILINE_START && !matches.empty())
            multilineOccurences.insert(_LineNumber);

        if(rule.Type == RegexRule::Type::MULTILINE_START)
        {
            if(matches.empty())
            {
                if(multilineOccurences.find(_LineNumber) == multilineOccurences.end())
                    m_MultilineStart.erase(_LineNumber);
            }
            else
            {
                m_MultilineStart[_LineNumber] =
                    RegexRuleEstimationResult(Frenchie::Core::String::Match(min), rule.Color);
            }
        }

        if(rule.Type == RegexRule::Type::MULTILINE_FINISH && !matches.empty())
            multilineOccurences.insert(_LineNumber);

        if(rule.Type == RegexRule::Type::MULTILINE_FINISH)
        {
            if(matches.empty())
            {
                if(multilineOccurences.find(_LineNumber) == multilineOccurences.end())
                    m_MultilineFinish.erase(_LineNumber);
            }
            else
            {
                m_MultilineFinish[_LineNumber] =
                    RegexRuleEstimationResult(Frenchie::Core::String::Match(max), rule.Color);
            }
        }
    }

    // add missing ranges
    for (int i = 0; i < (int)_Contents.size(); i++)
        uniqueRanges.insert({i, RegexRuleEstimationResult(Frenchie::Core::String::Match(i, i + 1), _DefaultColor)});

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

    // color scheme range optimization
    regexRulesEstimationResults optimized;

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

        optimized.insert({source, RegexRuleEstimationResult(Frenchie::Core::String::Match(source, target), sourceColor)});
        source = --target;
    }

    return optimized;
}