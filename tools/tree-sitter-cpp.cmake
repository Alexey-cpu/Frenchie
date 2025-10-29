# add_subdirectory("tools/tree-sitter-cpp/")

add_library(tree-sitter-cpp STATIC
  tools/tree-sitter-cpp/src/scanner.c
  tools/tree-sitter-cpp/src/parser.c
)
target_include_directories(tree-sitter-cpp PRIVATE
  tools/tree-sitter-cpp/src/
)