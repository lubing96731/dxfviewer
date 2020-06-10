#include "graphicsframe.h"

#include <QPainter>
#include <math.h>

GraphicsFrame::GraphicsFrame(QWidget *parent) : QFrame(parent) {
  m_curHoverEntity = nullptr;
  setMouseTracking(true);
}

GraphicsFrame::~GraphicsFrame() {
}

void GraphicsFrame::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  //背景黑色
  painter.fillRect(0, 0, width(), height(), QBrush(Qt::black));
  if (!m_Image.isNull()) { painter.drawImage(0, 0, m_Image); }
}
//鼠标事件
void GraphicsFrame::mouseDoubleClickEvent(QMouseEvent *event) {
  QFrame::mouseDoubleClickEvent(event);
}

void GraphicsFrame::mouseMoveEvent(QMouseEvent *event) {
  QFrame::mouseMoveEvent(event);
}

void GraphicsFrame::mousePressEvent(QMouseEvent *event) {
  QFrame::mousePressEvent(event);
}

void GraphicsFrame::mouseReleaseEvent(QMouseEvent *event) {
  QFrame::mouseReleaseEvent(event);
}

void GraphicsFrame::wheelEvent(QWheelEvent *event) {
  if (event->delta()) {
  } else {
  }
  QFrame::wheelEvent(event);
}

Entity *GraphicsFrame::GetPickedEntity(
    std::vector<Block *> &blocks, const QPointF &point) {
  size_t count = blocks.size();
  if (count > 0) {
    for (size_t i = 0; i < count; ++i) {
      Block *block = blocks[i];
      if (!block->IsEmpty()) {
        RectF rect = block->BoundingRect();
        if (rect.contains(point)) {
          size_t elemSize = block->GetElementSize();
          for (size_t k = 0; k < elemSize; ++k) {
            Entity *entity = block->ElementAt(k);
            if (entity->IsPickUp(point.x(), point.y(), 0)) {
              DrawState drawState = entity->GetDrawState();
              if (drawState != Entities::Select &&
                  drawState != Entities::Hide) {
                m_curHoverEntity = entity;
                return m_curHoverEntity;
              }
            }
          }
        }
      }
    }
  }
  return m_curHoverEntity;
}

void GraphicsFrame::PaintEntities(std::vector<Block *> &blocks) {
  int width  = this->width();
  int height = this->height();
  m_Image    = QImage(QSize(width, height), QImage::Format_ARGB32);
  QPainter painter(&m_Image);
  size_t count = blocks.size();
  if (count > 0) {
    //设置反锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);
    //背景黑色,擦除图形
    painter.fillRect(0, 0, width, height, QBrush(Qt::black));
    //绘制图元
    PainterAttributes painterAttributes;
    QPen normalPen;
    normalPen.setColor(Color(255, 255, 0));
    normalPen.setWidth(1);
    painterAttributes.m_pen = normalPen;
    for (size_t i = 0; i < blocks.size(); ++i) {
      Block *block = blocks[i];
      if (!block->IsEmpty()) {
        size_t elemSize = block->GetElementSize();
        for (size_t k = 0; k < elemSize; ++k) {
          Entity *entity = block->ElementAt(k);
          entity->SetPainterAttributes(painterAttributes);
          entity->Draw(painter);
        }
      }
    }
  }
  if (m_curHoverEntity) {
    PainterAttributes painterHoverAttributes;
    QPen hoverPen;
    hoverPen.setColor(Color(255, 255, 0));
    hoverPen.setWidth(3);
    painterHoverAttributes.m_pen = hoverPen;
    m_curHoverEntity->SetPainterAttributes(painterHoverAttributes);
    m_curHoverEntity->Draw(painter);
  }
  this->update();
}
