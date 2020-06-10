#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "esVCAD.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsScene>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMimeData>
#include <string>

using namespace Entities;

#if _MSC_VER >= 1600
#  pragma execution_character_set("utf-8")
#endif
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_pushButtonSelFile_clicked();

  void on_pushButtonDxfParse_clicked();

  void on_pushButtonMoveRight_clicked();

  void on_pushButtonMoveLeft_clicked();

  void on_pushButtonMoveUp_clicked();

  void on_pushButtonMoveDown_clicked();

  void on_pushButtonZoomOut_clicked();

  void on_pushButtonZoomIn_clicked();
  void on_listWidgetEntities_clicked(const QModelIndex &index);

  void on_pushButtonClearSel_clicked();

  void on_listWidgetEntities_currentItemChanged(
      QListWidgetItem *current, QListWidgetItem *previous);

 public:
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void wheelEvent(QWheelEvent *event);

 protected:
  virtual void dragEnterEvent(QDragEnterEvent *event);
  virtual void dropEvent(QDropEvent *event);

 private:
  Ui::MainWindow *ui;
  QString m_sFileName;
  QPoint m_dragBeginPoint;
  QPoint m_dragEndPoint;
  QPoint m_scaleBasePoint;
  EsvCad m_esvCad;
  QGraphicsScene *m_scene;

 private:
  void ParseDxf(const string &fileNam);
  void ShowEntities(std::vector<Block *> &blocks);
};

#endif // MAINWINDOW_H
