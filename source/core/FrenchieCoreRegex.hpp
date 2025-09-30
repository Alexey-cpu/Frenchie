#pragma once

#include <string>
#include <vector>

namespace Frenchie
{
    namespace Core
    {
        namespace Regex
        {
            struct Match
            {
                Match(int _Start = 0, int _Finish = 0) : 
                    Start(_Start), Finish(_Finish){}

                int Start {0};
                int Finish{0};
            };

            typedef std::vector<Match> Matches;
            
            Matches match(const std::string& _Contents, const std::string _Pattern);
            Matches match(const std::wstring& _Contents, const std::wstring _Pattern);

            std::string substring(const std::string& _Origin, const Match& _Match);
            std::wstring substring(const std::wstring& _Origin, const Match& _Match);
        }
    }
}