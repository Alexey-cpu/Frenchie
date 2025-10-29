// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorLauncher.hpp>

int main(int argc, char *argv[])
{
    
    return Frenchie::Editor::Launcher::execute();
}

// //tree sitter
// #include <tree_sitter/api.h> // Include Tree-sitter C API header

// #include <FrenchieEditorCodeEditorTreeSitterCpp.hpp>
// #include <FrenchieEditorCodeEditorSyntaxHighlighter.hpp>

// int main(int argc, char *argv[])
// {
//     // discard
//     (void)argc;
//     (void)argv;
    
//     // create query
// 	uint32_t error_offset;
// 	TSQueryError error_type;
// 	TSQuery* m_Query = ts_query_new(
//         Frenchie::Editor::tree_sitter_cpp(),
//         Frenchie::Editor::tree_sitter_cpp_query,
//         (int)std::strlen(Frenchie::Editor::tree_sitter_cpp_query),
//         &error_offset, 
//         &error_type);

//     // create parser
//     TSParser* m_Parser = ts_parser_new();

//     // Set the language
//     ts_parser_set_language(m_Parser, Frenchie::Editor::tree_sitter_cpp());

//     // build AST
//     const std::string source_code = R"(
//     void fcn(int _A)
//     {
//         std::cout << R"(Hello World)\n";
//     }
    
//     int main()
//     {
//         fcn();
//         return 0;
//     }
//     )";

//     TSTree* tree = ts_parser_parse_string(
//         m_Parser,
//         NULL,
//         source_code.c_str(),
//         (int)source_code.length()
//     );

//     //  TSNode root_node = ts_tree_root_node(tree);
//     // TSTreeCursor cursor = ts_tree_cursor_new(root_node);

// 	TSQueryCursor *cursor = ts_query_cursor_new();
// 	ts_query_cursor_exec(cursor, m_Query, ts_tree_root_node(tree));

//     Frenchie::Editor::ISyntaxHighlighter::regexRulesEstimationResults uniqueRanges;

// 	TSQueryMatch match;
// 	while (ts_query_cursor_next_match(cursor, &match))
// 	{
// 		for (uint32_t i = 0; i < match.capture_count; ++i)
// 		{
// 			TSNode node = match.captures[i].node;
			
//             uint32_t name_length;
			
//             const char *name_ptr = ts_query_capture_name_for_id(m_Query, match.captures[i].index, &name_length);
// 			std::string name(name_ptr, name_length);

// 			// auto color = capture_colors.count(name) ? capture_colors.at(name) : m_Colors.Text; // Fallback to text color

// 			int start = ts_node_start_byte(node);
// 			int end   = ts_node_end_byte(node);

//             std::cout << name << "\t" << std::string(&source_code[start], &source_code[end]) << "\n";

//             // for (int i = start; i <= end; i++)
//             // {
//             //     uniqueRanges[i] = Frenchie::Editor::RegexRuleEstimationResult(
//             //         Frenchie::Core::String::Match(i, i + 1), 
//             //         color
//             //     );
//             // }
// 		}
// 	}

// // while (true) {
// //         TSNode current_node = ts_tree_cursor_current_node(&cursor);
// //         // Process the current_node (e.g., print its type, text, etc.)
// //         // printf("Node type: %s, Text: %.*s\n",
// //         //        ts_node_type(current_node),
// //         //        (int)(ts_node_end_byte(current_node) - ts_node_start_byte(current_node)),
// //         //        source_code + ts_node_start_byte(current_node));

// //             int start = ts_node_start_byte(node);
// // 			int end   = ts_node_end_byte(node);

// //         std::cout << ts_node_type(current_node) << 
// //             std::string(&source_code[start], &source_code[end]) << "\n";

// //         // Try to move to the first child
// //         if (ts_tree_cursor_goto_first_child(&cursor)) {
// //             continue;
// //         }

// //         // If no more children, try to move to the next sibling
// //         if (ts_tree_cursor_goto_next_sibling(&cursor)) {
// //             continue;
// //         }

// //         // If no more siblings, move up the tree until a sibling is found or the root is reached
// //         while (ts_tree_cursor_goto_parent(&cursor)) {
// //             if (ts_tree_cursor_goto_next_sibling(&cursor)) {
// //                 break;
// //             }
// //         }

// //         // If we're back at the root and no more siblings, traversal is complete
// //         if (ts_node_eq(ts_tree_cursor_current_node(&cursor), root_node) && !ts_tree_cursor_goto_next_sibling(&cursor)) {
// //             break;
// //         }
// //     }

//     // Clean up
//     ts_tree_delete(tree);
//     ts_query_cursor_delete(cursor);

//     return 0;
// }