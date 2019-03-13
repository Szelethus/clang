set(LLVM_TARGET_DEFINITIONS Options.td)
tablegen(LLVM Options.inc -gen-opt-parser-defs -I ${CLANG_SOURCE_DIR}/include)
add_public_tablegen_target(ClangDriverOptions)
