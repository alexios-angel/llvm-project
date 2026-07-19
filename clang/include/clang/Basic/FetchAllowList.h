//===--- FetchAllowList.h - Authorized URLs for std::fetch ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The authorization gate for __builtin_std_fetch: fetching a URL over the
// network at compile time is only permitted when the URL matches one of the
// allow-list glob patterns supplied on the command line (--fetch-allow=) or
// via a directive. This is the fetch analogue of the #depend file gate.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_BASIC_FETCHALLOWLIST_H
#define LLVM_CLANG_BASIC_FETCHALLOWLIST_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Regex.h"
#include <string>
#include <vector>

namespace clang {

/// One allow-list URL glob compiled to a regex. Glob semantics: `*`
/// matches any run of characters except '/', `**` matches any run
/// including '/', and every other character is matched literally. A
/// pattern with no wildcard authorizes exactly that URL (prefix, if it
/// ends in '/').
struct FetchPattern {
  std::string Glob;
  llvm::Regex PatternRegex;

  explicit FetchPattern(std::string Glob);
  bool matches(llvm::StringRef Url) const;
};

/// The set of authorized URL patterns for this compilation.
class FetchAllowList {
  std::vector<FetchPattern> Patterns;

public:
  FetchAllowList() = default;

  void add(std::string Glob) { Patterns.emplace_back(std::move(Glob)); }
  bool empty() const { return Patterns.empty(); }

  /// Is \p Url authorized to be fetched at compile time?
  bool isAllowed(llvm::StringRef Url) const;
};

} // namespace clang

#endif
