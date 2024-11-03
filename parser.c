#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/parser.h"

void free_parsed_data(parsed_data* p) {
  if (p->name) free(p->name);
  if (p->next) free_parsed_data(p->next);
  free(p);
}

//todo: add error handling
int replace_section(char* input_str, int start, int end, char* replacement) {
  int replacement_len = strlen(replacement);
  int section_len = end - start + 1;
  int tail_len = strlen(input_str) - end - 1;

  memmove(input_str + start + replacement_len, input_str + end + 1, tail_len + 1);
  memcpy(input_str + start, replacement, replacement_len);
  return replacement_len - section_len;
}

//todo: add error handling
char** replace_instrs(char* instr, parsed_data* parsed, int n){
  char** replaced_instrs = (char**)malloc(sizeof(char*)*2);
  replaced_instrs[0] = (char*)calloc(1,INSTR_MAX_SIZE);
  replaced_instrs[1] = (char*)calloc(1,INSTR_MAX_SIZE);

  strcpy(replaced_instrs[0], instr);
  strcpy(replaced_instrs[1], instr);

  int i = 0;
  int offset = 0;
  int tmp = 0;
  while(parsed) {
    uint64_t test_value;
    if (parsed->size != 64)
      test_value = ((1ULL << parsed->size) - 1) & (~(1ULL << (parsed->size - 1))); // clear_msb(2^size - 1)
    else
      test_value = 9223372036854775807;
    char* test_value_str = (char*)calloc(1, SIZE_MAX_DIGITS);
    sprintf(test_value_str, "0x%lx", test_value);
    if (i == n) {
      uint64_t test_value_mo = test_value - 1;
      char* test_value_mo_str = (char*)calloc(1,SIZE_MAX_DIGITS);
      sprintf(test_value_mo_str, "0x%lx", test_value_mo);
      tmp = replace_section(replaced_instrs[0], offset + parsed->pos, offset + parsed->end, test_value_str); 
      replace_section(replaced_instrs[1], offset + parsed->pos, offset + parsed->end, test_value_mo_str); 
      offset += tmp;
      free(test_value_mo_str);
    }
    else {
      tmp = replace_section(replaced_instrs[0], offset + parsed->pos, offset + parsed->end, test_value_str); 
      replace_section(replaced_instrs[1], offset + parsed->pos, offset + parsed->end, test_value_str); 
      offset += tmp;
    }
    free(test_value_str);
    i++;
    parsed = parsed->next;
  }
  return replaced_instrs;
}

parsed_data* parse(const char* input, int* ll_size) {
  /*  int i = 0;
  bool a = false;
  while(input[i]) {
    if (input[i] == '`') {
      a = true;
      break;
    }
    i++;
  }
  if (!a) return NULL;
  */
  parsed_data* variables_ll = (parsed_data*)calloc(1, sizeof(parsed_data));
  parsed_data* last_ll;
  parsed_data* head = variables_ll;
  int pos = 0;
  int in_backtick_expr = 0;
  int name_part;

  int ll_size_loc = 0;
  
  int name_pos = 0;
  int need_to_free = 0;
  while (input[pos]) {
    if (in_backtick_expr) {
      if (input[pos] == '`') {
	need_to_free = 1;
	variables_ll->end = pos;
	variables_ll->next = (parsed_data*)calloc(1, sizeof(parsed_data));
	ll_size_loc++;
	last_ll = variables_ll;
	variables_ll = variables_ll->next;
	in_backtick_expr = 0;
	pos++;
      }
      else if (input[pos] == ',') {
	(variables_ll->name)[name_pos] = 0;
	variables_ll->size = strtol(input+pos+1, NULL, 10);
	name_part = 0;
	pos++;
      }
      else if (name_part) {
	if (name_pos >= 0xff) return NULL;
	(variables_ll->name)[name_pos++] = input[pos++];
      }
      else {
	pos++;
      }
    }
    else {
      if (input[pos] == '`') {
	in_backtick_expr = 1;
	name_part = 1;
	name_pos = 0;
	variables_ll->name = (char*)calloc(1,INSTR_MAX_SIZE);
	variables_ll->pos = pos;
	pos++;
      }
      else {
	pos++;
      }
    }
  }
  if (head->name == NULL) {
    free(variables_ll);
    return NULL;
  }
  else {
    variables_ll->next = NULL;
    if (ll_size) *ll_size = ll_size_loc;
    if (need_to_free) {
      free(variables_ll);
      last_ll->next = NULL;
    }
    return head;
  }
}
