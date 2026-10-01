#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QColor>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QTimer>
#include <QWheelEvent>

#define MAX_SCALE 400.0
#define MIN_SCALE 0.04
#define FAR_POINT 1000.0
#define NEAR_POINT 1.0

extern "C" {
#include "../modules/data_mapper.h"
#include "../modules/obj_parser.h"
}

class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions {
  Q_OBJECT
 public:
  OpenGLWidget(QWidget* parent = nullptr);

  typedef enum { PERSPECTIVE, PARALLEL } Projection;
  typedef enum { SOLID, DOTTED } Line_type;
  typedef enum { NONE, CIRCLE, SQUARE } Dot_type;

  QColor color_dot = Qt::white;
  QColor color_edge = Qt::white;
  QColor color_background = Qt::black;  // QColor(222, 167, 60, 255);
  int projection = PERSPECTIVE;
  int line_type = SOLID;
  int dot_type = NONE;
  float line_width = 1.0;
  float dot_size = 1.0;
  bool show_axis = false;
  int stipple_type = 0;
  bool move_camera_z = false;

  Data data = {0, NULL, 0, NULL, 0.0};
  double scale = 2.0;
  double rotateSpeed = 0.01;

  void initializeData();
 signals:
  void scaleChanging();

 protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;

 private:
  struct line_stipple {
    GLint factor;
    GLushort pattern;
  };

  const struct line_stipple patterns[8] = {
      {1, 0x00FF}, {2, 0x00FF}, {1, 0x0C0F}, {3, 0x0C0F},
      {1, 0xAAAA}, {2, 0xAAAA}, {3, 0xAAAA}, {4, 0xAAAA}};
  double ratio = 1.0, max_z = 0.0, normal = 1.0;
  double xRot = 0.0, yRot = 0.0;

  QPoint mPos;
  void mousePressEvent(QMouseEvent* move) override;
  void mouseMoveEvent(QMouseEvent* move) override;
  void wheelEvent(QWheelEvent* move) override;
  void drawModel();
  void drawAxes();
  void centralize();
  void normalize();
  void calculateRotateSpeed();
};

#endif  // OPENGLWIDGET_H
