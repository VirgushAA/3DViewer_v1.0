#include "data_mapper.h"

const callback callback_list[] = {
    [SHIFT] = {shift}, [SCALE] = {scale}, [ROTATE] = {rotate}};

/*
The argument "axis" can be: X, Y, Z - for shift and rotate;
ALL - use only for scale. The argument "callback" can be: SHIFT, ROTATE, SCALE.
*/
int transform_data(Data* data, Axis axis, Action action, double value) {
  int ret = E_OK;
  if (data != NULL && action >= SHIFT && action <= ROTATE) {
    data->max_z = 0.0;
    for (size_t i = 0; data != NULL && i < data->vertex_n; i++) {
      (callback_list[action].callback)(data, i, axis, value);
      fill_max_values(data, i);
    }
  } else
    ret = E_ARG;
  return ret;
}

void shift(Data* data, size_t index, Axis axis, double value) {
  if (axis >= X && axis <= Z) data->vertexes[index * 3 + axis] += value;
}

void scale(Data* data, size_t index, Axis axis, double value) {
  for (size_t i = 0; i < 3 && axis == ALL && value != 0; i++) {
    data->vertexes[index * 3 + i] *= value;
  }
}

void rotate(Data* data, size_t index, Axis axis, double value) {
  double x = data->vertexes[index * 3], y = data->vertexes[index * 3 + 1],
         z = data->vertexes[index * 3 + 2];
  switch (axis) {
    case X:
      data->vertexes[index * 3 + 1] = y * cos(value) - z * sin(value);
      data->vertexes[index * 3 + 2] = y * sin(value) + z * cos(value);
      break;
    case Y:
      data->vertexes[index * 3] = x * cos(value) + z * sin(value);
      data->vertexes[index * 3 + 2] = -x * sin(value) + z * cos(value);
      break;
    case Z:
      data->vertexes[index * 3] = x * cos(value) - y * sin(value);
      data->vertexes[index * 3 + 1] = x * sin(value) + y * cos(value);
      break;
    case ALL:
    default:;
  }
}

void fill_max_values(Data* data, size_t index) {
  if (fabs(data->vertexes[index * 3 + 2]) > data->max_z)
    data->max_z = fabs(data->vertexes[index * 3 + 2]);
}
