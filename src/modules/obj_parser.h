#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#ifdef __linux__
#define _POSIX_C_SOURCE 200809L
#endif

#include "../3DViewer_v1.0.h"

#define REALLOC_SHIFT 1000

int parse_file(Data* data, const char* file_name);

int parse_run(Data* data, FILE* file);
int parse_v(Data* data, const char* str);
int parse_f(Data* data, char* str);
int sort_indexes(Data* data);
int index_comparison(const void* x1, const void* x2);
int remove_duplicate_indexes(Data* data);
bool valid_index(int index, size_t vertex_n);
int realloc_vertexes(Data* data);
int realloc_edges(Data* data);
void clear_data(Data* data);

#endif  // OBJ_PARSER_H