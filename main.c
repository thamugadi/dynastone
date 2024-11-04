#include <stdio.h>
#include <string.h>
#include "include/globals.h"
#include "include/asm.h"
#include "include/parser.h"
#include <keystone/keystone.h>

int main(int argc, char** argv)
{
  //court-circuiter le truc si pas de `
  ks_engine *ks;

  ks_open_arch(&ks, "x64");
  char* instr = (char*)calloc(1,0x100);
  // strcpy(instr, "addi `var1, 5`, `var2, 5` , `var3, 16`");
  strcpy(instr, "mov eax, `var1, 64`");
  parsed_data* x;
  uint8_t* bytes = compute_delimitations(ks, false, instr, &x);
  chunk_struct* chunk = make_chunks(x, bytes, x->binary_size);

  make_lv_chunks(chunk);
  free_chunks(chunk);
  
  free_parsed_data(x);
  
  ks_free(bytes);
  
  free(instr);

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
