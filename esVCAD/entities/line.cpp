#include "line.h"

#include <QBrush>
#include <QPen>

namespace Entities {
Line::Line() {
}

Line::Line(const Point &p1, const Point &p2) {
  m_startPoint = p1;
  m_endPoint   = p2;
}

Line::Line(const Line &line) {
  m_startPoint   = line.GetStartPoint();
  m_endPoint     = line.GetEndPoint();
  m_uuid         = line.GetId();
  m_boundingRect = line.BoundingRect();
  SetAttributes(line.GetAttributes());
}

Line::~Line() {
}

void Line::Draw(QPainter &painter) {
  painter.setPen(m_painterAttributes.m_pen);
  PointF p1(m_startPoint.GetDrawableX(), m_startPoint.GetDrawableY());
  PointF p2(m_endPoint.GetDrawableX(), m_endPoint.GetDrawableY());
  double dx = fabs(p1.x() - p2.x());
  double dy = fabs(p1.y() - p2.y());
  if (dx <= 1E-9 && dy <= 1E-9) {
    painter.drawPoint(p1);
  } else {
    painter.drawLine(p1, p2);
  }
}

Entity *Line::Clone() {
  Entity *line = new Line(*this);
  return line;
}

void Line::Transform(double *params, int size) {
  m_startPoint.Transform(params, size);
  m_endPoint.Transform(params, size);
  m_boundingRect = CalBoundingRect();
}

void Line::Scale(double ratio) {
  m_startPoint.Scale(ratio);
  m_endPoint.Scale(ratio);
  m_boundingRect = CalBoundingRect();
}

void Line::Transfer(double dx, double dy, double dz) {
  m_startPoint.Transfer(dx, dy, dz);
  m_endPoint.Transfer(dx, dy, dz);
  m_boundingRect = CalBoundingRect();
}

void Line::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(angle)
  Q_UNUSED(cx)
  Q_UNUSED(cy)
  Q_UNUSED(cz)
}

void Line::CorrectCoord(double bx, double by, double bz, double sx, double sy,
    double sz, double rotaAngle) {
  m_startPoint.CorrectCoord(bx, by, bz, sx, sy, sz, rotaAngle);
  m_endPoint.CorrectCoord(bx, by, bz, sx, sy, sz, rotaAngle);
}

bool Line::IsPickUp(double x, double y, double z) {
  Q_UNUSED(z)
  RectF rect = m_boundingRect;
  rect.adjust(-m_hitRange, -m_hitRange, m_hitRange, m_hitRange);
  if (rect.contains(x, y)) {
    double A = (m_endPoint.GetDrawableY() - m_startPoint.GetDrawableY());
    double B = (m_startPoint.GetDrawableX() - m_endPoint.GetDrawableX());
    double C = (m_endPoint.GetDrawableX() * m_startPoint.GetDrawableY()) -
        (m_startPoint.GetDrawableX() * m_endPoint.GetDrawableY());
    static double eps = 1E-12;
    if (fabs(A) < eps && fabs(B) < eps && fabs(C) < eps) {
      return true;
    } else {
      double distance = fabs((A * x + B * y + C) / sqrt(A * A + B * B));
      if (distance <= m_hitRange) { return true; }
    }
  }

  return false;
}

RectF Line::CalBoundingRect() {
  double minX =
      std::min(m_startPoint.GetDrawableX(), m_endPoint.GetDrawableX());
  double maxX =
      std::max(m_startPoint.GetDrawableX(), m_endPoint.GetDrawableX());
  double minY =
      std::min(m_startPoint.GetDrawableY(), m_endPoint.GetDrawableY());
  double maxY =
      std::max(m_startPoint.GetDrawableY(), m_endPoint.GetDrawableY());
  double width  = maxX - minX;
  double height = maxY - minY;
  RectF rect(minX, minY, width, height);
  if (width < Entity::m_BoundingRectExSize) {
    rect.adjust(
        -Entity::m_BoundingRectExSize, 0, Entity::m_BoundingRectExSize, 0);
  }
  if (height < Entity::m_BoundingRectExSize) {
    rect.adjust(
        0, -Entity::m_BoundingRectExSize, 0, Entity::m_BoundingRectExSize);
  }
  return rect;
}

} // namespace Entities
