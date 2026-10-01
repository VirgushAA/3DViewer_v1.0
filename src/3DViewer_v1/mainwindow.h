#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <openglwidget.h>

#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QGuiApplication>
#include <QImage>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QSettings>
#include <cstdio>

#include "qgifimage.h"

#define ORG_NAME "21 School"
#define ORG_DOMAIN "none"
#define APP_NAME "3DViewer_v1.0"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 private slots:
  void on_pushButton_openFile_clicked();
  void on_pushButton_draw_clicked();
  void on_pushButton_reset_clicked();

  void on_pushButton_scale_minus_pressed();
  void on_pushButton_scale_plus_pressed();
  void on_pushButton_shift_x_minus_pressed();
  void on_pushButton_shift_x_plus_pressed();
  void on_pushButton_shift_y_minus_pressed();
  void on_pushButton_shift_y_plus_pressed();
  void on_pushButton_shift_z_minus_pressed();
  void on_pushButton_shift_z_plus_pressed();
  void on_pushButton_rotation_x_minus_pressed();
  void on_pushButton_rotation_x_plus_pressed();
  void on_pushButton_rotation_y_minus_pressed();
  void on_pushButton_rotation_y_plus_pressed();
  void on_pushButton_rotation_z_minus_pressed();
  void on_pushButton_rotation_z_plus_pressed();

  void on_toolButton_color_dots_clicked();
  void on_toolButton_color_edges_clicked();
  void on_toolButton_color_background_clicked();
  void on_comboBox_projection_activated(int index);
  void on_comboBox_dot_type_activated(int index);
  void on_comboBox_line_type_activated(int index);
  void on_doubleSpinBox_dot_size_valueChanged(double arg1);
  void on_doubleSpinBox_line_width_valueChanged(double arg1);
  void on_checkBox_show_axis_stateChanged(int arg1);
  void on_comboBox_line_stipple_activated(int index);
  void on_pushButton_default_settings_clicked();

  void on_pushButton_bmp_clicked();
  void on_pushButton_jpeg_clicked();
  void on_pushButton_gif_clicked();
  void make_gif_frames();

  void on_checkBox_show_move_camera_z_stateChanged(int arg1);

  void renewScale();

 private:
  char* file_name = NULL;
  QSettings* settings;
  QTimer fTimer;
  QGifImage* gif = NULL;
  int miliseconds = 0;

  void loadFile();
  void setFileInfo();
  size_t getFileSize(const char* filename);
  void setDefaultSettings();
  void setUiSettings();
  void loadSettings();
  void saveSettings();
  void sendErrorMessage(int err);
  void sendExportedFileMessage(const QString filename);
  void save_gif();
  void exportImage(const QString ext);
  QString chooseDirectory();
  QString current_date_and_time();
  Ui::MainWindow* ui;
};
#endif  // MAINWINDOW_H
