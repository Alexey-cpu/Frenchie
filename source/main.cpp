// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorLauncher.hpp>

#include <FrenchieEditorTextEditor.hpp>

int main(int argc, char *argv[])
{
    // std::wstring contents = L"/*/asdasd for(int i = 0; i < 10; i++)";

    // Frenchie::Editor::SyntaxHighlighter syntaxHighlighter;

    // std::vector<Frenchie::Editor::SyntaxHighlighter::RegularExpressionRule> m_Patterns = 
    // {
    //     Frenchie::Editor::SyntaxHighlighter::RegularExpressionRule(
    //         L".*", 
    //         IM_COL32(0, 255, 0, 255)),
    //     Frenchie::Editor::SyntaxHighlighter::RegularExpressionRule(
    //         L"alignas|alignof|and|and_eq|asm|auto|bitand|bitor|bool|break|case|catch|char|char8_t|char16_t|char32_t|class|compl|concept|const|consteval|constexpr|constinit|const_cast|continue|co_await|co_return|co_yield|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|not|not_eq|nullptr|operator|or|or_eq|private|protected|public|reflexpr|register|reinterpret_cast|requires|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|synchronized|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq", 
    //         IM_COL32(0, 0, 255, 255)),
    //     Frenchie::Editor::SyntaxHighlighter::RegularExpressionRule(
    //         LR"(/\*)", 
    //         IM_COL32(0, 255, 0, 255), 
    //         Frenchie::Editor::SyntaxHighlighter::RegularExpressionRule::MULTILINE_START),
    //     Frenchie::Editor::SyntaxHighlighter::RegularExpressionRule(
    //         LR"(\*/)", 
    //         IM_COL32(0, 255, 0, 255), 
    //         Frenchie::Editor::SyntaxHighlighter::RegularExpressionRule::MULTILINE_FINISH)
    // };

    // Frenchie::Editor::SyntaxHighlighter::regexEstimationResults matches = 
    //     syntaxHighlighter.highlight(contents, m_Patterns, 1);

    // for(auto&& match : matches)
    // {
    //     //std::cout << match.second.Match.Start << "\t" << match.second.Match.Finish << "\n";
    //     //std::cout << Frenchie::Core::String::as_utf8(Frenchie::Core::Regex::substring(contents, match.second.Match));
    //     std::cout << match.second.Match.Start << "\t" << match.second.Match.Finish << "\t" << Frenchie::Core::String::as_utf8(Frenchie::Core::Regex::substring(contents, match.second.Match)) << "\n";
    // }

    // std::cout << "\n";

    return Frenchie::Editor::Launcher::execute();
}