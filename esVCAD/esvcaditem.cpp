#include "esvcaditem.h"

#include <qcursor.h>

EsvCadItem::EsvCadItem(int width, int height, QGraphicsItem *parent) :
    QGraphicsItem(parent) {
  m_width          = width;
  m_height         = height;
  m_curHoverEntity = nullptr;
  m_bHover         = false;
  m_esvCad.SetBoundingRectExSize(0.25);
  setFlag(QGraphicsItem::ItemIsFocusable);
  setFlag(QGraphicsItem::ItemIsMovable);
  setAcceptHoverEvents(true);
  this->setCursor(Qt::CrossCursor);
}

EsvCadItem::~EsvCadItem() {
}

Entity *EsvCadItem::GetPickedEntity(const QPointF &point) {
  Entity *pickedEntity        = nullptr;
  std::vector<Block *> blocks = m_esvCad.GetBlocks();
  size_t count                = blocks.size();
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
                pickedEntity = entity;
                return pickedEntity;
              }
            }
          }
        }
      }
    }
  }
  return pickedEntity;
}
void EsvCadItem::ParseDxf(const QString &fileName) {
  m_esvCad.ParseDxf(fileName.toStdString(), m_width, m_height);
}

void EsvCadItem::HideSelEntities() {
  for (size_t i = 0; i < m_selEntities.size(); ++i) {
    Entity *selEntity = m_selEntities[i];
    selEntity->SetDrawState(Entities::Hide);
  }
  m_selEntities.clear();
  update();
}

void EsvCadItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsItem::mousePressEvent(event);
}

void EsvCadItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsItem::mouseMoveEvent(event);
}
void EsvCadItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    if (m_curHoverEntity) {
      m_selEntities.push_back(m_curHoverEntity);
      m_curHoverEntity->SetDrawState(Entities::Select);
      update();
    }
    // Entity *selEntity = GetPickedEntity(event->pos());
    // if (selEntity) {
    //	m_selEntities.push_back(selEntity);
    //	selEntity->SetDrawState(Entities::Select);
    //	update();
    //}
  }
  QGraphicsItem::mouseReleaseEvent(event);
}
void EsvCadItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  m_curHoverEntity = GetPickedEntity(event->pos());
  update();
  QGraphicsItem::hoverMoveEvent(event);
}

void EsvCadItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
  QGraphicsItem::dragMoveEvent(event);
}

void EsvCadItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  HideSelEntities();
}

QRectF EsvCadItem::boundingRect() const {
  return QRectF(0, 0, m_width, m_height);
}

void EsvCadItem::paint(QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget) {
  //设置反锯齿
  painter->setRenderHint(QPainter::Antialiasing, true);
  double scaleFactor = 1.0 / painter->matrix().m11();
  PainterAttributes painterAttributes;
  QPen normalPen;
  normalPen.setCapStyle(Qt::FlatCap);
  normalPen.setColor(Color(255, 255, 0));
  normalPen.setWidthF(1 * scaleFactor);
  painterAttributes.m_pen = normalPen;

  PainterAttributes painterHoverAttributes;
  QPen hoverPen;
  hoverPen.setCapStyle(Qt::FlatCap);
  hoverPen.setColor(Color(255, 255, 255, 128)); // QColor(255,0,255,150)
  hoverPen.setWidthF(4.25 * scaleFactor);
  painterHoverAttributes.m_pen = hoverPen;

  double hitRange             = 3 * scaleFactor;
  std::vector<Block *> blocks = m_esvCad.GetBlocks();
  QPen pen(Qt::green, 1.5 * scaleFactor, Qt::DashDotLine, Qt::FlatCap);
  size_t count = blocks.size();
  if (count > 0) {
    for (size_t i = 0; i < count; ++i) {
      Block *block = blocks[i];
      if (!block->IsEmpty()) {
        block->SetHitRange(hitRange);
        size_t elemSize = block->GetElementSize();
        for (size_t k = 0; k < elemSize; ++k) {
          Entity *entity      = block->ElementAt(k);
          DrawState drawState = entity->GetDrawState();
          if (drawState != Entities::Select && drawState != Entities::Hide) {
            entity->SetHitRange(hitRange);
            entity->SetPainterAttributes(painterAttributes);
            entity->Draw(*painter);
          }
        }
      }
    }
  }

  if (m_curHoverEntity) {
    m_curHoverEntity->SetPainterAttributes(painterHoverAttributes);
    m_curHoverEntity->Draw(*painter);
  }
  QRectF drawRgn(0, 0, m_width, m_height);
  for (size_t i = 0; i < m_selEntities.size(); ++i) {
    Entity *selEntity = m_selEntities[i];

    painter->setPen(pen);
    const RectF &rect = selEntity->BoundingRect();
    painter->drawRect(rect);

    painterAttributes.m_pen.setColor(QColor(255, 0, 255, 150));
    painterAttributes.m_pen.setWidthF(1.5 * scaleFactor);
    selEntity->SetPainterAttributes(painterAttributes);
    selEntity->Draw(*painter);
  }
}

QPainterPath EsvCadItem::shape() const {
  return QGraphicsItem::shape();
}
