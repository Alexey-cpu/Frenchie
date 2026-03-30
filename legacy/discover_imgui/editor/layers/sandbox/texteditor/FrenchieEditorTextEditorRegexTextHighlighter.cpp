#include <FrenchieEditorTextEditorRegexTextHighlighter.hpp>

// STL
#include <set>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;

RegexTextHighlighter::RegexTextHighlighter(
    const std::vector<HighlightRule>& _Rules,
    const unsigned int&               _DefaultColor) :
    m_Rules(_Rules),
    m_DefaultColor(_DefaultColor){}

RegexTextHighlighter::~RegexTextHighlighter(){}

HighlightRulesEstimationResults RegexTextHighlighter::highlight(
    const std::u32string& _Contents,
    const int&            _LineNumber)
{
    HighlightRulesEstimationResults uniqueRanges;

    std::set<int> multilineOccurences;

    for(auto&& rule : m_Rules)
    {
        Frenchie::Core::String::IndexRanges matches =
            Frenchie::Core::String::regex_match(_Contents, rule.Pattern);

        int min = INT_MAX;
        int max = INT_MIN;

        for(auto&& match : matches)
        {
            for(int i = match.Start; i < match.Finish; i++)
            {
                uniqueRanges[i] = HighlightRuleEstimationResult(
                    Frenchie::Core::String::IndexRange(i, i + 1), 
                    rule.Color
                );
            }

            min = std::min<int>(min, match.Start);
            max = std::max<int>(max, match.Start);
        }

        if(rule.Type == HighlightRule::Type::MULTILINE_START && !matches.empty())
            multilineOccurences.insert(_LineNumber);

        if(rule.Type == HighlightRule::Type::MULTILINE_START)
        {
            if(matches.empty())
            {
                if(multilineOccurences.find(_LineNumber) == multilineOccurences.end())
                    m_MultilineStart.erase(_LineNumber);
            }
            else
            {
                m_MultilineStart[_LineNumber] =
                    HighlightRuleEstimationResult(Frenchie::Core::String::IndexRange(min), rule.Color);
            }
        }

        if(rule.Type == HighlightRule::Type::MULTILINE_FINISH && !matches.empty())
            multilineOccurences.insert(_LineNumber);

        if(rule.Type == HighlightRule::Type::MULTILINE_FINISH)
        {
            if(matches.empty())
            {
                if(multilineOccurences.find(_LineNumber) == multilineOccurences.end())
                    m_MultilineFinish.erase(_LineNumber);
            }
            else
            {
                m_MultilineFinish[_LineNumber] =
                    HighlightRuleEstimationResult(Frenchie::Core::String::IndexRange(max), rule.Color);
            }
        }
    }

    // add missing ranges
    for (int i = 0; i < (int)_Contents.size(); i++)
        uniqueRanges.insert({i, HighlightRuleEstimationResult(Frenchie::Core::String::IndexRange(i, i + 1), m_DefaultColor)});

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
    HighlightRulesEstimationResults optimized;

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

        optimized.insert({source, HighlightRuleEstimationResult(Frenchie::Core::String::IndexRange(source, target), sourceColor)});
        source = --target;
    }

    return optimized;
}