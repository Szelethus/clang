// RUN: %clang_analyze_cc1 -verify %s \
// RUN:   -analyzer-output=plist -o %t.plist \
// RUN:   -analyzer-checker=core \
// RUN:   -analyzer-checker=debug.ReportStmts

struct h {
  operator int();
};

int k() {
  return h(); // expected-warning{{Statement}}
              // expected-warning@-1{{Statement}}
              // expected-warning@-2{{Statement}}
}
