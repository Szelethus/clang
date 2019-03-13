clang_tablegen(Options.inc
  -gen-clang-sa-options
  -I ${LLVM_SOURCE_DIR}/include
  -I ${CLANG_SOURCE_DIR}/include
  -D GEN_STATIC_ANALYZER_OPTIONS_ONLY
  
  SOURCE CC1Options.td
  
  TARGET ClangStaticAnalyzerOptions
  )
