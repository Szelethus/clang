//===--- CC1Options.td - Frontend options for Clang Static Analyzer -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//  This file defines frontend options for the Clang Static Analyzer.
//
//===----------------------------------------------------------------------===//

#ifdef GEN_STATIC_ANALYZER_OPTIONS_ONLY
include "clang/Driver/OptionBase.td"
#endif

let Flags = [CC1Option, NoDriverOption] in {

def analysis_UnoptimizedCFG : Flag<["-"], "unoptimized-cfg">,
  HelpText<"Generate unoptimized CFGs for all analyses">;
def analysis_CFGAddImplicitDtors : Flag<["-"], "cfg-add-implicit-dtors">,
  HelpText<"Add C++ implicit destructors to CFGs for all analyses">;

def analyzer_store : Separate<["-"], "analyzer-store">,
  HelpText<"Source Code Analysis - Abstract Memory Store Models">;
def analyzer_store_EQ : Joined<["-"], "analyzer-store=">, Alias<analyzer_store>;

def analyzer_constraints : Separate<["-"], "analyzer-constraints">,
  HelpText<"Source Code Analysis - Symbolic Constraint Engines">;
def analyzer_constraints_EQ : Joined<["-"], "analyzer-constraints=">,
  Alias<analyzer_constraints>;

def analyzer_output : Separate<["-"], "analyzer-output">,
  HelpText<"Source Code Analysis - Output Options">;
def analyzer_output_EQ : Joined<["-"], "analyzer-output=">,
  Alias<analyzer_output>;

def analyzer_purge : Separate<["-"], "analyzer-purge">,
  HelpText<"Source Code Analysis - Dead Symbol Removal Frequency">;
def analyzer_purge_EQ : Joined<["-"], "analyzer-purge=">, Alias<analyzer_purge>;

def analyzer_opt_analyze_headers : Flag<["-"], "analyzer-opt-analyze-headers">,
  HelpText<"Force the static analyzer to analyze functions defined in header files">;
def analyzer_opt_analyze_nested_blocks : Flag<["-"], "analyzer-opt-analyze-nested-blocks">,
  HelpText<"Analyze the definitions of blocks in addition to functions">;
def analyzer_display_progress : Flag<["-"], "analyzer-display-progress">,
  HelpText<"Emit verbose output about the analyzer's progress">;
def analyze_function : Separate<["-"], "analyze-function">,
  HelpText<"Run analysis on specific function (for C++ include parameters in name)">;
def analyze_function_EQ : Joined<["-"], "analyze-function=">, Alias<analyze_function>;
def trim_egraph : Flag<["-"], "trim-egraph">,
  HelpText<"Only show error-related paths in the analysis graph">;
def analyzer_viz_egraph_graphviz : Flag<["-"], "analyzer-viz-egraph-graphviz">,
  HelpText<"Display exploded graph using GraphViz">;
def analyzer_dump_egraph : Separate<["-"], "analyzer-dump-egraph">,
  HelpText<"Dump exploded graph to the specified file">;
def analyzer_dump_egraph_EQ : Joined<["-"], "analyzer-dump-egraph=">, Alias<analyzer_dump_egraph>;

def analyzer_inline_max_stack_depth : Separate<["-"], "analyzer-inline-max-stack-depth">,
  HelpText<"Bound on stack depth while inlining (4 by default)">;
def analyzer_inline_max_stack_depth_EQ : Joined<["-"], "analyzer-inline-max-stack-depth=">,
  Alias<analyzer_inline_max_stack_depth>;

def analyzer_inlining_mode : Separate<["-"], "analyzer-inlining-mode">,
  HelpText<"Specify the function selection heuristic used during inlining">;
def analyzer_inlining_mode_EQ : Joined<["-"], "analyzer-inlining-mode=">, Alias<analyzer_inlining_mode>;

def analyzer_disable_retry_exhausted : Flag<["-"], "analyzer-disable-retry-exhausted">,
  HelpText<"Do not re-analyze paths leading to exhausted nodes with a different strategy (may decrease code coverage)">;

def analyzer_max_loop : Separate<["-"], "analyzer-max-loop">,
  HelpText<"The maximum number of times the analyzer will go through a loop">;
def analyzer_stats : Flag<["-"], "analyzer-stats">,
  HelpText<"Print internal analyzer statistics.">;

def analyzer_checker : Separate<["-"], "analyzer-checker">,
  HelpText<"Choose analyzer checkers to enable">,
  ValuesCode<[{
    const char *Values =
    #define GET_CHECKERS
    #define CHECKER(FULLNAME, CLASS, HT, DOC_URI, IS_HIDDEN)  FULLNAME ","
    #include "clang/StaticAnalyzer/Checkers/Checkers.inc"
    #undef GET_CHECKERS
    #define GET_PACKAGES
    #define PACKAGE(FULLNAME)  FULLNAME ","
    #include "clang/StaticAnalyzer/Checkers/Checkers.inc"
    #undef GET_PACKAGES
    ;
  }]>;
def analyzer_checker_EQ : Joined<["-"], "analyzer-checker=">,
  Alias<analyzer_checker>;

def analyzer_disable_checker : Separate<["-"], "analyzer-disable-checker">,
  HelpText<"Choose analyzer checkers to disable">;
def analyzer_disable_checker_EQ : Joined<["-"], "analyzer-disable-checker=">,
  Alias<analyzer_disable_checker>;

def analyzer_disable_all_checks : Flag<["-"], "analyzer-disable-all-checks">,
  HelpText<"Disable all static analyzer checks">;

def analyzer_checker_help : Flag<["-"], "analyzer-checker-help">,
  HelpText<"Display the list of analyzer checkers that are available">;

def analyzer_checker_help_hidden : Flag<["-"], "analyzer-checker-help-hidden">,
  HelpText<"Display the list of analyzer checkers that are available, "
           "including modeling checkers">;

def analyzer_config_help : Flag<["-"], "analyzer-config-help">,
  HelpText<"Display the list of -analyzer-config options">;

def analyzer_list_enabled_checkers : Flag<["-"], "analyzer-list-enabled-checkers">,
  HelpText<"Display the list of enabled analyzer checkers">;

def analyzer_config : Separate<["-"], "analyzer-config">,
  HelpText<"Choose analyzer options to enable">;

def analyzer_config_compatibility_mode : Separate<["-"], "analyzer-config-compatibility-mode">,
  HelpText<"Don't emit errors on invalid analyzer-config inputs">;

def analyzer_config_compatibility_mode_EQ : Joined<["-"], "analyzer-config-compatibility-mode=">,
  Alias<analyzer_config_compatibility_mode>;

} // let Flags = [CC1Option, NoDriverOption]
