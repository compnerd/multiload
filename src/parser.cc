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

#include "multiload/parser.hh"
#include "multiload/lexer.hh"

#include <cassert>
#include <iostream>

namespace multiload {
configuration::constraint parser::parse_constraint() {
  switch (lexer_.head()) {
  default: __builtin_trap();
  case token::type::kw_arch:
  case token::type::kw_subarch:
  case token::type::kw_endian:
  case token::type::kw_flags:
    token key = lexer_.next();

    if (not lexer_.head().is<token::type::literal>())
      __builtin_trap();
    token value = lexer_.next();

    if (lexer_.head().is<token::type::semi>())
      lexer_.next();

    return { std::string(key.value().data(), key.value().length()),
             std::string(value.value().data(), value.value().length()) };
  }
}

configuration::rule parser::parse_rule() {
  assert(lexer_.head().is<token::type::kw_loader>() && "expected 'loader'");
  lexer_.next();

  configuration::rule rule;

  if (not lexer_.head().is<token::type::literal>())
    __builtin_trap();
  token loader = lexer_.next();
  rule.loader = std::string(loader.value().data(), loader.value().length());

  if (not lexer_.head().is<token::type::l_brace>())
    __builtin_trap();
  lexer_.next();

  do
    rule.constraints.push_back(parse_constraint());
  while (not lexer_.head().is<token::type::r_brace>() and
         not lexer_.head().is<token::type::eof>());

  assert(lexer_.head().is<token::type::r_brace>() && "expected '}'");
  lexer_.next();

  return rule;
}

std::vector<configuration::rule> parser::parse() {
  std::vector<configuration::rule> rules;
  while (lexer_.head().is<token::type::kw_loader>())
    rules.push_back(parse_rule());
  return rules;
}
}

