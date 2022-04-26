#ifndef MINIMAL_ELF_H_
#define MINIMAL_ELF_H_

/* Minimal elf definitions stripped from FreeBSD's headers
 * and included so that there's no external dependencies
 * for the mac version of the Loader.
 *  */

/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 1996-1998 John D. Polstra.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#define EI_NIDENT 16
#define EM_386 3
#define EM_X86_64 62
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_REL 9
#define R_386_32 1
#define R_386_PC32 2

typedef uint32_t    Elf32_Addr;
typedef uint16_t    Elf32_Half;
typedef uint32_t    Elf32_Off;
typedef int32_t     Elf32_Sword;
typedef uint32_t    Elf32_Word;
typedef uint64_t    Elf32_Lword;

typedef Elf32_Word  Elf32_Hashelt;

typedef Elf32_Word  Elf32_Size;
typedef Elf32_Sword Elf32_Ssize;

typedef uint64_t    Elf64_Addr;
typedef uint16_t    Elf64_Half;
typedef uint64_t    Elf64_Off;
typedef int32_t     Elf64_Sword;
typedef int64_t     Elf64_Sxword;
typedef uint32_t    Elf64_Word;
typedef uint64_t    Elf64_Lword;
typedef uint64_t    Elf64_Xword;
typedef Elf64_Word  Elf64_Hashelt;

/* Non-standard class-dependent datatype used for abstraction. */
typedef Elf64_Xword Elf64_Size;
typedef Elf64_Sxword    Elf64_Ssize;

typedef struct {
    unsigned char   e_ident[EI_NIDENT]; /* File identification. */
    Elf64_Half  e_type;     /* File type. */
    Elf64_Half  e_machine;  /* Machine architecture. */
    Elf64_Word  e_version;  /* ELF format version. */
    Elf64_Addr  e_entry;    /* Entry point. */
    Elf64_Off   e_phoff;    /* Program header file offset. */
    Elf64_Off   e_shoff;    /* Section header file offset. */
    Elf64_Word  e_flags;    /* Architecture-specific flags. */
    Elf64_Half  e_ehsize;   /* Size of ELF header in bytes. */
    Elf64_Half  e_phentsize;    /* Size of program header entry. */
    Elf64_Half  e_phnum;    /* Number of program header entries. */
    Elf64_Half  e_shentsize;    /* Size of section header entry. */
    Elf64_Half  e_shnum;    /* Number of section header entries. */
    Elf64_Half  e_shstrndx; /* Section name strings section. */
} Elf64_Ehdr;

typedef struct {
    unsigned char   e_ident[EI_NIDENT]; /* File identification. */
    Elf32_Half  e_type;     /* File type. */
    Elf32_Half  e_machine;  /* Machine architecture. */
    Elf32_Word  e_version;  /* ELF format version. */
    Elf32_Addr  e_entry;    /* Entry point. */
    Elf32_Off   e_phoff;    /* Program header file offset. */
    Elf32_Off   e_shoff;    /* Section header file offset. */
    Elf32_Word  e_flags;    /* Architecture-specific flags. */
    Elf32_Half  e_ehsize;   /* Size of ELF header in bytes. */
    Elf32_Half  e_phentsize;    /* Size of program header entry. */
    Elf32_Half  e_phnum;    /* Number of program header entries. */
    Elf32_Half  e_shentsize;    /* Size of section header entry. */
    Elf32_Half  e_shnum;    /* Number of section header entries. */
    Elf32_Half  e_shstrndx; /* Section name strings section. */
} Elf32_Ehdr;

typedef struct {
    Elf32_Word  sh_name;    /* Section name (index into the
                       section header string table). */
    Elf32_Word  sh_type;    /* Section type. */
    Elf32_Word  sh_flags;   /* Section flags. */
    Elf32_Addr  sh_addr;    /* Address in memory image. */
    Elf32_Off   sh_offset;  /* Offset in file. */
    Elf32_Word  sh_size;    /* Size in bytes. */
    Elf32_Word  sh_link;    /* Index of a related section. */
    Elf32_Word  sh_info;    /* Depends on section type. */
    Elf32_Word  sh_addralign;   /* Alignment in bytes. */
    Elf32_Word  sh_entsize; /* Size of each entry in section. */
} Elf32_Shdr;

