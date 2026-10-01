#include "mainwindow.h"

#include "openglwidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  QCoreApplication::setOrganizationName(ORG_NAME);
  QCoreApplication::setOrganizationDomain(ORG_DOMAIN);
  QCoreApplication::setApplicationName(APP_NAME);
  settings = new QSettings();
  loadSettings();
  setUiSettings();
  connect(&fTimer, SIGNAL(timeout()), this, SLOT(make_gif_frames()));
  connect(ui->GLWidget, SIGNAL(scaleChanging()), this, SLOT(renewScale()));
}

MainWindow::~MainWindow() {
  saveSettings();
  clear_data(&ui->GLWidget->data);
  delete gif;
  delete settings;
  delete file_name;
  delete ui;
}

void MainWindow::loadFile() {
  delete[] file_name;
  file_name = NULL;
  std::string str = ui->lineEdit_path->text().toStdString();
  file_name = new char[str.size() + 1];
  std::strncpy(file_name, str.c_str(), str.size());
  int err = parse_file(&ui->GLWidget->data, file_name);
  if (err == E_OK) {
    setFileInfo();
    ui->GLWidget->initializeData();
    ui->GLWidget->update();
    renewScale();
  } else {
    sendErrorMessage(err);
    delete[] file_name;
    file_name = NULL;
  }
}

void MainWindow::setFileInfo() {
  size_t size = getFileSize(file_name);
  char size_str[25] = "", result_str[255] = "";
  char *file_name_str = std::strrchr(file_name, '/') + 1;
  if (size < 100000) {
    std::snprintf(size_str, 25, "%.2f KB", (double)size * 1e-3);
  } else {
    std::snprintf(size_str, 25, "%.2f MB", (double)size * 1e-6);
  }
  std::snprintf(result_str, 255,
                "File: %s\nSize: %s\nVertexes: %ld\nEdges: %ld", file_name_str,
                size_str, ui->GLWidget->data.vertex_n,
                ui->GLWidget->data.edge_n);
  ui->label_fileInfo->setText(
      QString::fromUtf8((const char *)result_str, std::strlen(result_str)));
}

size_t MainWindow::getFileSize(const char *filename) {
  FILE *file_ptr = NULL;
  file_ptr = fopen(filename, "rb");
  fseek(file_ptr, 0, SEEK_END);
  size_t file_size = ftell(file_ptr);
  fclose(file_ptr);
  return file_size;
}

void MainWindow::setDefaultSettings() {
  ui->GLWidget->projection = ui->GLWidget->PERSPECTIVE;
  ui->GLWidget->dot_type = ui->GLWidget->NONE;
  ui->GLWidget->line_type = ui->GLWidget->SOLID;
  ui->GLWidget->show_axis = false;
  ui->GLWidget->color_dot = QColor(Qt::white);
  ui->GLWidget->color_edge = QColor(Qt::white);
  ui->GLWidget->color_background = QColor(Qt::black);
  ui->GLWidget->dot_size = 1.0;
  ui->GLWidget->line_width = 1.0;
  ui->GLWidget->stipple_type = 0;
}

void MainWindow::setUiSettings() {
  ui->comboBox_projection->setCurrentIndex(ui->GLWidget->projection);
  ui->comboBox_dot_type->setCurrentIndex(ui->GLWidget->dot_type);
  ui->comboBox_line_type->setCurrentIndex(ui->GLWidget->line_type);
  ui->checkBox_show_axis->setChecked(ui->GLWidget->show_axis);
  ui->toolButton_color_dots->setPalette(QPalette(ui->GLWidget->color_dot));
  ui->toolButton_color_edges->setPalette(QPalette(ui->GLWidget->color_edge));
  ui->toolButton_color_background->setPalette(
      QPalette(ui->GLWidget->color_background));
  ui->doubleSpinBox_dot_size->setValue(ui->GLWidget->dot_size);
  ui->doubleSpinBox_line_width->setValue(ui->GLWidget->line_width);
  ui->comboBox_line_stipple->setCurrentIndex(ui->GLWidget->stipple_type);
}

