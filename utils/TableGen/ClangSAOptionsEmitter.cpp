//===- OptParserEmitter.cpp - Table Driven Command Line Parsing -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/TableGen/Error.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Twine.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"
#include "TableGenBackends.h"
#include <cctype>
#include <cstring>
#include <map>

using namespace llvm;

// Ordering on Info. The logic should match with the consumer-side function in
// llvm/Option/OptTable.h.
// FIXME: Mmake this take StringRefs instead of null terminated strings to
// simplify callers.
static int StrCmpOptionName(const char *A, const char *B) {
  const char *X = A, *Y = B;
  char a = tolower(*A), b = tolower(*B);
  while (a == b) {
    if (a == '\0')
      return strcmp(A, B);

    a = tolower(*++X);
    b = tolower(*++Y);
  }

  if (a == '\0') // A is a prefix of B.
    return 1;
  if (b == '\0') // B is a prefix of A.
    return -1;

  // Otherwise lexicographic.
  return (a < b) ? -1 : 1;
}

static int CompareOptionRecords(Record *const *Av, Record *const *Bv) {
  const Record *A = *Av;
  const Record *B = *Bv;

  // Sentinel options precede all others and are only ordered by precedence.
  bool ASent = A->getValueAsDef("Kind")->getValueAsBit("Sentinel");
  bool BSent = B->getValueAsDef("Kind")->getValueAsBit("Sentinel");
  if (ASent != BSent)
    return ASent ? -1 : 1;

  // Compare options by name, unless they are sentinels.
  if (!ASent)
    if (int Cmp = StrCmpOptionName(A->getValueAsString("Name").str().c_str(),
                                   B->getValueAsString("Name").str().c_str()))
      return Cmp;

  if (!ASent) {
    std::vector<StringRef> APrefixes = A->getValueAsListOfStrings("Prefixes");
    std::vector<StringRef> BPrefixes = B->getValueAsListOfStrings("Prefixes");

    for (std::vector<StringRef>::const_iterator APre = APrefixes.begin(),
                                                AEPre = APrefixes.end(),
                                                BPre = BPrefixes.begin(),
                                                BEPre = BPrefixes.end();
                                                APre != AEPre &&
                                                BPre != BEPre;
                                                ++APre, ++BPre) {
      if (int Cmp = StrCmpOptionName(APre->str().c_str(), BPre->str().c_str()))
        return Cmp;
    }
  }

  // Then by the kind precedence;
  int APrec = A->getValueAsDef("Kind")->getValueAsInt("Precedence");
  int BPrec = B->getValueAsDef("Kind")->getValueAsInt("Precedence");
  if (APrec == BPrec &&
      A->getValueAsListOfStrings("Prefixes") ==
      B->getValueAsListOfStrings("Prefixes")) {
    PrintError(A->getLoc(), Twine("Option is equivalent to"));
    PrintError(B->getLoc(), Twine("Other defined here"));
    PrintFatalError("Equivalent Options found.");
  }
  return APrec < BPrec ? -1 : 1;
}

static const std::string getOptionName(const Record &R) {
  // Use the record name unless EnumName is defined.
  if (isa<UnsetInit>(R.getValueInit("EnumName")))
    return R.getName();

  return R.getValueAsString("EnumName");
}

static raw_ostream &write_cstring(raw_ostream &OS, llvm::StringRef Str) {
  OS << '"';
  OS.write_escaped(Str);
  OS << '"';
  return OS;
}

/// OptParserEmitter - This tablegen backend takes an input .td file
/// describing a list of options and emits a data structure for parsing and
/// working with those options when given an input command line.
void clang::EmitClangSAOptions(RecordKeeper &Records, raw_ostream &OS) {
  std::vector<Record*> Opts = Records.getAllDerivedDefinitions("Option");

  emitSourceFileHeader("Option Parsing Definitions", OS);

  array_pod_sort(Opts.begin(), Opts.end(), CompareOptionRecords);

  {
    auto It = Opts.begin();
    assert((*It)->getName() == "INPUT");
    It = Opts.erase(It);
    assert((*It)->getName() == "UNKNOWN");
    Opts.erase(It);
  }

  // Generate prefix groups.
  typedef SmallVector<SmallString<2>, 2> PrefixKeyT;
  typedef std::map<PrefixKeyT, std::string> PrefixesT;
  PrefixesT Prefixes;
  Prefixes.insert(std::make_pair(PrefixKeyT(), "prefix_0"));
  unsigned CurPrefix = 0;

  for (const Record *R : Opts) {
    std::vector<StringRef> prf = R->getValueAsListOfStrings("Prefixes");
    PrefixKeyT prfkey(prf.begin(), prf.end());

    unsigned NewPrefix = CurPrefix + 1;
    if (Prefixes.insert(std::make_pair(prfkey, (Twine("prefix_") +
                                              Twine(NewPrefix)).str())).second)
      CurPrefix = NewPrefix;
  }

  OS << "//===" << std::string(70, '-') << "===//\n";
  OS << "// Options\n";
  OS << "//===" << std::string(70, '-') << "===//\n\n";
  for (unsigned i = 0, e = Opts.size(); i != e; ++i) {
    const Record &R = *Opts[i];

    if (const DefInit *DI = dyn_cast<DefInit>(R.getValueInit("Alias")))
      continue;

    // Start a single option entry.
    OS << "OPTION(";

    // The option string.
    write_cstring(OS, R.getValueAsString("Name"));

    // The option identifier name.
    OS  << ", "<< getOptionName(R);
    
    // The option help text.
    if (!isa<UnsetInit>(R.getValueInit("HelpText"))) {
      OS << ",\n";
      OS << "       ";
      write_cstring(OS, R.getValueAsString("HelpText"));
    } else
      OS << ", nullptr";
    OS << ")\n";
  }
}
