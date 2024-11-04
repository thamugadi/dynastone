#pragma once
#include "parser.h"
#include <keystone/keystone.h>
#include <stdint.h>
#define MAX_EMITTED_BYTES 0x400
#define MAX_C_CODE_LEN 0x1000
#define MAX_LONG_VARS 0x20
typedef struct long_var {
  char* name;
  size_t size;
} long_var;

typedef struct bit_entry {
  char* name;
  size_t size;
  uint8_t var_bit;
} bit_entry;

typedef struct var_content {
  bool is_bit;
  union {
    uint8_t bit;
    struct bit_entry var_part;
  };
} var_content;

typedef struct chunk_struct {
  bool is_long_var;
  union {
    var_content bit_array[8];
    long_var lv;
  };
  struct chunk_struct* next;
} chunk_struct;

chunk_struct* make_lv_chunks(chunk_struct* chunk); 
char* generate_c_code(chunk_struct* chunk, char* emit_8, char* emit_16, char* emit_32, char* emit_64, bool big_endian); 
chunk_struct* make_chunks(parsed_data* pdata, uint8_t* code, size_t code_size);
void free_chunks(chunk_struct* chunk);
uint8_t* compute_delimitations(ks_engine* ks, bool big_endian, char* instr, parsed_data** parsed);
