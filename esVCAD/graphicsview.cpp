#include "graphicsview.h"

#include <qmath.h>

GraphicsView::GraphicsView(QWidget *parent) :
    QGraphicsView(parent), m_bDraging(false), m_ratio(1.0) {
}

GraphicsView::~GraphicsView() {
}
void GraphicsView::wheelEvent(QWheelEvent *event) {
  qreal v = event->delta();
  v /= 120;
  m_ratio = qPow(1.15, v);
  scale(m_ratio, m_ratio);
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
  QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {
  QGraphicsView::mouseMoveEvent(event);
}
