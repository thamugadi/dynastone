#include "include/globals.h"
#include "keystone/keystone.h"
#include <stdio.h>
#include <string.h>

const char* list_arch = 
  "x16:       X86 16bit, Intel syntax\n"
  "x32:       X86 32bit, Intel syntax\n"
  "x64:       X86 64bit, Intel syntax\n"
  "x16att:    X86 16bit, AT&T syntax\n"
  "x32att:    X86 32bit, AT&T syntax\n"
  "x64att:    X86 64bit, AT&T syntax\n"
  "x16nasm:   X86 16bit, NASM syntax\n"
  "x32nasm:   X86 32bit, NASM syntax\n"
  "x64nasm:   X86 64bit, NASM syntax\n"
  "arm:       ARM - little endian\n"
  "armbe:     ARM - big endian\n"
  "thumb:     Thumb - little endian\n"
  "thumbbe:   Thumb - big endian\n"
  "armv8:     ARM V8 - little endian\n"
  "armv8be:   ARM V8 - big endian\n"
  "thumbv8:   Thumb V8 - little endian\n"
  "thumbv8be: Thumb V8 - big endian\n"
  "arm64:     AArch64\n"
  "hexagon:   Hexagon\n"
  "mips:      Mips - little endian\n"
  "mipsbe:    Mips - big endian\n"
  "mips64:    Mips64 - little endian\n"
  "mips64be:  Mips64 - big endian\n"
  "ppc32be:   PowerPC32 - big endian\n"
  "ppc64:     PowerPC64 - little endian\n"
  "ppc64be:   PowerPC64 - big endian\n"
  "sparc:     Sparc - little endian\n"
  "sparcbe:   Sparc - big endian\n"
  "sparc64be: Sparc64 - big endian\n"
  "systemz:   SystemZ (S390x)\n"
  "evm:       Ethereum Virtual Machine";

int is_big_endian_architecture(const char *arch) {
  if (!strcmp(arch, "armbe")) return 1;
  if (!strcmp(arch, "thumbbe")) return 1;
  if (!strcmp(arch, "armv8be")) return 1;
  if (!strcmp(arch, "thumbv8be")) return 1;
  if (!strcmp(arch, "mipsbe")) return 1;
  if (!strcmp(arch, "mips64be")) return 1;
  if (!strcmp(arch, "ppc32be")) return 1;
  if (!strcmp(arch, "ppc64be")) return 1;
  if (!strcmp(arch, "sparcbe")) return 1;
  if (!strcmp(arch, "sparc64be")) return 1;
  if (!strcmp(arch, "systemz")) return 1;
  return 0;
}

