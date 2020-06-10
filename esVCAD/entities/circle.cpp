#include "circle.h"
namespace Entities {
Circle::Circle() :
    m_center(0.0, 0.0, 0.0), m_radius(0.0), m_drawableRadius(0.0) {
}

Circle::Circle(const Point &center, double radius) {
  m_center         = center;
  m_radius         = radius;
  m_drawableRadius = 0.0;
}

Circle::Circle(const Circle &circle) {
  m_center         = circle.m_center;
  m_radius         = circle.m_radius;
  m_drawableRadius = circle.GetDrawableRadius();
  m_uuid           = circle.GetId();
  m_boundingRect   = circle.BoundingRect();
  SetAttributes(circle.GetAttributes());
}

Circle::~Circle() {
}

void Circle::Draw(QPainter &painter) {
  painter.setPen(m_painterAttributes.m_pen);
  double centerX = m_center.GetDrawableX();
  double centerY = m_center.GetDrawableY();
  PointF point(centerX, centerY);
  double radius = m_drawableRadius;
  painter.drawEllipse(point, radius, radius);
}

Entity *Circle::Clone() {
  Entity *circle = new Circle(*this);
  return circle;
}

void Circle::Transform(double *params, int size) {
  if (size > 0) {
    double scale = params[0];
    m_center.Transform(params, size);
    m_drawableRadius = m_radius / scale;
    m_boundingRect   = CalBoundingRect();
  }
}
void Circle::Scale(double ratio) {
  m_center.Scale(ratio);
  m_drawableRadius *= ratio;
  m_boundingRect = CalBoundingRect();
}

void Circle::Transfer(double dx, double dy, double dz) {
  m_center.Transfer(dx, dy, dz);
  m_boundingRect = CalBoundingRect();
}

void Circle::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(angle)
  Q_UNUSED(cx)
  Q_UNUSED(cy)
  Q_UNUSED(cz)
}

void Circle::CorrectCoord(double bx, double by, double bz, double sx, double sy,
    double sz, double rotaAngle) {
  m_center.CorrectCoord(bx, by, bz, sx, sy, sz, rotaAngle);
  m_radius *= sx;
}

bool Circle::IsPickUp(double x, double y, double z) {
  Q_UNUSED(z)
  RectF rect = m_boundingRect;
  rect.adjust(-m_hitRange, -m_hitRange, m_hitRange, m_hitRange);
  if (rect.contains(x, y)) {
    double r0 = m_drawableRadius - m_hitRange;
    double r1 = m_drawableRadius + m_hitRange;
    double dx = x - m_center.GetDrawableX();
    double dy = y - m_center.GetDrawableY();
    double d2 = dx * dx + dy * dy;
    if (d2 >= r0 * r0 && d2 <= r1 * r1) { return true; }
  }
  return false;
}

RectF Circle::CalBoundingRect() {
  RectF rect(m_center.GetDrawableX() - m_drawableRadius,
      m_center.GetDrawableY() - m_drawableRadius, 2 * m_drawableRadius,
      2 * m_drawableRadius);
  return rect;
}

} // namespace Entities
