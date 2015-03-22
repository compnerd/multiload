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

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "multiload/scoped-file-descriptor.hh"
#include "multiload/scoped-mmap.hh"
#include "elf/types.hh"
#include "support/format.hh"

namespace multiload {
void print_help(const char *argv0) {
  std::cerr << R"(multiload - a loader dispatcher
Copyright 2015 Saleem Abdulrasool <compnerd@compnerd.org>
)";
}

template <size_t BitSex>
[[noreturn]] void dispatch(const uint8_t *base, char *argv[]);

template <>
[[noreturn]] void dispatch<32>(const uint8_t *base, char *argv[]) {
  const auto ehdr = *reinterpret_cast<const elf::header<32> *>(base);
  switch (ehdr.machine_type) {
  case elf::machine::i386:
    ::execvpe("/usr/i686-pc-linux-gnu/lib/ld-linux.so.2", argv, environ);
  }
  std::cerr << "unhandled 32-bit machine type: "
            << format::hex(static_cast<uint16_t>(ehdr.machine_type));
  __builtin_trap();
}

template <>
[[noreturn]] void dispatch<64>(const uint8_t *base, char *argv[]) {
  const auto ehdr = *reinterpret_cast<const elf::header<64> *>(base);
  switch (ehdr.machine_type) {
  case elf::machine::x86_64:
    ::execvpe("/usr/x86_64-pc-linux-gnu/lib/ld-linux-x86-64.so.2", argv,
              environ);
  }
  std::cerr << "unhandled 64-bit machine type: "
            << format::hex(static_cast<uint16_t>(ehdr.machine_type));
  __builtin_trap();
}

[[noreturn]] void dispatch_elf_class(const uint8_t *base, char *argv[]) {
  const auto &identifier = *reinterpret_cast<const elf::identifier *>(base);
  const auto file_class =
      identifier[static_cast<int>(elf::identifier_field::file_class)];

  // NOTE(compnerd) hide the fact that multiload was ever in the picture
  argv[0] = argv[1];

  switch (static_cast<const elf::file_class>(file_class)) {
  case elf::file_class::none:
    ::exit(EXIT_FAILURE);
  case elf::file_class::class_32:
    dispatch<32>(base, argv);
  case elf::file_class::class_64:
    dispatch<64>(base, argv);
  }

  __builtin_trap();
}
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    multiload::print_help(argv[0]);
    return EXIT_FAILURE;
  }

  // TODO(compnerd) support arguments


  multiload::scoped_file_descriptor fd(::open(argv[1], O_RDONLY | O_CLOEXEC));
  if (fd < 0) {
    std::cerr << "unable to open '" << argv[1] << "': "
              << std::strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }

  struct stat st;
  if (::fstat(fd, &st) < 0) {
    std::cerr << "unable to stat '" << argv[1] << "': "
              << std::strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }

  void *base = ::mmap(NULL, st.st_size, PROT_READ,
                      MAP_PRIVATE | MAP_NONBLOCK | MAP_NORESERVE, fd, 0);
  multiload::scoped_mmap mapping(base, st.st_size);
  if (mapping == MAP_FAILED) {
    std::cerr << "unable to mmap '" << argv[1] << "': "
              << std::strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }

  static_assert(sizeof(elf::magic) - 1 == sizeof(uint32_t),
                "type-punning will truncate magic");
  if (not (*reinterpret_cast<const uint32_t *>(base) ==
           *reinterpret_cast<const uint32_t *>(elf::magic))) {
    std::cerr << "'" << argv[1] << "' is not an ELF module" << std::endl;
    return EXIT_FAILURE;
  }

  dispatch_elf_class(mapping, argv);

  __builtin_trap();
}