void MainWindow::loadSettings() {
  ui->GLWidget->projection =
      settings->value("Projection", ui->GLWidget->PERSPECTIVE).toInt();
  ui->GLWidget->dot_type =
      settings->value("Dot/type", ui->GLWidget->NONE).toInt();
  ui->GLWidget->line_type =
      settings->value("Line/type", ui->GLWidget->SOLID).toInt();
  ui->GLWidget->show_axis = settings->value("Show/axis", false).toBool();
  ui->GLWidget->color_dot =
      settings->value("Dot/color", QColor(Qt::white)).value<QColor>();
  ui->GLWidget->color_edge =
      settings->value("Line/color", QColor(Qt::white)).value<QColor>();
  ui->GLWidget->color_background =
      settings->value("Background/color", QColor(Qt::black)).value<QColor>();
  ui->GLWidget->dot_size = settings->value("Dot/size", 1.0).toFloat();
  ui->GLWidget->line_width = settings->value("Line/width", 1.0).toFloat();
  ui->GLWidget->stipple_type = settings->value("Line/stipple/type", 0).toInt();
  ;
}

void MainWindow::saveSettings() {
  settings->setValue("Projection", ui->GLWidget->projection);
  settings->setValue("Dot/type", ui->GLWidget->dot_type);
  settings->setValue("Line/type", ui->GLWidget->line_type);
  settings->setValue("Show/axis", ui->GLWidget->show_axis);
  settings->setValue("Dot/color", ui->GLWidget->color_dot);
  settings->setValue("Line/color", ui->GLWidget->color_edge);
  settings->setValue("Background/color", ui->GLWidget->color_background);
  settings->setValue("Dot/size", ui->GLWidget->dot_size);
  settings->setValue("Line/width", ui->GLWidget->line_width);
  settings->setValue("Line/stipple/type", ui->GLWidget->stipple_type);
}

void MainWindow::sendErrorMessage(int err) {
  QMessageBox errorBox = QMessageBox();
  errorBox.setWindowTitle("Error");
  errorBox.setMinimumSize(300, 200);
  switch (err) {
    case E_ARG:
      errorBox.setText("Open file to continue.");
      break;
    case E_FILE:
      errorBox.setText("File doesn't exist.");
      break;
    case E_PARSE:
      errorBox.setText(
          "Couldn't read the fule. Content is invalid or corrupted.");
      break;
    case E_VAL:
      errorBox.setText("File contents incorrect index.");
      break;
    default:
      errorBox.setText("Unknown error occured.");
  }
  errorBox.exec();
}

void MainWindow::on_pushButton_openFile_clicked() {
  QString q_file_name = QFileDialog::getOpenFileName(
      this, tr("Open .obj file"), "~", tr("Files (*.obj)"), 0);
  if (!q_file_name.isEmpty() && !q_file_name.isNull()) {
    ui->lineEdit_path->setText(q_file_name);
  }
}

void MainWindow::on_pushButton_draw_clicked() { loadFile(); }

void MainWindow::on_pushButton_reset_clicked() {
  if (ui->GLWidget->data.vertex_n > 0) {
    ui->GLWidget->initializeData();
    ui->GLWidget->update();
    renewScale();
  } else {
    sendErrorMessage(E_ARG);
  }
}

void MainWindow::on_pushButton_scale_minus_pressed() {
  double copy_scale =
      ui->GLWidget->scale * (1.0 - ui->doubleSpinBox_scale->value() / 100.0);
  if (copy_scale > MIN_SCALE && copy_scale < MAX_SCALE) {
    ui->GLWidget->scale *= 1.0 - ui->doubleSpinBox_scale->value() / 100.0;
    transform_data(&ui->GLWidget->data, ALL, SCALE,
                   1.0 - ui->doubleSpinBox_scale->value() / 100.0);
    ui->GLWidget->update();
  }
  renewScale();
}

