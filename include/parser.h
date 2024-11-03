#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#define INSTR_MAX_SIZE 0x400
#define SIZE_MAX_DIGITS 0x50
typedef struct parsed_data {
  char* name;
  int size;
  int pos;
  int end;

  size_t binary_size;
  int binary_pos;
  struct parsed_data* next;
} parsed_data;

int replace_section(char* input_str, int start, int end, char* replacement);
char** replace_instrs(char* instr, parsed_data* parsed, int n);
parsed_data* parse(const char* input, int* ll_size);
void free_parsed_data(parsed_data* p);
