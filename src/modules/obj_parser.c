#include "obj_parser.h"

/*
Returns E_OK if fine, if any error occurs, it returns code and purges data.
*/
int parse_file(Data* data, const char* file_name) {
  int ret = E_OK;
  FILE* file = NULL;

  if (data == NULL || file_name == NULL) {
    ret = E_ARG;
  } else {
    file = fopen(file_name, "r");
    if (file == NULL) {
      ret = E_FILE;
    } else {
      clear_data(data);
      ret = parse_run(data, file);
      if (ret != E_OK)
        clear_data(data);
      else if (data->edge_n > 0) {
        sort_indexes(data);
        ret = remove_duplicate_indexes(data);
      }
      fclose(file);
    }
  }
  return ret;
}

int parse_run(Data* data, FILE* file) {
  int ret = E_OK;
  char* line = NULL;
  size_t line_n = 0;
  while (ret == E_OK && getline(&line, &line_n, file) != EOF) {
    if (strncmp(line, "v ", 2) == 0) {
      ret = parse_v(data, line + 2);
    } else if (strncmp(line, "f ", 2) == 0) {
      ret = parse_f(data, line + 2);
    }
    free(line);
    line = NULL;
  }
  free(line);
  line = NULL;
  if (data->vertexes == NULL || data->vertex_n == 0) ret = E_PARSE;
  return ret;
}

int parse_v(Data* data, const char* str) {
  int ret = E_OK;
  double x = 0.0, y = 0.0, z = 0.0;
  if (data->vertex_n % REALLOC_SHIFT == 0) ret = realloc_vertexes(data);
  if (ret == E_OK) {
    if (sscanf(str, "%lf %lf %lf", &x, &y, &z) == 3) {
      data->vertexes[data->vertex_n * 3] = x;
      data->vertexes[data->vertex_n * 3 + 1] = y;
      data->vertexes[data->vertex_n * 3 + 2] = z;
      if (fabs(z) > data->max_z) data->max_z = fabs(z);
      data->vertex_n++;
    }
  }
  return ret;
}

int parse_f(Data* data, char* str) {
  int ret = E_OK;
  char* ptr = strtok(str, " ");
  int first = atoi(ptr);
  int prev = first;
  int index_1, index_2, flag = prev;
  ptr = strtok(NULL, " ");
  if (first != 0) {
    while (ptr != NULL && flag != 0 && ret == E_OK) {
      if (data->edge_n % REALLOC_SHIFT == 0) ret = realloc_edges(data);
      index_1 = prev < 0 ? (int)data->vertex_n + prev : prev - 1;
      flag = atoi(ptr);
      if (flag != 0 && ret == E_OK) {
        prev = flag;
        index_2 = prev < 0 ? (int)data->vertex_n + prev : prev - 1;
        data->edges[data->edge_n * 2] = index_1 > index_2 ? index_2 : index_1;
        data->edges[data->edge_n * 2 + 1] =
            index_1 > index_2 ? index_1 : index_2;
        data->edge_n++;
        ptr = strtok(NULL, " ");
      }
    }
    if (prev != first) {
      if (data->edge_n % REALLOC_SHIFT == 0) ret = realloc_edges(data);
      if (ret == E_OK) {
        index_1 = prev < 0 ? (int)data->vertex_n + prev : prev - 1;
        index_2 = first < 0 ? (int)data->vertex_n + first : first - 1;
        data->edges[data->edge_n * 2] = index_1 > index_2 ? index_2 : index_1;
        data->edges[data->edge_n * 2 + 1] =
            index_1 > index_2 ? index_1 : index_2;
        data->edge_n++;
      }
    }
  }
  return ret;
}

int sort_indexes(Data* data) {
  int ret = E_OK;
  qsort(data->edges, data->edge_n, sizeof(int) * 2, index_comparison);
  return ret;
}

int remove_duplicate_indexes(Data* data) {
  int ret = E_OK, prev_1 = 0, prev_2 = 0;
  size_t new_edge_n = 0;
  int* buffer = (int*)malloc(sizeof(int) * 2 * data->edge_n);
  if (buffer == NULL) {
    ret = E_MEM;
  } else {
    for (size_t i = 0; i < data->edge_n * 2 && ret == E_OK; i += 2) {
      if ((prev_1 != data->edges[i] || prev_2 != data->edges[i + 1]) &&
          data->edges[i] != data->edges[i + 1]) {
        ret = valid_index(data->edges[i], data->vertex_n) &&
                      valid_index(data->edges[i + 1], data->vertex_n)
                  ? E_OK
                  : E_VAL;
        prev_1 = data->edges[i];
        prev_2 = data->edges[i + 1];
        buffer[new_edge_n * 2] = data->edges[i];
        buffer[new_edge_n * 2 + 1] = data->edges[i + 1];
        new_edge_n++;
      }
    }
    if (ret == E_OK) {
      int* ptr = (int*)realloc(buffer, sizeof(int) * 2 * new_edge_n);
      if (ptr != NULL) {
        free(data->edges);
        data->edges = ptr;
        data->edge_n = new_edge_n;
      } else {
        ret = E_MEM;
        free(buffer);
      }
    } else {
      free(buffer);
    }
  }
  return ret;
}

int index_comparison(const void* x1, const void* x2) {
  int ret = 0;
  ret = (*(int*)x1 - *(int*)x2);
  if (ret == 0) {
    ret = (*(int*)(x1 + sizeof(int)) - *(int*)(x2 + sizeof(int)));
  }
  return ret;
}

bool valid_index(int index, size_t vertex_n) {
  return index >= 0 && index < (int)vertex_n;
}

int realloc_vertexes(Data* data) {
  int ret = E_OK;
  double* ptr = NULL;
  ptr = (double*)realloc(data->vertexes,
                         sizeof(double) * (data->vertex_n + REALLOC_SHIFT) * 3);
  if (ptr == NULL) {
    ret = E_MEM;
  } else {
    data->vertexes = ptr;
  }
  return ret;
}

int realloc_edges(Data* data) {
  int ret = E_OK, *ptr = NULL;
  ptr = (int*)realloc(data->edges,
                      sizeof(int) * (data->edge_n + REALLOC_SHIFT) * 2);
  if (ptr == NULL) {
    ret = E_MEM;
  } else {
    data->edges = ptr;
  }
  return ret;
}

void clear_data(Data* data) {
  if (data != NULL) {
    free(data->edges);
    free(data->vertexes);
    data->vertexes = NULL;
    data->vertex_n = 0;
    data->edges = NULL;
    data->edge_n = 0;
    data->max_z = 0.0;
  }
}
