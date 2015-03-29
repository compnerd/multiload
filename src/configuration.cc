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

#include "multiload/configuration.hh"
#include "multiload/checker.hh"
#include "multiload/lexer.hh"
#include "multiload/parser.hh"
#include "multiload/scoped-file-descriptor.hh"
#include "multiload/scoped-mmap.hh"

#include "elf/types.hh"

#include "support/format.hh"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <functional>
#include <iostream>
#include <tuple>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace multiload {
bool configuration::load() noexcept {
  multiload::scoped_file_descriptor fd(::open(file_.c_str(),
                                              O_RDONLY | O_CLOEXEC));
  if (fd < 0) {
    std::cerr << "unable to open '" << file_ << "': "
              << std::strerror(errno) << std::endl;
    return false;
  }

  struct stat st;
  if (::fstat(fd, &st) < 0) {
    std::cerr << "unable to stat '" << file_ << "': "
              << std::strerror(errno) << std::endl;
    return false;
  }

  void *base = ::mmap(NULL, st.st_size, PROT_READ,
                      MAP_PRIVATE | MAP_NONBLOCK | MAP_NORESERVE, fd, 0);
  multiload::scoped_mmap mapping(base, st.st_size);
  if (mapping == MAP_FAILED) {
    std::cerr << "unable to mmap '" << file_ << "': "
              << std::strerror(errno) << std::endl;
    return false;
  }

  multiload::lexer lexer(static_cast<const char *>(base), st.st_size);
  multiload::parser parser(lexer);

  rules_ = parser.parse();
  return not rules_.empty();
}

[[noreturn]] void
configuration::dispatch(const uint8_t *base, char *argv[]) const noexcept {
  assert(not rules_.empty() && "configuration must be loaded first");

  const auto &identifier = *reinterpret_cast<const elf::identifier *>(base);
  const auto file_class =
      identifier[static_cast<int>(elf::identifier_field::file_class)];
  elf::machine machine_type = elf::machine::none;

  switch (static_cast<const elf::file_class>(file_class)) {
  case elf::file_class::none:
    ::exit(EXIT_FAILURE);
  case elf::file_class::class_32: {
    const auto ehdr = *reinterpret_cast<const elf::header<32> *>(base);
    machine_type = ehdr.machine_type;
  }
  case elf::file_class::class_64: {
    const auto ehdr = *reinterpret_cast<const elf::header<64> *>(base);
    machine_type = ehdr.machine_type;
  }
  }

  std::function<bool(const uint8_t *, const configuration::constraints &)>
      validate;
  std::string loader;

  switch (machine_type) {
  case elf::machine::none:
    ::exit(EXIT_FAILURE);
  case elf::machine::aarch64:
    validate = arch::aarch64::validate;
    break;
  case elf::machine::arm:
    validate = arch::arm::validate;
    break;
  case elf::machine::i386:
    validate = arch::i386::validate;
    break;
  case elf::machine::x86_64:
    validate = arch::x86_64::validate;
    break;
  }

  for (const auto &rule : rules_) {
    if (validate(base, rule.constraints)) {
      loader = rule.loader;
      break;
    }
  }

  multiload::validate_loader(machine_type, loader);
  ::execvpe(loader.c_str(), argv, environ);
  __builtin_trap();
}
}

