#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "include/parser.h"
#include "include/asm.h"

#include <keystone/keystone.h>
char* generate_c_code(chunk_struct* chunk, char* emit_8) {
  char* c_code = (char*)calloc(1,0x1000);
  char buffer[0x200];
  uint8_t base = 0;
  char* current_var = NULL;
  int current_start;
  int current_end;
  int i;
  char not_var[8];
  do {
    memset(not_var, 0, 8);
    for (i = 0; i < 8; i++) {
      if (chunk->bit_array[i].is_bit) {
        base |= (chunk->bit_array[i].bit << (7 - i));
	not_var[i] = 0;
      }
      else not_var[i] = 1;
    }
    //todo: clear the bits that were not found
    sprintf(buffer, (char*)"%s(0x%02x", emit_8, base);
    strcat(c_code, buffer);
    for (i = 0; i < 8; i++) {
      if (!chunk->bit_array[i].is_bit) {
	if (chunk->bit_array[i].var_part.name == current_var && chunk->next) {
	  current_end = chunk->bit_array[i].var_part.var_bit;
	}
	else {
	  if (current_var != NULL) {
	    //todo: core
	    strcat(c_code, buffer);
	  }
	  current_start = chunk->bit_array[i].var_part.var_bit;
	  current_end = chunk->bit_array[i].var_part.var_bit;
	  current_var = chunk->bit_array[i].var_part.name;
	}
      }
    }
    strcat(c_code, (char*)");\n");
    chunk = chunk->next;
    base = 0;
  } while(chunk);
  return c_code;
}

//supposes that they differ by one bit, which is 0 in array1 and 1 in array2
int first_diff_bit(uint8_t* array1, uint8_t* array2, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (array1[i] != array2[i]) {
      uint8_t diff = array1[i] ^ array2[i];
      if (diff & 0b10000000) return i * 8 + 0;
      if (diff & 0b1000000) return i * 8 + 1;
      if (diff & 0b100000) return i * 8 + 2;
      if (diff & 0b10000) return i * 8 + 3;
      if (diff & 0b1000) return i * 8 + 4;
      if (diff & 0b100) return i * 8 + 5;
      if (diff & 0b10) return i * 8 + 6;
      if (diff & 0b1) return i * 8 + 7;
    }
  }
  return -1;
}

// to run before free_parsed_data
void free_chunks(chunk_struct* chunk) {
  if (chunk->next) {
    free_chunks(chunk->next);
    free(chunk);
  }
  else {
    free(chunk);
  }
}

chunk_struct* make_chunks(parsed_data* pdata, uint8_t* code, size_t code_size) {
  int byte = 0;
  int i = 0;
  chunk_struct* chunk;
  chunk_struct* head;
  int current_bit = 0;
  while (byte < code_size) {
    if (byte == 0) {
      chunk = (chunk_struct*)calloc(1, sizeof(chunk_struct));
      head = chunk;
    }
    else {
      chunk->next = (chunk_struct*)calloc(1, sizeof(chunk_struct));
      chunk = chunk->next;
    }
    for (i = 0; i < 8; i++, current_bit++) {
      if (current_bit >= pdata->binary_pos && current_bit < pdata->binary_pos + pdata->size) {
	chunk->bit_array[i].is_bit = false;
	chunk->bit_array[i].var_part.name = pdata->name;
	chunk->bit_array[i].var_part.size = pdata->size;
	chunk->bit_array[i].var_part.var_bit = current_bit - pdata->binary_pos;
	if (current_bit == pdata->binary_pos + pdata->size - 1) pdata = pdata->next;
      }
      else {
	chunk->bit_array[i].is_bit = true;
	chunk->bit_array[i].bit = (code[byte] >> (7 - i)) & 1;
      }
    }
    byte++;
  }
  return head;
}

uint8_t* compute_delimitations(ks_engine* ks, bool be_arch, char* instr, parsed_data** parsed) {
  int n_vars;
  *parsed = parse(instr, &n_vars);
  parsed_data* initial_parsed = *parsed;
  if (!n_vars) {
    free_parsed_data(*parsed);
    return NULL;
  }
  char*** filled_instrs = (char***)malloc(sizeof(char**)*n_vars);
  uint8_t* assembled;
  uint8_t* assembled_mo;
  int i = 0;
  while(*parsed) {
    filled_instrs[i] = replace_instrs(instr, initial_parsed, i);
    if (!filled_instrs[i]) return NULL;
    size_t count;
    printf("%s\n", filled_instrs[i][0]);
    printf("%s\n", filled_instrs[i][1]);
    int err1, err2;
    err1 = ks_asm(ks, filled_instrs[i][0], 0, &assembled, &(*parsed)->binary_size, &count);
    err2 = ks_asm(ks, filled_instrs[i][1], 0, &assembled_mo, &(*parsed)->binary_size, &count);
    if (err1 | err2) {
      printf("failed\n");
      exit(0);
    }
    (*parsed)->binary_pos = 1 + first_diff_bit(assembled, assembled_mo, (*parsed)->binary_size);
    if (be_arch) (*parsed)->binary_pos -= (*parsed)->size;
    else (*parsed)->binary_pos -= 7;
    free(filled_instrs[i][0]);
    free(filled_instrs[i][1]);
    free(filled_instrs[i]);
    *parsed = (*parsed)->next;
    if (i == n_vars - 1) {
      ks_free(assembled_mo);
      break;
    }
    else {
      ks_free(assembled);
      ks_free(assembled_mo);
      printf("????\n");
      exit(0);
    }
    i++;
  }
  free(filled_instrs);
  *parsed = initial_parsed;
  printf("%p\n", assembled);
  return assembled;
}
