//===--- FetchAllowList.cpp - Authorized URLs for std::fetch --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "clang/Basic/FetchAllowList.h"

using namespace clang;

// Compile a URL glob into an anchored regex. `**` -> ".*", `*` ->
// "[^/]*", everything else is regex-escaped so it matches literally.
static std::string globToRegex(llvm::StringRef Glob) {
  std::string Re = "^";
  for (size_t I = 0, N = Glob.size(); I < N; ++I) {
    const char C = Glob[I];
    if (C == '*') {
      if (I + 1 < N && Glob[I + 1] == '*') {
        Re += ".*";
        ++I;
      } else {
        Re += "[^/]*";
      }
      continue;
    }
    // escape regex metacharacters so URL punctuation is literal
    if (llvm::StringRef(".^$|()[]{}+?\\").contains(C))
      Re += '\\';
    Re += C;
  }
  Re += "$";
  return Re;
}

FetchPattern::FetchPattern(std::string Glob)
    : Glob(std::move(Glob)), PatternRegex(globToRegex(this->Glob)) {}

bool FetchPattern::matches(llvm::StringRef Url) const {
  return PatternRegex.isValid() && PatternRegex.match(Url);
}

bool FetchAllowList::isAllowed(llvm::StringRef Url) const {
  for (const FetchPattern &P : Patterns)
    if (P.matches(Url))
      return true;
  return false;
}
