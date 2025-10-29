add_library(tree-sitter STATIC
  tools/tree-sitter/lib/src/lib.c
  tools/tree-sitter/lib/src/query.c 
)
target_include_directories(tree-sitter PUBLIC
  tools/tree-sitter/lib/include
  tools/tree-sitter/lib/src
)