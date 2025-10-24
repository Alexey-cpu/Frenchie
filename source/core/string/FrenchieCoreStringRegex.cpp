#include <FrenchieCoreStringRegex.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Core::String;

// PCRE
#define PCRE2_CODE_UNIT_WIDTH 0
#include <pcre2.h>

Frenchie::Core::String::Matches 
Frenchie::Core::String::regex_match(const std::string& _Contents, const std::string _Pattern)
{
    Frenchie::Core::String::Matches result;

    if(_Contents.empty()) 
        return result;

    PCRE2_SPTR8 subject = (PCRE2_SPTR8)_Contents.c_str();
    PCRE2_SPTR8 pattern = (PCRE2_SPTR8)_Pattern.c_str();

    int errorcode;
    PCRE2_SIZE erroroffset;

    pcre2_code_8 *re = pcre2_compile_8(pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroroffset, NULL);

    if (re == NULL)
        return result;

    pcre2_match_data_8 *match_data = pcre2_match_data_create_from_pattern_8(re, NULL);
    
    if (match_data == NULL) 
    {
        // Handle compilation error
        pcre2_code_free_8(re);
        return result;
    }

    PCRE2_SIZE startoffset = 0;
    int rc;

    while ((rc = pcre2_match_8(re, subject, (PCRE2_SIZE)strlen((char*)subject), startoffset, 0, match_data, NULL)) >= 0) 
    {
        PCRE2_SIZE *ovector = pcre2_get_ovector_pointer_8(match_data);

        result.push_back({(int)ovector[0], (int)ovector[1]});

        // Advance startoffset for the next search
        startoffset = ovector[1];

        if (ovector[0] == ovector[1]) 
        { // Handle zero-length matches to prevent infinite loops
            startoffset++;
        }
    }

    if(rc != PCRE2_ERROR_NOMATCH){}

    pcre2_match_data_free_8(match_data);
    pcre2_code_free_8(re);

    return result;
}

Frenchie::Core::String::Matches 
Frenchie::Core::String::regex_match(const std::u16string& _Contents, const std::u16string _Pattern)
{
    Frenchie::Core::String::Matches result;

    if(_Contents.empty()) 
        return result;

    PCRE2_SPTR16 subject = (PCRE2_SPTR16)_Contents.c_str();
    PCRE2_SPTR16 pattern = (PCRE2_SPTR16)_Pattern.c_str();

    int errorcode;
    PCRE2_SIZE erroroffset;

    pcre2_code_16 *re = pcre2_compile_16(pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroroffset, NULL);

    if (re == NULL)
        return result;

    pcre2_match_data_16 *match_data = pcre2_match_data_create_from_pattern_16(re, NULL);
    
    if (match_data == NULL) 
    {
        // Handle compilation error
        pcre2_code_free_16(re);
        return result;
    }

    PCRE2_SIZE startoffset = 0;
    int rc;

    while ((rc = pcre2_match_16(re, subject, (PCRE2_SIZE)_Contents.size() + 1, startoffset, 0, match_data, NULL)) >= 0) 
    {
        PCRE2_SIZE *ovector = pcre2_get_ovector_pointer_16(match_data);

        result.push_back(Frenchie::Core::String::Match((int)ovector[0], (int)ovector[1]));

        // Advance startoffset for the next search
        startoffset = ovector[1];

        if (ovector[0] == ovector[1]) 
        { // Handle zero-length matches to prevent infinite loops
            startoffset++;
        }
    }

    if(rc != PCRE2_ERROR_NOMATCH){}

    pcre2_match_data_free_16(match_data);
    pcre2_code_free_16(re);

    return result;
}

Frenchie::Core::String::Matches 
Frenchie::Core::String::regex_match(const std::u32string& _Contents, const std::u32string _Pattern)
{
    Frenchie::Core::String::Matches result;

    if(_Contents.empty()) 
        return result;

    PCRE2_SPTR32 subject = (PCRE2_SPTR32)_Contents.c_str();
    PCRE2_SPTR32 pattern = (PCRE2_SPTR32)_Pattern.c_str();

    int errorcode;
    PCRE2_SIZE erroroffset;

    pcre2_code_32 *re = pcre2_compile_32(pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroroffset, NULL);

    if (re == NULL)
        return result;

    pcre2_match_data_32 *match_data = pcre2_match_data_create_from_pattern_32(re, NULL);
    
    if (match_data == NULL) 
    {
        // Handle compilation error
        pcre2_code_free_32(re);
        return result;
    }

    PCRE2_SIZE startoffset = 0;
    int rc;

    while ((rc = pcre2_match_32(re, subject, (PCRE2_SIZE)_Contents.size() + 1, startoffset, 0, match_data, NULL)) >= 0) 
    {
        PCRE2_SIZE *ovector = pcre2_get_ovector_pointer_32(match_data);

        result.push_back({(int)ovector[0], (int)ovector[1]});

        // Advance startoffset for the next search
        startoffset = ovector[1];

        if (ovector[0] == ovector[1]) 
        { // Handle zero-length matches to prevent infinite loops
            startoffset++;
        }
    }

    if(rc != PCRE2_ERROR_NOMATCH){}

    pcre2_match_data_free_32(match_data);
    pcre2_code_free_32(re);

    return result;
}

std::string Frenchie::Core::String::regex_match_substring(const std::string& _Origin, const Frenchie::Core::String::Match& _Match)
{
    return std::string(&_Origin[_Match.Start], &_Origin[_Match.Finish]);
}

std::u16string Frenchie::Core::String::regex_match_substring(const std::u16string& _Origin, const Frenchie::Core::String::Match& _Match)
{
    return std::u16string(&_Origin[_Match.Start], &_Origin[_Match.Finish]);
}

std::u32string Frenchie::Core::String::regex_match_substring(const std::u32string& _Origin, const Frenchie::Core::String::Match& _Match)
{
    return std::u32string(&_Origin[_Match.Start], &_Origin[_Match.Finish]);
}