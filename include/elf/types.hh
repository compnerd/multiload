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

#ifndef multiload_elf_types_hh
#define multiload_elf_types_hh

#include <cstddef>
#include <cstdint>

namespace elf {
constexpr static const char magic[] = { 0x7f, 'E', 'L', 'F', '\0' };
constexpr static const size_t identifier_length = 16;

template <size_t BitSex>
struct traits;

template <>
struct traits<32> {
  typedef int32_t signed_t;
  typedef uint32_t unsigned_t;

  typedef uint32_t address_t;
  typedef uint32_t offset_t;
};

template <>
struct traits<64> {
  typedef int64_t signed_t;
  typedef uint64_t unsigned_t;

  typedef uint64_t address_t;
  typedef uint64_t offset_t;
};

enum class type : uint16_t {
  none,                      //! ET_NONE
  relocatable,               //! ET_REL
  exec,                      //! ET_EXEC
  dynamic,                   //! ET_DYN
  core,                      //! ET_CORE
  processor_specific_begin,  //! ET_LOPROC
  processor_specific_end,    //! ET_HIPROC
};

enum class machine : uint16_t {
  none,
  m32,                //! EM_M32 (ATT WE 32100)
  sparc,              //! EM_SPARC (SPARC)
  i386,               //! EM_386 (Intel i386)
  m68k,               //! EM_68K (Motorola 68000)
  m88k,               //! EM_88K (Motorols 88000)
  i486,               //! EM_486 (Intel 486)
  i860,               //! EM_860 (Intel 80860)
  mips,               //! EM_MIPS (MIPS RS3000)
  mips_rs4_be = 10,   //! EM_MIPS_RS4_BE (MIPS RS4000 Big Endian)
  mips_rs3_le = 10,   //! EM_MIPS_RS3_LE (MIPS RS3000 Little Endian)
  parisc = 15,        //! EM_PARISC (HPPA)
  sparc32_plus = 18,  //! EM_SPARC32PLUS (SUN v8+)
  ppc = 20,           //! EM_PPC (PowerPC)
  ppc64 = 21,         //! EM_PPC64 (PowerPC 64)
  s390 = 22,          //! EM_S390 (IBM S/390)
  spu = 23,           //! EM_SPU (Cell SPU Big Endian)
  arm = 40,           //! EM_ARM (ARM 32-bit)
  sh = 42,            //! EM_SH (SuperH)
  sparcv9 = 43,       //! EM_SPARCV9 (Sparc v9)
  itanium = 50,       //! EM_IA_64 (HP/Intel Itanium - IA/64)
  x86_64 = 62,        //! EM_X86_64 (AMD x86-64)
  cris = 76,          //! EM_CRIS (Axis Communications 32-bit embedded processor)
  v850 = 87,          //! EM_V850 (NEC v850)
  m32r = 88,          //! EM_M32R (Renesas M32R)
  mn10300 = 89,       //! EM_MN10300 (Panasonic/MEI MN10300, AM33)
  openrisc = 92,      //! EM_OPENRISC (OpenRISC 32-bit embeedded processor)
  blackfin = 106,     //! EM_BLACKFIN (ADI Blackfin Processor)
  nios2 = 113,        //! EM_ALTERA_NIOS2 (Altera NIOS II soft-core processor)
  c6000 = 140,        //! EM_TI_C6000 (TI C6X DSP)
  aarch64 = 183,      //! EM_AARCH64 (ARM 64-bit)
  avr32 = 0x18ad,     //! EM_AVR32 (Atmel AVR32)
  frv = 0x5441,       //! EM_FRV (Fujitsu FR-V)
};

enum class version : uint32_t {
  none,     //! EV_NONE
  current,  //! EV_CURRENT
};

enum class file_class : uint8_t {
  none,     //! ELFCLASSNONE
  class_32, //! ELFCLASS32
  class_64, //! ELFCLASS64
};

enum class identifier_field : uint8_t {
  magic0,         //! EI_MAG0
  magic1,         //! EI_MAG1
  magic2,         //! EI_MAG2
  magic3,         //! EI_MAG3
  file_class,     //! EI_CLASS
  data_encoding,  //! EI_DATA
  file_version,   //! EI_VERSION
  os_abi,         //! EI_OSABI
  abi_version,    //! EI_ABIVERSION
};

typedef uint8_t identifier[identifier_length];

enum class flags : uint32_t {
  arm_abi_mask = 0xff000000,        //! EF_ARM_ABIMASK
  arm_big_endian = 0x00800000,      //! EF_ARM_BE8
  arm_gcc_mask = 0x00400fff,        //! EF_ARM_GCCMASK
  arm_abi_hard_float = 0x00000400,  //! EF_ARM_ABI_FLOAT_HARD
  arm_abi_soft_float = 0x00000200,  //! EF_ARM_ABI_SOFT_FLOAT
};

template <size_t BitSex>
class header {
  using address_t = typename traits<BitSex>::address_t;
  using offset_t = typename traits<BitSex>::offset_t;

public:
  elf::identifier identifier;                 //! e_ident;
  elf::type file_type;                        //! e_type
  elf::machine machine_type;                  //! e_machine
  elf::version version;                       //! e_version
  address_t entry_point;                      //! e_entry
  offset_t program_header_offset;             //! e_phoff
  offset_t section_header_offset;             //! e_shoff
  uint32_t flags;                             //! e_flags
  uint16_t size;                              //! e_ehsize
  uint16_t program_header_size;               //! e_phentsize
  uint16_t program_headers;                   //! e_phnum
  uint16_t section_header_size;               //! e_shentsize
  uint16_t section_headers;                   //! e_shnum
  uint16_t section_header_string_index;       //! e_shstrndx
};

enum class section_type : uint32_t {
  inactive,                                   //! SHT_NULL
  program_bits,                               //! SHT_PROGBITS
  symbol_table,                               //! SHT_SYMTAB
  string_table,                               //! SHT_STRTAB
  relocation_with_addend,                     //! SHT_RELA
  hashtable,                                  //! SHT_HASH
  dynamic_link_info,                          //! SHT_DYNAMIC
  note,                                       //! SHT_NOTE
  empty,                                      //! SHT_EMPTY
  relocation,                                 //! SHT_REL
  shared_library,                             //! SHT_SHLIB
  dynamic_symbols,                            //! SHT_DYNSYM
  processor_specific_begin = 0x70000000,      //! SHT_LOPROC
  processor_specific_end = 0x7fffffff,        //! SHT_HIPROC
  user_specific_begin = 0x80000000,           //! SHT_LOUSER
  user_specific_end = 0xffffffff,             //! SHT_HIUSER

  /*!< extensions >*/
  arm_exception_index_table = 0x70000001,     //! SHT_ARM_EXIDX
  arm_preemption_map = 0x70000002,            //! SHT_ARM_PREEMPTMAP
  arm_attributes = 0x70030003,                //! SHT_ARM_ATTRIBUTES
  arm_debug_overlay = 0x70000004,             //! SHT_ARM_DEBUGOVERLAY
  arm_debug_overlaysection = 0x70000005,      //! SHT_ARM_OVERLAYSECTION
};

template <size_t BitSex>
class section_header {
  using address_t = typename traits<BitSex>::address_t;
  using offset_t = typename traits<BitSex>::offset_t;
  using word_t = typename traits<BitSex>::unsigned_t;

public:
  uint32_t name;             //! sh_name
  uint32_t type;             //! sh_type
  word_t flags;              //! sh_flags
  address_t address;         //! sh_addr
  offset_t offset;           //! sh_offset
  word_t size;               //! sh_size
  uint32_t link;             //! sh_link
  uint32_t info;             //! sh_info
  word_t address_alignment;  //! sh_addralign
  word_t entry_size;         //! sh_entsize
};
}

#endif