ks_err ks_open_arch(ks_engine** ks, const char* arch) {
  ks_err err;

  if (!strcmp(arch, "x16")) {
    err = ks_open(KS_ARCH_X86, KS_MODE_16 | KS_MODE_LITTLE_ENDIAN, ks);
    ks_option(*ks, KS_OPT_SYNTAX, KS_OPT_SYNTAX_INTEL);
    return err;
  }
  if (!strcmp(arch, "x32")) {
    err = ks_open(KS_ARCH_X86, KS_MODE_32 | KS_MODE_LITTLE_ENDIAN, ks);
    ks_option(*ks, KS_OPT_SYNTAX, KS_OPT_SYNTAX_INTEL);
    return err;
  }
  if (!strcmp(arch, "x64")) {
    err = ks_open(KS_ARCH_X86, KS_MODE_64 | KS_MODE_LITTLE_ENDIAN, ks);
    ks_option(*ks, KS_OPT_SYNTAX, KS_OPT_SYNTAX_INTEL);
    return err;
  }
  if (!strcmp(arch, "x16att")) {
    err = ks_open(KS_ARCH_X86, KS_MODE_16 | KS_MODE_LITTLE_ENDIAN, ks);
    ks_option(*ks, KS_OPT_SYNTAX, KS_OPT_SYNTAX_ATT);
    return err;
  }
  if (!strcmp(arch, "x32att")) {
    err = ks_open(KS_ARCH_X86, KS_MODE_32 | KS_MODE_LITTLE_ENDIAN, ks);
    ks_option(*ks, KS_OPT_SYNTAX, KS_OPT_SYNTAX_ATT);
    return err;
  }
  if (!strcmp(arch, "x64att")) {
    err = ks_open(KS_ARCH_X86, KS_MODE_64 | KS_MODE_LITTLE_ENDIAN, ks);
    ks_option(*ks, KS_OPT_SYNTAX, KS_OPT_SYNTAX_ATT);
    return err;
  }
  if (!strcmp(arch, "x16nasm")) {
    err = ks_open(KS_ARCH_X86, KS_MODE_16 | KS_MODE_LITTLE_ENDIAN, ks);
    ks_option(*ks, KS_OPT_SYNTAX, KS_OPT_SYNTAX_NASM);
    return err;
  }
  if (!strcmp(arch, "x32nasm")) {
    err = ks_open(KS_ARCH_X86, KS_MODE_32 | KS_MODE_LITTLE_ENDIAN, ks);
    ks_option(*ks, KS_OPT_SYNTAX, KS_OPT_SYNTAX_NASM);
    return err;
  }
  if (!strcmp(arch, "x64nasm")) {
    err = ks_open(KS_ARCH_X86, KS_MODE_64 | KS_MODE_LITTLE_ENDIAN, ks);
    ks_option(*ks, KS_OPT_SYNTAX, KS_OPT_SYNTAX_NASM);
    return err;
  } 
  if (!strcmp(arch, "arm")) return ks_open(KS_ARCH_ARM, KS_MODE_ARM | KS_MODE_LITTLE_ENDIAN, ks);
  if (!strcmp(arch, "armbe")) return ks_open(KS_ARCH_ARM, KS_MODE_ARM | KS_MODE_BIG_ENDIAN, ks);
  if (!strcmp(arch, "thumb")) return ks_open(KS_ARCH_ARM, KS_MODE_THUMB | KS_MODE_LITTLE_ENDIAN, ks);
  if (!strcmp(arch, "thumbbe")) return ks_open(KS_ARCH_ARM, KS_MODE_THUMB | KS_MODE_BIG_ENDIAN, ks);
  if (!strcmp(arch, "armv8")) return ks_open(KS_ARCH_ARM, KS_MODE_V8 | KS_MODE_LITTLE_ENDIAN, ks);
  if (!strcmp(arch, "armv8be")) return ks_open(KS_ARCH_ARM, KS_MODE_V8 | KS_MODE_BIG_ENDIAN, ks);
  if (!strcmp(arch, "thumbv8")) return ks_open(KS_ARCH_ARM, KS_MODE_THUMB | KS_MODE_V8 | KS_MODE_LITTLE_ENDIAN, ks);
  if (!strcmp(arch, "thumbv8be")) return ks_open(KS_ARCH_ARM, KS_MODE_THUMB | KS_MODE_V8 | KS_MODE_BIG_ENDIAN, ks);
  if (!strcmp(arch, "arm64")) return ks_open(KS_ARCH_ARM64, KS_MODE_LITTLE_ENDIAN, ks);
  if (!strcmp(arch, "hexagon")) return ks_open(KS_ARCH_HEXAGON, KS_MODE_LITTLE_ENDIAN, ks);
  if (!strcmp(arch, "mips")) return ks_open(KS_ARCH_MIPS, KS_MODE_MIPS32 | KS_MODE_LITTLE_ENDIAN, ks);
  if (!strcmp(arch, "mipsbe")) return ks_open(KS_ARCH_MIPS, KS_MODE_MIPS32 | KS_MODE_BIG_ENDIAN, ks);
  if (!strcmp(arch, "mips64")) return ks_open(KS_ARCH_MIPS, KS_MODE_MIPS64 | KS_MODE_LITTLE_ENDIAN, ks);
  if (!strcmp(arch, "mips64be")) return ks_open(KS_ARCH_MIPS, KS_MODE_MIPS64 | KS_MODE_BIG_ENDIAN, ks);
  if (!strcmp(arch, "ppc32be")) return ks_open(KS_ARCH_PPC, KS_MODE_PPC32 | KS_MODE_BIG_ENDIAN, ks);
  if (!strcmp(arch, "ppc64")) return ks_open(KS_ARCH_PPC, KS_MODE_PPC64 | KS_MODE_LITTLE_ENDIAN, ks);
  if (!strcmp(arch, "ppc64be")) return ks_open(KS_ARCH_PPC, KS_MODE_PPC64 | KS_MODE_BIG_ENDIAN, ks);
  if (!strcmp(arch, "sparc")) return ks_open(KS_ARCH_SPARC, KS_MODE_SPARC32 | KS_MODE_LITTLE_ENDIAN, ks);
  if (!strcmp(arch, "sparcbe")) return ks_open(KS_ARCH_SPARC, KS_MODE_SPARC32 | KS_MODE_BIG_ENDIAN, ks);
  if (!strcmp(arch, "sparc64be")) return ks_open(KS_ARCH_SPARC, KS_MODE_SPARC64 | KS_MODE_BIG_ENDIAN, ks);
  if (!strcmp(arch, "systemz")) return ks_open(KS_ARCH_SYSTEMZ, KS_MODE_BIG_ENDIAN, ks);
  if (!strcmp(arch, "evm")) return ks_open(KS_ARCH_EVM, KS_MODE_LITTLE_ENDIAN, ks); 
  printf("Unsupported architecture: %s\n", arch);
  printf("List of architectures:\n%s", list_arch);
  return (ks_err)-1;
}
