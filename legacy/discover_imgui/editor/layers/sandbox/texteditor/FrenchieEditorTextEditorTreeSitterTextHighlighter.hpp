#pragma once

#include <FrenchieEditorTextEditorITextHighlighter.hpp>

// TREE-SITTER
#include <tree_sitter/api.h>

namespace Frenchie
{
    namespace Editor
    {
        class TreeSitterTextHighlighter : public ITextHighlighter
		{
		public:

            typedef std::map<std::string, unsigned int> ThemeColors;

            TreeSitterTextHighlighter(
                TSLanguage*                                _Language,
                const std::string&                         _Query,
                const std::map<std::string, unsigned int>& _Colors);
            
            virtual ~TreeSitterTextHighlighter();

			HighlightRulesEstimationResults highlight(
				const std::u32string& _Contents,
                const int&            _LineNumber) override;

        protected:
            TSLanguage*                         m_Language;
            TSParser*                           m_Parser;
            TSQuery*                            m_Query;
            std::map<std::string, unsigned int> m_Colors;
		};
    }
}