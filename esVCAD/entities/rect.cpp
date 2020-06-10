#include "rect.h"

namespace Entities {
Rect::Rect() {
}

Rect::Rect(const Point &leftTop, double width, double height) {
  m_leftTop     = leftTop;
  m_rightBottom = Point(leftTop.GetX() + width, leftTop.GetY() + height, 0.0);
}
Rect::Rect(const Point &leftTop, const Point &rightBottom) {
  m_leftTop     = leftTop;
  m_rightBottom = rightBottom;
}

Rect::Rect(const Rect &rect) {
  m_leftTop      = rect.GetLeftTop();
  m_rightBottom  = rect.GetRightBottom();
  m_uuid         = rect.GetId();
  m_boundingRect = rect.BoundingRect();
  SetAttributes(rect.GetAttributes());
}

Rect::~Rect() {
}

void Rect::Draw(QPainter &painter) {
  Q_UNUSED(painter)
}

Entity *Rect::Clone() {
  Entity *rect = new Rect(*this);
  return rect;
}

void Rect::Transform(double *params, int size) {
  Q_UNUSED(params)
  Q_UNUSED(size)
  m_boundingRect = CalBoundingRect();
}
void Rect::Scale(double ratio) {
  Q_UNUSED(ratio)
  m_boundingRect = CalBoundingRect();
}

void Rect::Transfer(double dx, double dy, double dz) {
  Q_UNUSED(dx)
  Q_UNUSED(dy)
  Q_UNUSED(dz)
  m_boundingRect = CalBoundingRect();
}

void Rect::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(angle)
  Q_UNUSED(cx)
  Q_UNUSED(cy)
  Q_UNUSED(cz)
}
bool Rect::IsPtInRect(const Point &pt) {
  double x = pt.GetX();
  double y = pt.GetY();
  if (x >= m_leftTop.GetX() && x <= m_rightBottom.GetX() &&
      y >= m_leftTop.GetY() && y <= m_rightBottom.GetY()) {
    return true;
  }
  return false;
}

void Rect::CorrectCoord(double bx, double by, double bz, double sx, double sy,
    double sz, double rotaAngle) {
  Q_UNUSED(rotaAngle)
  Q_UNUSED(bx)
  Q_UNUSED(by)
  Q_UNUSED(bz)
  Q_UNUSED(sx)
  Q_UNUSED(sy)
  Q_UNUSED(sz)
}

bool Rect::IsPickUp(double x, double y, double z) {
  RectF rect = m_boundingRect;
  rect.adjust(-m_hitRange, -m_hitRange, m_hitRange, m_hitRange);
  if (rect.contains(x, y)) { return true; }
  return false;
}

RectF Rect::CalBoundingRect() {
  return RectF(m_leftTop.GetDrawableX(), m_leftTop.GetDrawableY(),
      m_rightBottom.GetDrawableX(), m_rightBottom.GetDrawableY());
}

} // namespace Entities
