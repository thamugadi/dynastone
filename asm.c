#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "include/parser.h"
#include "include/asm.h"

#include <keystone/keystone.h>

char* generate_c_code(chunk_struct* chunk, char* emit_8, char* emit_16, char* emit_32, char* emit_64, bool big_endian) {
  char* c_code = (char*)calloc(1, 0x1000);
  char buffer[0x200];
  return c_code;
}

//TODO: make other kind of chunks, more precise

chunk_struct* make_lv_chunks(chunk_struct* chunk) {
  //chunk_struct* new_chunk = (chunk_struct*)calloc(1, sizeof(chunk_struct));
  int repeat = 1;
  char* repeated_name = NULL;
  int i;
  int k = 0;
  chunk_struct* head = chunk;
  int current_pos = 0;
  struct long_vars {
    char* name;
    size_t size;
    int pos;
  } long_vars[MAX_LONG_VARS];
  while(chunk) {
    if (repeated_name == NULL && !chunk->bit_array[0].is_bit) {
      repeated_name = chunk->bit_array[0].var_part.name;
    }
    else if (chunk->bit_array[0].is_bit == false && repeated_name == chunk->bit_array[0].var_part.name) {
      repeat++;
      for (i = 0; i < 8; i++) {
        if (chunk->bit_array[i].is_bit) {
	  repeat = 1;
	  repeated_name = NULL;
	}
      }
      if (!chunk->next && (repeat == 2 || repeat == 4 || repeat == 8)) {
	long_vars[k].name = repeated_name;
	long_vars[k].pos = current_pos;
	long_vars[k].size = repeat;
	k++;
	repeat = 1;
	repeated_name = NULL;
      }
    }
    else if (((chunk->bit_array[0].is_bit || repeated_name != chunk->bit_array[0].var_part.name) &&
	      (repeat == 2 || repeat == 4 || repeat == 8))) {
      long_vars[k].name = repeated_name;
      long_vars[k].pos = current_pos;
      long_vars[k].size = repeat;
      k++;
      repeat = 1;
      repeated_name = NULL;
    }
    else {
      repeat = 1;
      repeated_name = NULL;
    }
    current_pos++;
    chunk = chunk->next;
  }

  for (i = 0; i < k; i++) {
    printf("X\n");
  }
  
  chunk_struct* new_chunk = (chunk_struct*)calloc(1, sizeof(chunk_struct));

  current_pos = 0;
  int j = 0;
  chunk = head;
  chunk_struct* new_chunk_head = new_chunk;
  while(chunk) {
    if (j < k) {
      if (current_pos == long_vars[j].pos) {
	new_chunk->is_long_var = true;
	new_chunk->lv.name = long_vars[j].name;
	new_chunk->lv.size = long_vars[j].size;
	new_chunk->next = (chunk_struct*)calloc(1, sizeof(chunk_struct));
	new_chunk = new_chunk->next;
	for (i = 0; i < long_vars[j].size; i++) {
	  printf("%d\n", i);
	  if (chunk) chunk = chunk->next;
	}
	j++;
	continue;
      }
      else goto handle_normal;
    }
    else {
      handle_normal:
      for (i = 0; i < 8; i++) {
	new_chunk->is_long_var = false;
	new_chunk->bit_array[i].is_bit = chunk->bit_array[i].is_bit;
	if (new_chunk->bit_array[i].is_bit) {
	  new_chunk->bit_array[i].bit = chunk->bit_array[i].bit;
	}
	else {
	  new_chunk->bit_array[i].var_part.name = chunk->bit_array[i].var_part.name;
	  new_chunk->bit_array[i].var_part.size= chunk->bit_array[i].var_part.size;
	  new_chunk->bit_array[i].var_part.var_bit = chunk->bit_array[i].var_part.var_bit;
	}
	new_chunk->next = (chunk_struct*)calloc(1, sizeof(chunk_struct));
	new_chunk = new_chunk->next;
      }
    }
    chunk = chunk->next;
    current_pos++;
  }
  return new_chunk_head;
}

//supposes that they differ by one bit, which is 0 in array1 and 1 in array2
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
	chunk->is_long_var = false;
	chunk->bit_array[i].is_bit = false;
	chunk->bit_array[i].var_part.name = pdata->name;
	chunk->bit_array[i].var_part.size = pdata->size;
	chunk->bit_array[i].var_part.var_bit = current_bit - pdata->binary_pos;
	if (current_bit == pdata->binary_pos + pdata->size - 1) pdata = pdata->next;
      }
      else {
	chunk->is_long_var = false;
	chunk->bit_array[i].is_bit = true;
	chunk->bit_array[i].bit = (code[byte] >> (7 - i)) & 1;
      }
    }
    byte++;
  }
  return head;
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
    int err1, err2;
    err1 = ks_asm(ks, filled_instrs[i][0], 0, &assembled, &(*parsed)->binary_size, &count);
    err2 = ks_asm(ks, filled_instrs[i][1], 0, &assembled_mo, &(*parsed)->binary_size, &count);
    if (err1 | err2) {
      printf("failed\n");
      exit(0);
    }
    (*parsed)->binary_pos = 1 + first_diff_bit(assembled, assembled_mo, (*parsed)->binary_size);
    if (be_arch) (*parsed)->binary_pos -= (*parsed)->size;
    else (*parsed)->binary_pos -= 8;
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
    }
    i++;
  }
  free(filled_instrs);
  *parsed = initial_parsed;
  return assembled;
}
