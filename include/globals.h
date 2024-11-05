#pragma once
#include <keystone/keystone.h>
bool is_big_endian_architecture(const char* arch);
char* must_inverse_bytes(const char* arch);
ks_err ks_open_arch(ks_engine** ks, const char* arch);
extern const char* list_arch;

#define SPECIAL '|'

extern char emit_8_default[], emit_16_default[], emit_32_default[], emit_64_default[];
