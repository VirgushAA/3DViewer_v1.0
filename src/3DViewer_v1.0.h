#ifndef _3DVIEWER_V1_0_H
#define _3DVIEWER_V1_0_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum return_codes { E_OK, E_ARG, E_FILE, E_MEM, E_PARSE, E_TEST, E_VAL };

typedef struct {
  size_t vertex_n;
  double *vertexes;
  size_t edge_n;
  int *edges;
  double max_z;
} Data;

#endif  // _3DVIEWER_V1_0_H