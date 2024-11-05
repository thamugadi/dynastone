#include <stdio.h>
#include <string.h>
#include "include/globals.h"
#include "include/asm.h"
#include "include/parser.h"
#include <keystone/keystone.h>

int main(int argc, char** argv)
{
  if (argc < 3) {
    printf("usage: ajit <arch> <code> <emit_function_8> <emit_function_16> <emit_function_32> <emit_function_64>\n");
    exit(0);
  }
  bool direct = true;
  int i = 0;
  while(argv[2][i]) {
    if (argv[2][i] == SPECIAL) {
      direct = false;
    }
    i++;
  }
  char *emit_8, *emit_16, *emit_32, *emit_64;

  if (argc < 4) emit_8 = emit_8_default; else emit_8 = argv[3];
  if (argc < 5) emit_16 = emit_16_default; else emit_16 = argv[4];
  if (argc < 6) emit_32 = emit_32_default; else emit_32 = argv[5];
  if (argc < 7) emit_64 = emit_64_default; else emit_64 = argv[6];
  
  char* arch = argv[1];
  char* instr = argv[2];
  ks_engine *ks;
  
  ks_open_arch(&ks, arch);

  char* c_code = NULL;
  uint8_t* bytes;
  
  if (direct) {
    //todo: allow for grouping
    size_t size;
    size_t c;
    char buffer[0x100];
    c_code = (char*)calloc(1, MAX_C_CODE_LEN);
    ks_asm(ks, instr, 0, &bytes, &size, &c);
    for (i = 0; i < size; i++) {
      sprintf(buffer, "%s(%02x);\n", emit_8, bytes[i]);
      strcat(c_code, buffer);
    }
    printf("%s", c_code);
    exit(0);
  }
  
  parsed_data* pdata;
  
  bytes = compute_delimitations(ks, is_big_endian_architecture(arch), instr, &pdata);
  chunk_struct* chunk = make_chunks(pdata, bytes, pdata->binary_size);
  chunk_struct* lv_chunk = make_lv_chunks(chunk, pdata);
  c_code = generate_c_code(lv_chunk, emit_8, emit_16, emit_32, emit_64);

  printf("%s", c_code);
  
  free(c_code);
  free_chunks(chunk);
  free_chunks(lv_chunk);
  free_parsed_data(pdata);
  ks_free(bytes);
  ks_close(ks);

  exit(0);
}
