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

#include "multiload/checker.hh"

#include "support/format.hh"

#include <cstring>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace multiload {
void validate_loader(elf::machine machine_type, const std::string &loader) {
  if (loader.empty()) {
    std::cerr << "cannot load binary for machine "
              << format::hex(static_cast<uint16_t>(machine_type))
              << ": no loader specified" << std::endl;
    ::exit(EXIT_FAILURE);
  }

  struct stat mld;
  if (::stat("/proc/self/exe", &mld) < 0) {
    std::cerr << "unable to stat '/proc/self/exe': " << std::strerror(errno)
              << std::endl;
    ::exit(EXIT_FAILURE);
  }

  struct stat ldr;
  if (::stat(loader.c_str(), &ldr) < 0) {
    std::cerr << "unable to stat '" << loader << "':" << std::strerror(errno)
              << std::endl;
    ::exit(EXIT_FAILURE);
  }

  if (mld.st_dev == ldr.st_dev and mld.st_ino == ldr.st_ino) {
    std::cerr << "recursively invoking multiload is not permitted" << std::endl;
    ::exit(EXIT_FAILURE);
  }
}

namespace arch {
namespace aarch64 {
bool validate(const uint8_t *base,
              const multiload::configuration::constraints &constraints) {
  for (const auto &constraint : constraints)
    if (constraint.key == "arch")
      return constraint.value == "aarch64";
  return false;
}
}

namespace arm {
bool validate(const uint8_t *base,
              const multiload::configuration::constraints &constraints) {
  for (const auto &constraint : constraints)
    if (constraint.key == "arch")
      return constraint.value == "arm";
  return false;
}
}

namespace i386 {
bool validate(const uint8_t *base,
              const multiload::configuration::constraints &constraints) {
  for (const auto &constraint : constraints)
    if (constraint.key == "arch")
      return constraint.value == "i386";
  return false;
}
}

namespace x86_64 {
bool validate(const uint8_t *base,
              const multiload::configuration::constraints &constraints) {
  for (const auto &constraint : constraints)
    if (constraint.key == "arch")
      return constraint.value == "x86_64";
  return false;
}
}
}
}

