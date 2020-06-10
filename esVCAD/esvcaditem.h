#ifndef ESVCADITEM_H
#define ESVCADITEM_H

#include "esVCAD.h"

#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#define CAD 3000
class EsvCadItem : public QGraphicsItem {
 public:
  explicit EsvCadItem(int width, int height, QGraphicsItem *parent = Q_NULLPTR);
  ~EsvCadItem();
  enum { Type = UserType + CAD };
  int type() const { return Type; }

 protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
  virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
  virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
      QWidget *widget);
  virtual QPainterPath shape() const;

 private:
  EsvCad m_esvCad;
  int m_width;
  int m_height;
  Entity *m_curHoverEntity;
  std::vector<Entity *> m_selEntities;
  bool m_bHover;

 private:
  Entity *GetPickedEntity(const QPointF &point);

 public:
  void ParseDxf(const QString &fileName);
  void HideSelEntities();
  std::vector<Block *> &GetBlocks() { return m_esvCad.GetBlocks(); }
};

#endif // ESVCADITEM_H
