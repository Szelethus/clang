//==- DebugCheckers.cpp - Debugging Checkers ---------------------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//  This file defines checkers that display debugging information.
//
//===----------------------------------------------------------------------===//

#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h"
#include "clang/Analysis/Analyses/Dominators.h"
#include "clang/Analysis/Analyses/LiveVariables.h"
#include "clang/Analysis/CallGraph.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/AnalysisManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExplodedGraph.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExprEngine.h"
#include "llvm/Support/Process.h"

using namespace clang;
using namespace ento;

//===----------------------------------------------------------------------===//
// DominatorsTreeDumper
//===----------------------------------------------------------------------===//

namespace {
class DominatorsTreeDumper : public Checker<check::ASTCodeBody> {
public:
  void checkASTCodeBody(const Decl *D, AnalysisManager& mgr,
                        BugReporter &BR) const {
    if (AnalysisDeclContext *AC = mgr.getAnalysisDeclContext(D)) {
      DominatorTree dom;
      dom.buildDominatorTree(*AC);
      dom.dump();
    }
  }
};
}

void ento::registerDominatorsTreeDumper(CheckerManager &mgr) {
  mgr.registerChecker<DominatorsTreeDumper>();
}

bool ento::shouldRegisterDominatorsTreeDumper(const LangOptions &LO) {
  return true;
}

//===----------------------------------------------------------------------===//
// LiveVariablesDumper
//===----------------------------------------------------------------------===//

namespace {
class LiveVariablesDumper : public Checker<check::ASTCodeBody> {
public:
  void checkASTCodeBody(const Decl *D, AnalysisManager& mgr,
                        BugReporter &BR) const {
    if (LiveVariables* L = mgr.getAnalysis<LiveVariables>(D)) {
      L->dumpBlockLiveness(mgr.getSourceManager());
    }
  }
};
}

void ento::registerLiveVariablesDumper(CheckerManager &mgr) {
  mgr.registerChecker<LiveVariablesDumper>();
}

bool ento::shouldRegisterLiveVariablesDumper(const LangOptions &LO) {
  return true;
}

//===----------------------------------------------------------------------===//
// LiveStatementsDumper
//===----------------------------------------------------------------------===//

namespace {
class LiveStatementsDumper : public Checker<check::ASTCodeBody> {
public:
  void checkASTCodeBody(const Decl *D, AnalysisManager& Mgr,
                        BugReporter &BR) const {
    if (LiveVariables *L = Mgr.getAnalysis<RelaxedLiveVariables>(D))
      L->dumpStmtLiveness(Mgr.getSourceManager());
  }
};
}

void ento::registerLiveStatementsDumper(CheckerManager &mgr) {
  mgr.registerChecker<LiveStatementsDumper>();
}

bool ento::shouldRegisterLiveStatementsDumper(const LangOptions &LO) {
  return true;
}

//===----------------------------------------------------------------------===//
// CFGViewer
//===----------------------------------------------------------------------===//

namespace {
class CFGViewer : public Checker<check::ASTCodeBody> {
public:
  void checkASTCodeBody(const Decl *D, AnalysisManager& mgr,
                        BugReporter &BR) const {
    if (CFG *cfg = mgr.getCFG(D)) {
      cfg->viewCFG(mgr.getLangOpts());
    }
  }
};
}

void ento::registerCFGViewer(CheckerManager &mgr) {
  mgr.registerChecker<CFGViewer>();
}

bool ento::shouldRegisterCFGViewer(const LangOptions &LO) {
  return true;
}

//===----------------------------------------------------------------------===//
// CFGDumper
//===----------------------------------------------------------------------===//

namespace {
class CFGDumper : public Checker<check::ASTCodeBody> {
public:
  void checkASTCodeBody(const Decl *D, AnalysisManager& mgr,
                        BugReporter &BR) const {
    PrintingPolicy Policy(mgr.getLangOpts());
    Policy.TerseOutput = true;
    Policy.PolishForDeclaration = true;
    D->print(llvm::errs(), Policy);

    if (CFG *cfg = mgr.getCFG(D)) {
      cfg->dump(mgr.getLangOpts(),
                llvm::sys::Process::StandardErrHasColors());
    }
  }
};
}

void ento::registerCFGDumper(CheckerManager &mgr) {
  mgr.registerChecker<CFGDumper>();
}

bool ento::shouldRegisterCFGDumper(const LangOptions &LO) {
  return true;
}

//===----------------------------------------------------------------------===//
// CallGraphViewer
//===----------------------------------------------------------------------===//

namespace {
class CallGraphViewer : public Checker< check::ASTDecl<TranslationUnitDecl> > {
public:
  void checkASTDecl(const TranslationUnitDecl *TU, AnalysisManager& mgr,
                    BugReporter &BR) const {
    CallGraph CG;
    CG.addToCallGraph(const_cast<TranslationUnitDecl*>(TU));
    CG.viewGraph();
  }
};
}

void ento::registerCallGraphViewer(CheckerManager &mgr) {
  mgr.registerChecker<CallGraphViewer>();
}

