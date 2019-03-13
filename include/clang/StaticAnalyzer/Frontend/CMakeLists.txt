set(LLVM_TARGET_DEFINITIONS CC1Options.td)
tablegen(LLVM Options.inc
  -gen-opt-parser-defs
  -I ${CLANG_SOURCE_DIR}/include
  -D GEN_STATIC_ANALYZER_OPTIONS_ONLY)
add_public_tablegen_target(ClangStaticAnalyzerOptions)
