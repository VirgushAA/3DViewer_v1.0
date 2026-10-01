#ifndef DATA_MAPPER_H
#define DATA_MAPPER_H

#include "../3DViewer_v1.0.h"

typedef enum { X, Y, Z, ALL } Axis;

typedef enum { SHIFT, SCALE, ROTATE } Action;

typedef struct {
  void (*callback)(Data* data, size_t index, Axis axis, double value);
} callback;

int transform_data(Data* data, Axis axis, Action action, double value);
void fill_max_values(Data* data, size_t index);

/* Callback functions */
void shift(Data* data, size_t index, Axis axis, double value);
void scale(Data* data, size_t index, Axis axis, double value);
void rotate(Data* data, size_t index, Axis axis, double value);

#endif  // DATA_MAPPER_H