typedef struct {
    Elf64_Word  sh_name;    /* Section name (index into the
                       section header string table). */
    Elf64_Word  sh_type;    /* Section type. */
    Elf64_Xword sh_flags;   /* Section flags. */
    Elf64_Addr  sh_addr;    /* Address in memory image. */
    Elf64_Off   sh_offset;  /* Offset in file. */
    Elf64_Xword sh_size;    /* Size in bytes. */
    Elf64_Word  sh_link;    /* Index of a related section. */
    Elf64_Word  sh_info;    /* Depends on section type. */
    Elf64_Xword sh_addralign;   /* Alignment in bytes. */
    Elf64_Xword sh_entsize; /* Size of each entry in section. */
} Elf64_Shdr;

typedef struct {
    Elf64_Word  p_type;     /* Entry type. */
    Elf64_Word  p_flags;    /* Access permission flags. */
    Elf64_Off   p_offset;   /* File offset of contents. */
    Elf64_Addr  p_vaddr;    /* Virtual address in memory image. */
    Elf64_Addr  p_paddr;    /* Physical address (not used). */
    Elf64_Xword p_filesz;   /* Size of contents in file. */
    Elf64_Xword p_memsz;    /* Size of contents in memory. */
    Elf64_Xword p_align;    /* Alignment in memory and file. */
} Elf64_Phdr;

typedef struct {
    Elf32_Word  p_type;     /* Entry type. */
    Elf32_Off   p_offset;   /* File offset of contents. */
    Elf32_Addr  p_vaddr;    /* Virtual address in memory image. */
    Elf32_Addr  p_paddr;    /* Physical address (not used). */
    Elf32_Word  p_filesz;   /* Size of contents in file. */
    Elf32_Word  p_memsz;    /* Size of contents in memory. */
    Elf32_Word  p_flags;    /* Access permission flags. */
    Elf32_Word  p_align;    /* Alignment in memory and file. */
} Elf32_Phdr;

/* Relocations that don't need an addend field. */
typedef struct {
    Elf32_Addr  r_offset;   /* Location to be relocated. */
    Elf32_Word  r_info;     /* Relocation type and symbol index. */
} Elf32_Rel;

/* Relocations that need an addend field. */
typedef struct {
    Elf32_Addr  r_offset;   /* Location to be relocated. */
    Elf32_Word  r_info;     /* Relocation type and symbol index. */
    Elf32_Sword r_addend;   /* Addend. */
} Elf32_Rela;

typedef struct {
    Elf32_Word  st_name;    /* String table index of name. */
    Elf32_Addr  st_value;   /* Symbol value. */
    Elf32_Word  st_size;    /* Size of associated object. */
    unsigned char   st_info;    /* Type and binding information. */
    unsigned char   st_other;   /* Reserved (not used). */
    Elf32_Half  st_shndx;   /* Section index of symbol. */
} Elf32_Sym;

typedef struct {
    Elf64_Word  st_name;    /* String table index of name. */
    unsigned char   st_info;    /* Type and binding information. */
    unsigned char   st_other;   /* Reserved (not used). */
    Elf64_Half  st_shndx;   /* Section index of symbol. */
    Elf64_Addr  st_value;   /* Symbol value. */
    Elf64_Xword st_size;    /* Size of associated object. */
} Elf64_Sym;

/* Relocations that don't need an addend field. */
typedef struct {
    Elf64_Addr  r_offset;   /* Location to be relocated. */
    Elf64_Xword r_info;     /* Relocation type and symbol index. */
} Elf64_Rel;

/* Relocations that need an addend field. */
typedef struct {
    Elf64_Addr  r_offset;   /* Location to be relocated. */
    Elf64_Xword r_info;     /* Relocation type and symbol index. */
    Elf64_Sxword    r_addend;   /* Addend. */
} Elf64_Rela;


#endif
