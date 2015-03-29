/**
 * Copyright © 2015 Saleem Abdulrasool <compnerd@compnerd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **/

#include "multiload/lexer.hh"

#include <cassert>

using token = multiload::token;
using string = std::char_traits<char>;

#if defined(__clang__)
static constexpr const char * const spelling[] = {
  [static_cast<int>(token::type::l_brace)] = "{",
  [static_cast<int>(token::type::r_brace)] = "}",
  [static_cast<int>(token::type::semi)] = ";",

  [static_cast<int>(token::type::kw_arch)] = "arch",
  [static_cast<int>(token::type::kw_subarch)] = "subarch",
  [static_cast<int>(token::type::kw_endian)] = "endian",
  [static_cast<int>(token::type::kw_flags)] = "flags",
  [static_cast<int>(token::type::kw_loader)] = "loader",
};
#else
static constexpr const char * const spelling [] = {
  /* invalid */     nullptr,
  /* whitespace */  nullptr,
  /* eof */         nullptr,
  /* l_brace */     "{",
  /* r_brace */     "}",
  /* semi */        ";",

  /* kw_arch */     "arch",
  /* kw_subarch */  "subarch",
  /* kw_endian */   "endian",
  /* kw_flags */    "flags",
  /* kw_loader */   "loader",
};
#endif

template <char ...>
struct set;

template <>
struct set<> {
  static constexpr bool contains(char) noexcept {
    return false;
  }
};

template <char Value, char... Values>
struct set<Value, Values...> {
  static constexpr bool contains(char ch) noexcept {
    return ch == Value or set<Values...>::contains(ch);
  }
};

namespace multiload {
template <token::type Type>
token lexer::consume() noexcept {
  const char *lexeme = cursor_;

  const auto token_length = string::length(spelling[static_cast<int>(Type)]);

  cursor_ = cursor_ + token_length;
  column_ = column_ + token_length;

  return token(Type, std::experimental::string_view(lexeme, token_length));
}

template <token::type Type>
bool lexer::match() const noexcept {
  auto constexpr token_spelling = spelling[static_cast<int>(Type)];
  const auto token_length = string::length(token_spelling);
  return static_cast<size_t>(buffer_end_ - cursor_) >= token_length and
         std::experimental::string_view(cursor_, token_length) ==
             token_spelling and
         (static_cast<size_t>(buffer_end_ - cursor_) == token_length or
          not isalpha(cursor_[token_length]));
}

template <>
token lexer::consume<token::type::literal>() noexcept {
  using whitespace = set<' ', '\f', '\n', '\t', '\r', '\v'>;

  const char *lexeme = cursor_;

  do
    ++cursor_, ++column_;
  while (not whitespace::contains(*cursor_));

  return token(token::type::literal,
               std::experimental::string_view(lexeme, cursor_ - lexeme));
}

template <>
token lexer::consume<token::type::whitespace>() noexcept {
  while (cursor_ < buffer_end_) {
    switch (*cursor_) {
    default:
      return token(token::type::whitespace, std::experimental::string_view());
    case '\0':
    case ' ':
    case '\t':
      column_++;
      cursor_++;
      break;
    case '\f':
      cursor_++;
    case '\r':
    case '\n':
    case '\v':
      cursor_++;
      column_ = 0;
      line_++;
      break;
    }
  }
  return token();
}

token lexer::lex() noexcept {
  assert(cursor_ <= buffer_end_ && "cursor may not extend beyond buffer_end_");
  if (cursor_ == buffer_end_)
    return token();

  consume<token::type::whitespace>();
  if (cursor_ == buffer_end_)
    return token();

  switch (*cursor_) {
  case '{':
    return consume<token::type::l_brace>();
  case '}':
    return consume<token::type::r_brace>();
  case ';':
    return consume<token::type::semi>();
  case 'a':
    if (match<token::type::kw_arch>())
      return consume<token::type::kw_arch>();
  case 'e':
    if (match<token::type::kw_endian>())
      return consume<token::type::kw_endian>();
  case 'f':
    if (match<token::type::kw_flags>())
      return consume<token::type::kw_flags>();
  case 'l':
    if (match<token::type::kw_loader>())
      return consume<token::type::kw_loader>();
  case 's':
    if (match<token::type::kw_subarch>())
      return consume<token::type::kw_subarch>();
  }
  return consume<token::type::literal>();
}

token lexer::head() noexcept {
  if (lookahead_.empty())
    lookahead_.push_back(next());
  return lookahead_.front();
}

token lexer::next() noexcept {
  if (not lookahead_.empty()) {
    auto token = lookahead_.front();
    lookahead_.pop_front();
    return token;
  }
  return lex();
}
}

