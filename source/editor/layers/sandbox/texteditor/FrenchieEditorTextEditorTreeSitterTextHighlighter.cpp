#include <FrenchieEditorTextEditorTreeSitterTextHighlighter.hpp>

// Core
#include <FrenchieCoreStringUnicode.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;

TreeSitterTextHighlighter::TreeSitterTextHighlighter(
    TSLanguage*        _Language,
    const std::string&                         _Query,
    const std::map<std::string, unsigned int>& _Colors) :
    m_Language(_Language),
    m_Parser(ts_parser_new()),
    m_Colors(_Colors)
{
	uint32_t     error_offset;
	TSQueryError error_type;

	m_Query = ts_query_new(
        m_Language,
        _Query.c_str(),
        (int)std::strlen(_Query.c_str()),
        &error_offset,
        &error_type
    );
}

TreeSitterTextHighlighter::~TreeSitterTextHighlighter()
{
    ts_query_delete(m_Query);
    ts_parser_delete(m_Parser);
}

HighlightRulesEstimationResults TreeSitterTextHighlighter::highlight(
    const std::u32string& _Contents,
    const int&            _LineNumber)
{
    // Set the language
    ts_parser_set_language(m_Parser, m_Language);

    // build AST
    const std::string source_code =
        Frenchie::Core::String::convert_utf32_to_utf8(_Contents);

    TSTree* tree = ts_parser_parse_string(
        m_Parser,
        NULL,
        source_code.c_str(),
        (int)source_code.length()
    );

	TSQueryCursor *cursor = ts_query_cursor_new();
	ts_query_cursor_exec(cursor, m_Query, ts_tree_root_node(tree));

    HighlightRulesEstimationResults uniqueRanges;

	TSQueryMatch match;
	while (ts_query_cursor_next_match(cursor, &match))
	{
		for (uint32_t i = 0; i < match.capture_count; ++i)
		{
			TSNode node = match.captures[i].node;
			
            uint32_t name_length;
			
            const char *name_ptr = ts_query_capture_name_for_id(m_Query, match.captures[i].index, &name_length);
			std::string name(name_ptr, name_length);

			auto color = m_Colors.count(name) ? m_Colors.at(name) : 0xFFFFFFFF; // Fallback to text color

			int start = ts_node_start_byte(node);
			int end   = ts_node_end_byte(node);

            for (int i = start; i <= end; i++)
            {
                uniqueRanges[i] = Frenchie::Editor::HighlightRuleEstimationResult(
                    Frenchie::Core::String::IndexRange(i, i + 1), 
                    color
                );
            }
		}
	}

    // add missing ranges
    for (int i = 0; i < (int)_Contents.size(); i++)
        uniqueRanges.insert({i, HighlightRuleEstimationResult(Frenchie::Core::String::IndexRange(i, i + 1), 0xFFFFFFFF)});

    // Clean up
    ts_tree_delete(tree);
    ts_query_cursor_delete(cursor);

    return uniqueRanges;
}