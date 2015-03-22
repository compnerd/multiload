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

#ifndef multiload_auxiliary_hh
#define multiload_auxiliary_hh

#include "elf/types.hh"
#include "elf/pointer.hh"

#include <cstdint>

namespace elf {
template <size_t BitSex>
class auxiliary {
private:
  using address_t = typename traits<BitSex>::address_t;

public:
  enum class vector : address_t {
    null,                   //! AT_NULL
    ignore,                 //! AT_IGNORE
    file_descriptor,        //! AT_EXECFD
    program_header,         //! AT_PHDR
    program_header_size,    //! AT_PHENT
    program_headers,        //! AT_PHUM
    page_size,              //! AT_PAGESZ
    base_address,           //! AT_BASE
    flags,                  //! AT_FLAGS
    entry_point,            //! AT_ENTRY
    invalid_format,         //! AT_NOTELF
    user_id,                //! AT_UID
    effective_user_id,      //! AT_EUID
    group_id,               //! AT_GID
    effective_group_id,     //! AT_EGID
    platform,               //! AT_PLATFORM
    hardware_capabilities,  //! AT_HWCAP
    clock_tick,             //! AT_CLKTCK
  };

  vector type;
  address_t value;
};
}

#endif

