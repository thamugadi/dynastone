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

  if (direct) {
    // directly emit
  }

  char *emit_8, *emit_16, *emit_32, *emit_64 = (char*)calloc(1, 0x40);
  if (argc < 4) strcpy(emit_8, "emit_8"); else strcpy(emit_8, argv[3]);
  if (argc < 5) strcpy(emit_16, "emit_16"); else strcpy(emit_16, argv[4]);
  if (argc < 6) strcpy(emit_32, "emit_32"); else strcpy(emit_32, argv[5]);
  if (argc < 7) strcpy(emit_64, "emit_64"); else strcpy(emit_64, argv[6]);
  
  ks_engine *ks;

  ks_open_arch(&ks, "ppc64be");
  char* instr = (char*)calloc(1,0x100);
  // strcpy(instr, "mov rax, `var, 64`");
  strcpy(instr, "addi |var1, 5|, |var2, 5|, |var3, 16|");
  parsed_data* pdata;
  uint8_t* bytes = compute_delimitations(ks, true, instr, &pdata);
  chunk_struct* chunk = make_chunks(pdata, bytes, pdata->binary_size);
  chunk_struct* lv_chunk = make_lv_chunks(chunk, pdata);

  char* c_code = generate_c_code(lv_chunk, "emit8","emit16","emit32","emit64");

  printf("%s", c_code);
  
  free(c_code);
  free_chunks(chunk);
  free_chunks(lv_chunk);
  free_parsed_data(pdata);
  ks_free(bytes);
  free(instr);
  ks_close(ks);

  exit(0);
  /*  const char* arch = argv[1];
  custom_ks_open(ks, arch);

  parsed_data* ax = parse("addi `var1, 8`, `var2, 11`, `var1, 19`", NULL);
  printf("%s", (ax->next)->size);
  
  
  if (argc != 8) {
    return 0;
  }
  */
  //char* instr = argv[2];
}