void MainWindow::on_pushButton_scale_plus_pressed() {
  double copy_scale =
      ui->GLWidget->scale * (1.0 + ui->doubleSpinBox_scale->value() / 100.0);
  if (copy_scale > MIN_SCALE && copy_scale < MAX_SCALE) {
    ui->GLWidget->scale *= 1.0 + ui->doubleSpinBox_scale->value() / 100.0;
    transform_data(&ui->GLWidget->data, ALL, SCALE,
                   1.0 + ui->doubleSpinBox_scale->value() / 100.0);
    ui->GLWidget->update();
  }
  renewScale();
}

void MainWindow::on_pushButton_shift_x_minus_pressed() {
  transform_data(&ui->GLWidget->data, X, SHIFT,
                 -ui->doubleSpinBox_shift_x->value() / 100.0);
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_shift_x_plus_pressed() {
  transform_data(&ui->GLWidget->data, X, SHIFT,
                 ui->doubleSpinBox_shift_x->value() / 100.0);
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_shift_y_minus_pressed() {
  transform_data(&ui->GLWidget->data, Y, SHIFT,
                 -ui->doubleSpinBox_shift_y->value() / 100.0);
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_shift_y_plus_pressed() {
  transform_data(&ui->GLWidget->data, Y, SHIFT,
                 ui->doubleSpinBox_shift_y->value() / 100.0);
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_shift_z_minus_pressed() {
  transform_data(&ui->GLWidget->data, Z, SHIFT,
                 -ui->doubleSpinBox_shift_z->value() / 100.0);
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_shift_z_plus_pressed() {
  transform_data(&ui->GLWidget->data, Z, SHIFT,
                 ui->doubleSpinBox_shift_z->value() / 100.0);
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_rotation_x_minus_pressed() {
  transform_data(&ui->GLWidget->data, X, ROTATE,
                 -ui->doubleSpinBox_rotation_x->value() * M_PI / 180.0);
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_rotation_x_plus_pressed() {
  transform_data(&ui->GLWidget->data, X, ROTATE,
                 ui->doubleSpinBox_rotation_x->value() * M_PI / 180.0);
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_rotation_y_minus_pressed() {
  transform_data(&ui->GLWidget->data, Y, ROTATE,
                 -ui->doubleSpinBox_rotation_y->value() * M_PI / 180.0);
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_rotation_y_plus_pressed() {
  transform_data(&ui->GLWidget->data, Y, ROTATE,
                 ui->doubleSpinBox_rotation_y->value() * M_PI / 180.0);
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_rotation_z_minus_pressed() {
  transform_data(&ui->GLWidget->data, Z, ROTATE,
                 -ui->doubleSpinBox_rotation_z->value() * M_PI / 180.0);
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_rotation_z_plus_pressed() {
  transform_data(&ui->GLWidget->data, Z, ROTATE,
                 ui->doubleSpinBox_rotation_z->value() * M_PI / 180.0);
  ui->GLWidget->update();
}

void MainWindow::on_toolButton_color_dots_clicked() {
  QColor color = QColorDialog::getColor();
  if (color.isValid()) {
    ui->GLWidget->color_dot = color;
    ui->toolButton_color_dots->setPalette(QPalette(color));
    ui->GLWidget->update();
  }
}

void MainWindow::on_toolButton_color_edges_clicked() {
  QColor color = QColorDialog::getColor();
  if (color.isValid()) {
    ui->GLWidget->color_edge = color;
    ui->toolButton_color_edges->setPalette(QPalette(color));
    ui->GLWidget->update();
  }
}

void MainWindow::on_toolButton_color_background_clicked() {
  QColor color = QColorDialog::getColor();
  if (color.isValid()) {
    ui->GLWidget->color_background = color;
    ui->toolButton_color_background->setPalette(QPalette(color));
    ui->GLWidget->update();
  }
}

void MainWindow::on_comboBox_projection_activated(int index) {
  ui->GLWidget->projection = index;
  ui->GLWidget->update();
}

void MainWindow::on_comboBox_dot_type_activated(int index) {
  ui->GLWidget->dot_type = index;
  ui->GLWidget->update();
}

void MainWindow::on_comboBox_line_type_activated(int index) {
  ui->GLWidget->line_type = index;
  ui->GLWidget->update();
}

void MainWindow::on_doubleSpinBox_dot_size_valueChanged(double arg1) {
  ui->GLWidget->dot_size = (float)arg1;
  ui->GLWidget->update();
}

void MainWindow::on_doubleSpinBox_line_width_valueChanged(double arg1) {
  ui->GLWidget->line_width = (float)arg1;
  ui->GLWidget->update();
}

void MainWindow::on_checkBox_show_axis_stateChanged(int arg1) {
  ui->GLWidget->show_axis = arg1;
  ui->GLWidget->update();
}

void MainWindow::on_comboBox_line_stipple_activated(int index) {
  ui->GLWidget->stipple_type = index;
  ui->GLWidget->update();
}

void MainWindow::on_pushButton_default_settings_clicked() {
  setDefaultSettings();
  setUiSettings();
  ui->GLWidget->update();
}

QString MainWindow::current_date_and_time() {
  QDateTime date = QDateTime::currentDateTime();
  QString formattedTime = date.toString("ddMMyy_hhmmss");
  return formattedTime;
}

void MainWindow::on_pushButton_bmp_clicked() { exportImage("BMP"); }

void MainWindow::on_pushButton_jpeg_clicked() { exportImage("JPEG"); }

void MainWindow::exportImage(const QString ext) {
  QString path = chooseDirectory();
  if (!path.isEmpty() && !path.isNull()) {
    QString str = "screenshot_" + current_date_and_time() + "." + ext.toLower();
    ui->GLWidget->grabFramebuffer().save(path + "/" + str, ext.toUtf8());
    if (QFile::exists(path + "/" + str) == true) {
      sendExportedFileMessage(str);
    } else {
      sendErrorMessage(E_FILE);
    }
  }
}

void MainWindow::make_gif_frames() {
  gif->addFrame(ui->GLWidget->grabFramebuffer().scaled(640, 480), 100);
  miliseconds += 100;
  ui->progressBar_gif->setValue(miliseconds);
  if (miliseconds >= 5000) {
    fTimer.stop();
    save_gif();
    ui->progressBar_gif->setValue(0);
  }
}

void MainWindow::save_gif() {
  QString path = chooseDirectory();
  if (!path.isEmpty() && !path.isNull()) {
    QString str = "gif_" + current_date_and_time() + ".gif";
    gif->save(path + "/" + str);
    if (QFile::exists(path + "/" + str) == true) {
      sendExportedFileMessage(str);
    } else {
      sendErrorMessage(E_FILE);
    }
  }
}

void MainWindow::on_pushButton_gif_clicked() {
  delete gif;
  miliseconds = 0;
  gif = new QGifImage();
  gif->setDefaultDelay(1000 / 10);
  gif->loopCount();
  fTimer.start(1000 / 10);
}

void MainWindow::sendExportedFileMessage(const QString filename) {
  QMessageBox messageBox = QMessageBox();
  messageBox.setWindowTitle("Success");
  messageBox.setMinimumSize(300, 200);
  messageBox.setText("File\n" + filename + "\nexported successfully.");
  messageBox.exec();
}

void MainWindow::on_checkBox_show_move_camera_z_stateChanged(int arg1) {
  ui->GLWidget->move_camera_z = arg1;
  ui->GLWidget->initializeData();
  ui->GLWidget->update();
}

void MainWindow::renewScale() {
  ui->label_scale_value->setText(
      QString::number(ui->GLWidget->scale * 100.0 / 4.0, 'f', 2));
}

QString MainWindow::chooseDirectory() {
  return QFileDialog::getExistingDirectory(this, ("Select directory"),
                                           QDir::currentPath(),
                                           QFileDialog::ShowDirsOnly);
}
