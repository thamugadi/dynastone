#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "include/parser.h"
#include "include/asm.h"

#include <keystone/keystone.h>

char* generate_c_code(chunk_struct* chunk, char* emit_8, char* emit_16, char* emit_32, char* emit_64, int* size) {
  char* c_code = (char*)calloc(1, 0x1000);
  char buffer[0x200];
  uint8_t base = 0;
  char* current_var = NULL;
  int current_start = -1, current_end = -1;
  int current_end_in_byte = -1;
  int i, shift;
  *size = 0;

  while (chunk->next) {
    if (!chunk->is_long_var) {
      base = 0;
      for (i = 0; i < 8; i++) {
        if (chunk->bit_array[i].is_bit)
          base |= chunk->bit_array[i].bit << (7 - i);
      }

      *size += 1;
      sprintf(buffer, "%s(0x%02x", emit_8, base);
      strcat(c_code, buffer);

      current_var = NULL;

      for (i = 0; i < 8; i++) {
        if (!chunk->bit_array[i].is_bit) {
          char* var_name = chunk->bit_array[i].var_part.name;
          int var_bit = chunk->bit_array[i].var_part.var_bit;

          if (current_var && var_name == current_var &&
              var_bit == current_end + 1) {
            current_end = var_bit;
            current_end_in_byte = i;
          }
          else {
            if (current_var) {
              int size = current_end - current_start + 1;
              int mask = (1 << size) - 1;
              shift = 7 - current_end_in_byte;

	      int len;
	      len = sprintf(buffer, " | ((");
	      if (current_start) {
	        len += sprintf(buffer+len, "(%s >> %d)", current_var, current_start);
	      }
	      else {
	        len += sprintf(buffer+len, "%s", current_var);
	      }
	      if (mask != 0xff) {
	        len += sprintf(buffer+len, " & 0x%02x)", mask); 
	      }
	      else {
		delete_last_char(buffer, '(');
	      }
	      if (shift) {
	        len += sprintf(buffer+len, " << %d)", shift);
	      }
	      else {
		delete_last_char(buffer, '(');
	      }
              strcat(c_code, buffer);
            }
            current_var = var_name;
            current_start = current_end = var_bit;
            current_end_in_byte = i;
          }
        }
        else if (current_var) {
          int size = current_end - current_start + 1;
          int mask = (1 << size) - 1;
          shift = 7 - current_end_in_byte;

	  int len;

	  len = sprintf(buffer, " | ((");
	  if (current_start) {
	    len += sprintf(buffer+len, "(%s >> %d)", current_var, current_start);
	  }
	  else {
	    len += sprintf(buffer+len, "%s", current_var);
	  }
	  if (mask != 0xff) {
	    len += sprintf(buffer+len, " & 0x%02x)", mask); 
	  }
	  else {
	    delete_last_char(buffer, '(');
	  }
	  if (shift) {
	    len += sprintf(buffer+len, " << %d)", shift);
	  }
	  else {
	    delete_last_char(buffer, '(');
	  }
          strcat(c_code, buffer);
          current_var = NULL;
        }
      }
      if (current_var) {
        int size = current_end - current_start + 1;
        int mask = (1 << size) - 1;
        shift = 7 - current_end_in_byte;

	int len = sprintf(buffer, " | ((");

	len = sprintf(buffer, " | ((");

        if (current_start) {
	  len += sprintf(buffer+len, "(%s >> %d)", current_var, current_start);
	}
	else {
          len += sprintf(buffer+len, "%s", current_var);
        }
        if (mask != 0xff) {
          len += sprintf(buffer+len, " & 0x%02x)", mask); 
        }
	else {
	  delete_last_char(buffer, '(');
	}
	if (shift) {
	  len += sprintf(buffer+len, " << %d)", shift);
	}
	else {
	  delete_last_char(buffer, '(');
	}
        strcat(c_code, buffer);
      }

      strcat(c_code, ");\n");
      chunk = chunk->next;
    }
    else { //long_var
      char* emit;
      if (chunk->lv.size == 2) emit = emit_16;
      else if (chunk->lv.size == 4) emit = emit_32;
      else if (chunk->lv.size == 8) emit = emit_64;
      else {
	printf("shouldn't happen: %d size in lv\n", (int)chunk->lv.size);
	exit(0);
      }
      *size += chunk->lv.size;
      sprintf(buffer, "%s(%s);\n", emit, chunk->lv.name);
      strcat(c_code, buffer);
      chunk = chunk->next;
    }
  }
  return c_code;
}

chunk_struct* make_lv_chunks(chunk_struct* chunk, parsed_data* pdata) {
  int i;
  int k = 0;
  chunk_struct* head = chunk;
  int current_pos = 0;
  struct long_vars {
    char* name;
    size_t size;
    int pos;
  } long_vars[MAX_LONG_VARS];

  while(pdata) {
    if (pdata->binary_pos % 8 == 0 && (pdata->size == 16 || pdata->size == 32 || pdata->size == 64)) {
      long_vars[k].name = pdata->name;
      long_vars[k].size = pdata->size / 8;
      long_vars[k].pos = pdata->binary_pos / 8;
      k++;
      pdata = pdata->next;
    }
    else {
      pdata = pdata->next;
    }
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
	  current_pos++;
	  if (chunk) {
	    chunk = chunk->next;
	  }
	  
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
      }
      new_chunk->next = (chunk_struct*)calloc(1, sizeof(chunk_struct));
      new_chunk = new_chunk->next;
    }
    chunk = chunk->next;
    current_pos++;
  }

  return new_chunk_head;
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
	chunk->is_long_var = false;
	chunk->bit_array[i].is_bit = false;
	chunk->bit_array[i].var_part.name = pdata->name;
	chunk->bit_array[i].var_part.size = pdata->size;
	chunk->bit_array[i].var_part.var_bit = current_bit - pdata->binary_pos;
	if (current_bit == pdata->binary_pos + pdata->size - 1) {
	  if (pdata->next) {
	    pdata = pdata->next;
	  }
	}
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
      printf("failed, can't assemble %s and %s\n", filled_instrs[i][0], filled_instrs[i][1]);
      exit(0);
    }
    (*parsed)->binary_pos = 1 + first_diff_bit(assembled, assembled_mo, (*parsed)->binary_size);
    if (be_arch) (*parsed)->binary_pos -= (*parsed)->size;
    else (*parsed)->binary_pos -= 8; // unsure if correct
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
