#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget{parent} {
  setWindowTitle("Viewer");
}

void OpenGLWidget::initializeData() {
  if (data.vertex_n > 0) {
    scale = 2.0;
    calculateRotateSpeed();
    centralize();
    normalize();
  }
}

void OpenGLWidget::initializeGL() {
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);
}

void OpenGLWidget::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);
  ratio = (double)w / (double)h;
}

void OpenGLWidget::paintGL() {
  glClearColor(color_background.redF(), color_background.greenF(),
               color_background.blueF(), color_background.alphaF());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  double shift = scale;
  if (move_camera_z == true) {
    shift = scale > data.max_z ? scale : data.max_z;
  }
  if (projection == PERSPECTIVE) {
    glFrustum(-1.0 * ratio, 1.0 * ratio, -1.0, 1.0, NEAR_POINT * shift,
              FAR_POINT * shift);
  } else if (projection == PARALLEL) {
    glOrtho(-2.0 * ratio, 2.0 * ratio, -2.0, 2.0, NEAR_POINT * shift,
            FAR_POINT * shift);
  }
  glTranslatef(0, 0, -2.0 * shift);
  if (show_axis == true) drawAxes();
  drawModel();
  glDisable(GL_BLEND);
}

void OpenGLWidget::drawModel() {
  glVertexPointer(3, GL_DOUBLE, 0, data.vertexes);
  glEnableClientState(GL_VERTEX_ARRAY);
  glPointSize(dot_size);
  glColor4f(color_dot.redF(), color_dot.greenF(), color_dot.blueF(),
            color_dot.alphaF());
  if (dot_type == CIRCLE) {
    glEnable(GL_POINT_SMOOTH);
    glDrawArrays(GL_POINTS, 0, data.vertex_n);
    glDisable(GL_POINT_SMOOTH);
  } else if (dot_type == SQUARE) {
    glDrawArrays(GL_POINTS, 0, data.vertex_n);
  }

  glLineWidth(line_width);
  glColor4f(color_edge.redF(), color_edge.greenF(), color_edge.blueF(),
            color_edge.alphaF());
  if (line_type == DOTTED) {
    glLineStipple(patterns[stipple_type].factor,
                  patterns[stipple_type].pattern);
    glEnable(GL_LINE_STIPPLE);
    glDrawElements(GL_LINES, data.edge_n * 2, GL_UNSIGNED_INT, data.edges);
    glDisable(GL_LINE_STIPPLE);
  } else {
    glDrawElements(GL_LINES, data.edge_n * 2, GL_UNSIGNED_INT, data.edges);
  }
  glDisableClientState(GL_VERTEX_ARRAY);
}

void OpenGLWidget::drawAxes() {
  double dots[] = {2.0 * ratio, 0.0, 0.0, -2.0 * ratio, 0.0, 0.0, 0.0,
                   2.0 * ratio, 0.0, 0.0, -2.0 * ratio, 0.0, 0.0, 0.0,
                   2.0 * ratio, 0.0, 0.0, -2.0 * ratio};
  int axis[] = {0, 1, 2, 3, 4, 5};
  glVertexPointer(3, GL_DOUBLE, 0, dots);
  glEnableClientState(GL_VERTEX_ARRAY);
  glLineWidth(2.0);
  glColor4f(1.0, 0.0, 0.0, 1.0);
  glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, axis);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void OpenGLWidget::normalize() {
  double normal = 0.0;
  for (size_t i = 0; i < data.vertex_n; i++) {
    if (normal < fabs(data.vertexes[i])) normal = fabs(data.vertexes[i]);
    if (normal < fabs(data.vertexes[i + 1]))
      normal = fabs(data.vertexes[i + 1]);
  }
  if (data.max_z > normal) normal = data.max_z;
  transform_data(&data, ALL, SCALE, 1.0 / normal);
}

void OpenGLWidget::centralize() {
  double min_x = data.vertexes[0], max_x = data.vertexes[0],
         min_y = data.vertexes[1], max_y = data.vertexes[1],
         min_z = data.vertexes[2], max_z = data.vertexes[2];
  for (size_t i = 0; i < data.vertex_n; i++) {
    if (data.vertexes[i * 3] < min_x) min_x = data.vertexes[i * 3];
    if (data.vertexes[i * 3] > max_x) max_x = data.vertexes[i * 3];
    if (data.vertexes[i * 3 + 1] < min_y) min_y = data.vertexes[i * 3 + 1];
    if (data.vertexes[i * 3 + 1] > max_y) max_y = data.vertexes[i * 3 + 1];
    if (data.vertexes[i * 3 + 2] < min_z) min_z = data.vertexes[i * 3 + 2];
    if (data.vertexes[i * 3 + 2] > max_z) max_z = data.vertexes[i * 3 + 2];
  }
  transform_data(&data, X, SHIFT, -(min_x + max_x) / 2.0);
  transform_data(&data, Y, SHIFT, -(min_y + max_y) / 2.0);
  transform_data(&data, Z, SHIFT, -(min_z + max_z) / 2.0);
}

void OpenGLWidget::mousePressEvent(QMouseEvent* move) { mPos = move->pos(); }

void OpenGLWidget::mouseMoveEvent(QMouseEvent* move) {
  xRot += 0.0002 / M_PI * (move->pos().y() - mPos.y());
  yRot += 0.0002 / M_PI * (move->pos().x() - mPos.x());
  if (fabs(xRot) > rotateSpeed || fabs(yRot) > rotateSpeed) {
    transform_data(&data, X, ROTATE, xRot);
    transform_data(&data, Y, ROTATE, yRot);
    xRot = 0.0;
    yRot = 0.0;
    update();
  }
}

void OpenGLWidget::wheelEvent(QWheelEvent* move) {
  double steps = (move->angleDelta().y() % 180) / 180.0;
  double copy_scale = scale;
  copy_scale *= 1.0 + steps;
  if (copy_scale > MIN_SCALE && copy_scale < MAX_SCALE) {
    scale *= 1.0 + steps;
    transform_data(&data, ALL, SCALE, 1.0 + steps);
    update();
  }
  emit scaleChanging();
}

void OpenGLWidget::calculateRotateSpeed() {
  if (data.vertex_n > 1000000) {
    rotateSpeed = 0.1;
  } else if (data.vertex_n > 300000) {
    rotateSpeed = 0.01;
  } else {
    rotateSpeed = 0.001;
  }
}
