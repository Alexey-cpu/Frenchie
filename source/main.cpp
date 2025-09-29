// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorLauncher.hpp>

#define PCRE2_CODE_UNIT_WIDTH 16
#include <pcre2.h>

struct RegularExpressionRule
{
    RegularExpressionRule(const std::wstring& Pattern, const unsigned int& _Color) : 
        Pattern(Pattern), Color(_Color){}

    std::wstring Pattern = std::wstring();
    unsigned int Color   = 0;
};

struct RegularExpressionEstimationResult
{
    RegularExpressionEstimationResult(){}

    RegularExpressionEstimationResult(const int& _Position, const int& _Length, const unsigned int& _Color) : 
        Position(_Position), Length(_Length), Color(_Color){}

    int          Position = 0;
    int          Length   = 0;
    unsigned int Color    = 0;
};

static std::map<int, RegularExpressionEstimationResult> colorize1(
    const std::wstring&                       _Contents, 
    const std::vector<RegularExpressionRule>& _Rules)
{
    std::map<int, RegularExpressionEstimationResult> colors;

    colors[0] = RegularExpressionEstimationResult(0, 0, 0);

    PCRE2_SPTR subject = (PCRE2_SPTR)_Contents.c_str();

    for(auto&& rule : _Rules)
    {
        PCRE2_SPTR pattern = (PCRE2_SPTR)rule.Pattern.c_str();

        int errorcode;
        PCRE2_SIZE erroroffset;

        pcre2_code *re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroroffset, NULL);

        if (re == NULL) 
        {
            std::cout << "Handle compilation error " << "\n";
            return  colors;
        }

        pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
        if (match_data == NULL) 
        {
            // Handle compilation error
            pcre2_code_free(re);
            return colors;
        }

        PCRE2_SIZE startoffset = 0;
        int rc;

        while ((rc = pcre2_match(re, subject, (PCRE2_SIZE)wcslen((wchar_t*)subject), startoffset, 0, match_data, NULL)) >= 0) 
        {
            PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);

            colors[(int)ovector[0]] = RegularExpressionEstimationResult((int)ovector[0], (int)(ovector[1] - ovector[0]), rule.Color);

            // Advance startoffset for the next search
            startoffset = ovector[1];

            if (ovector[0] == ovector[1]) 
            { // Handle zero-length matches to prevent infinite loops
                startoffset++;
            }
        }

        if (rc != PCRE2_ERROR_NOMATCH) 
        {
            // Handle other matching errors
        }

        pcre2_match_data_free(match_data);
        pcre2_code_free(re);
    }

    auto result = colors;

    // handle untill the end
    for(auto&& color : colors)
    {
        int source = color.second.Position + color.second.Length;
        int target = color.second.Position + color.second.Length;

        do
        {
            target++;
        } 
        while(colors.find(target) == colors.end() && target < (int)_Contents.size() - 1);

        if(source < (int)_Contents.size())
        {
            result[source] = RegularExpressionEstimationResult(
                source, 
                target - source, color.second.Color);
        }
    }

    return result;
}

int main(int argc, char *argv[])
{
    return Frenchie::Editor::Launcher::execute();
}