#ifndef GRAPHICSFRAME_H
#define GRAPHICSFRAME_H

#include "entities/block.h"
#include "entities/coordxy.h"
#include "entities/rect.h"

#include <QFrame>
#include <QImage>
#include <QWheelEvent>

using namespace Entities;

class GraphicsFrame : public QFrame {
  Q_OBJECT
 public:
  GraphicsFrame(QWidget *parent);
  ~GraphicsFrame();

 private:
 private:
  void paintEvent(QPaintEvent *event);
  //鼠标事件
  void mouseDoubleClickEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

 private:
  QImage m_Image;
  Entity *m_curHoverEntity;

 public:
  void PaintEntities(std::vector<Block *> &blocks);
  Entity *GetPickedEntity(std::vector<Block *> &blocks, const QPointF &point);
};

#endif // GRAPHICSFRAME_H
