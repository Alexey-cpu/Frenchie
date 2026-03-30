#pragma once

#include <string>
#include <vector>

namespace Frenchie
{
    namespace Core
    {
        namespace String
        {
            struct IndexRange
            {
                IndexRange(int _Start = 0, int _Finish = 0) : 
                    Start(_Start), Finish(_Finish){}

                int Start {0};
                int Finish{0};
            };

            typedef std::vector<IndexRange> IndexRanges;
            
            IndexRanges regex_match(const std::string& _Contents, const std::string _Pattern);
            IndexRanges regex_match(const std::u16string& _Contents, const std::u16string _Pattern);
            IndexRanges regex_match(const std::u32string& _Contents, const std::u32string _Pattern);

            std::string regex_match_substring(const std::string& _Origin, const IndexRange& _Match);
            std::u16string regex_match_substring(const std::u16string& _Origin, const IndexRange& _Match);
            std::u32string regex_match_substring(const std::u32string& _Origin, const IndexRange& _Match);
        }
    }
}