bool ento::shouldRegisterCallGraphViewer(const LangOptions &LO) {
  return true;
}

//===----------------------------------------------------------------------===//
// CallGraphDumper
//===----------------------------------------------------------------------===//

namespace {
class CallGraphDumper : public Checker< check::ASTDecl<TranslationUnitDecl> > {
public:
  void checkASTDecl(const TranslationUnitDecl *TU, AnalysisManager& mgr,
                    BugReporter &BR) const {
    CallGraph CG;
    CG.addToCallGraph(const_cast<TranslationUnitDecl*>(TU));
    CG.dump();
  }
};
}

void ento::registerCallGraphDumper(CheckerManager &mgr) {
  mgr.registerChecker<CallGraphDumper>();
}

bool ento::shouldRegisterCallGraphDumper(const LangOptions &LO) {
  return true;
}

//===----------------------------------------------------------------------===//
// ConfigDumper
//===----------------------------------------------------------------------===//

namespace {
class ConfigDumper : public Checker< check::EndOfTranslationUnit > {

  static constexpr size_t getNonCheckerConfigCount() {
#define ANALYZER_OPTION_DEPENDS_ON_USER_MODE(TYPE, NAME, CMDFLAG, DESC,        \
                                             SHALLOW_VAL, DEEP_VAL)            \
  ANALYZER_OPTION(TYPE, NAME, CMDFLAG, DESC, SHALLOW_VAL)

  return 0
#define ANALYZER_OPTION(TYPE, NAME, CMDFLAG, DESC, DEFAULT_VAL)                \
           + 1
#include "clang/StaticAnalyzer/Core/AnalyzerOptions.def"
              ;
#undef ANALYZER_OPTION
#undef ANALYZER_OPTION_DEPENDS_ON_USER_MODE
  }

  static std::string getStr(unsigned U) {
    return std::to_string(U);
  }

  static std::string getStr(bool B) {
    return B ? "true" : "false";
  }

  static std::string getStr(StringRef Str) {
    return Str.empty() ? "\"\"" : Str;
  }

public:
  void checkEndOfTranslationUnit(const TranslationUnitDecl *TU,
                                 AnalysisManager& mgr,
                                 BugReporter &BR) const {
    llvm::errs() << "[config]\n";

    const AnalyzerOptions &AnOpts = mgr.getAnalyzerOptions();

    std::string Options[] = {
#define ANALYZER_OPTION_DEPENDS_ON_USER_MODE(TYPE, NAME, CMDFLAG, DESC,        \
                                             SHALLOW_VAL, DEEP_VAL)            \
  ANALYZER_OPTION(TYPE, NAME, CMDFLAG, DESC, SHALLOW_VAL)

#define ANALYZER_OPTION(TYPE, NAME, CMDFLAG, DESC, DEFAULT_VAL)                \
     (llvm::Twine(CMDFLAG " = ") + getStr(AnOpts.NAME.getValue())).str(),

#include "clang/StaticAnalyzer/Core/AnalyzerOptions.def"
#undef ANALYZER_OPTION
#undef ANALYZER_OPTION_DEPENDS_ON_USER_MODE
    };

    std::sort(std::begin(Options), std::end(Options));

    for (const std::string &Option : Options)
      llvm::errs() << Option << '\n';

    llvm::errs() << "[stats]\n" << "num-entries = "
                 << getNonCheckerConfigCount() << '\n';
  }
};
}

void ento::registerConfigDumper(CheckerManager &mgr) {
  mgr.registerChecker<ConfigDumper>();
}

bool ento::shouldRegisterConfigDumper(const LangOptions &LO) {
  return true;
}

//===----------------------------------------------------------------------===//
// ExplodedGraph Viewer
//===----------------------------------------------------------------------===//

namespace {
class ExplodedGraphViewer : public Checker< check::EndAnalysis > {
public:
  ExplodedGraphViewer() {}
  void checkEndAnalysis(ExplodedGraph &G, BugReporter &B,ExprEngine &Eng) const {
    Eng.ViewGraph(0);
  }
};

}

void ento::registerExplodedGraphViewer(CheckerManager &mgr) {
  mgr.registerChecker<ExplodedGraphViewer>();
}

bool ento::shouldRegisterExplodedGraphViewer(const LangOptions &LO) {
  return true;
}

//===----------------------------------------------------------------------===//
// Emits a report for every Stmt that the analyzer visits.
//===----------------------------------------------------------------------===//

namespace {

class ReportStmts : public Checker<check::PreStmt<Stmt>> {
  BuiltinBug BT_stmtLoc{this, "Statement"};

public:
  void checkPreStmt(const Stmt *S, CheckerContext &C) const {
    ExplodedNode *Node = C.generateNonFatalErrorNode();
    if (!Node)
      return;

    auto Report = llvm::make_unique<BugReport>(BT_stmtLoc, "Statement", Node);

    C.emitReport(std::move(Report));
  }
};

} // end of anonymous namespace

void ento::registerReportStmts(CheckerManager &mgr) {
  mgr.registerChecker<ReportStmts>();
}

bool ento::shouldRegisterReportStmts(const LangOptions &LO) {
  return true;
}
