#include "point.h"

namespace Entities {
Point::Point() :
    m_x(0.0), m_y(0.0), m_z(0.0), m_bulge(0.0), m_drawableX(0.0),
    m_drawableY(0.0), m_drawableZ(0.0) {
}

Point::Point(double x, double y, double z) {
  m_x         = x;
  m_y         = y;
  m_z         = z;
  m_bulge     = 0.0;
  m_drawableX = 0.0;
  m_drawableY = 0.0;
  m_drawableZ = 0.0;
}

Point::Point(double x, double y, double z, double bulge) {
  m_x         = x;
  m_y         = y;
  m_z         = z;
  m_bulge     = bulge;
  m_drawableX = 0.0;
  m_drawableY = 0.0;
  m_drawableZ = 0.0;
}

Point::Point(const Point &pt) {
  m_x            = pt.GetX();
  m_y            = pt.GetY();
  m_z            = pt.GetZ();
  m_bulge        = pt.GetBulge();
  m_drawableX    = pt.GetDrawableX();
  m_drawableY    = pt.GetDrawableY();
  m_drawableZ    = pt.GetDrawableZ();
  m_uuid         = pt.GetId();
  m_boundingRect = pt.BoundingRect();
  SetAttributes(pt.GetAttributes());
}

Point::~Point() {
}
Point &Point::operator=(const Point &pt) {
  if (&pt != this) {
    m_x         = pt.GetX();
    m_y         = pt.GetY();
    m_z         = pt.GetZ();
    m_bulge     = pt.GetBulge();
    m_drawableX = pt.GetDrawableX();
    m_drawableY = pt.GetDrawableY();
    m_drawableZ = pt.GetDrawableZ();
    SetAttributes(pt.GetAttributes());
  }
  return *this;
}

bool Point::operator==(const Point &pt) {
  static double EPS = 1E-12;
  if (fabs(m_x - pt.GetX()) < EPS && fabs(m_y - pt.GetY()) < EPS &&
      fabs(m_z - pt.GetZ()) < EPS && fabs(m_bulge - pt.GetBulge()) < EPS &&
      fabs(m_drawableX - pt.GetDrawableX()) < EPS &&
      fabs(m_drawableY - pt.GetDrawableY()) < EPS &&
      fabs(m_drawableZ - pt.GetDrawableZ()) < EPS) {
    return true;
  }
  return false;
}
void Point::Draw(QPainter &painter) {
  painter.setPen(m_painterAttributes.m_pen);
  painter.drawPoint(PointF(m_drawableX, m_drawableY));
}

Entity *Point::Clone() {
  Entity *pt = new Point(*this);
  return pt;
}

void Point::Transform(double *params, int size) {
  if (size >= 4) {
    double scale   = params[0];
    double yMax    = params[1];
    double dx      = params[2];
    double dy      = params[3];
    m_drawableX    = (m_x - dx) / scale;
    m_drawableY    = yMax - ((m_y - dy) / scale);
    m_drawableZ    = (m_z - dx) / scale;
    m_boundingRect = CalBoundingRect();
  }
}

void Point::Scale(double ratio) {
  m_drawableX *= ratio;
  m_drawableY *= ratio;
  m_drawableZ *= ratio;
  m_boundingRect = CalBoundingRect();
}

void Point::Transfer(double dx, double dy, double dz) {
  m_drawableX += dx;
  m_drawableY += dy;
  m_drawableZ += dz;
  m_boundingRect = CalBoundingRect();
}

void Point::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(cz)
  // rota at org
  double cosVal = cos(angle);
  double sinVal = sin(angle);
  double newX = (m_drawableX - cx) * cosVal - (m_drawableY - cy) * sinVal + cx;
  double newY = (m_drawableY - cy) * cosVal + (m_drawableX - cx) * sinVal + cy;
  m_drawableX = newX;
  m_drawableY = newY;
  m_boundingRect = CalBoundingRect();
}

void Point::CorrectCoord(double bx, double by, double bz, double sx, double sy,
    double sz, double rotaAngle) {
  double sinValue = sin(rotaAngle);
  double cosValue = cos(rotaAngle);
  m_x *= sx;
  m_y *= sy;
  m_z *= sz;
  double newX0 = m_x * cosValue - m_y * sinValue;
  double newY0 = m_x * sinValue + m_y * cosValue;
  m_x          = newX0 + bx;
  m_y          = newY0 + by;
  m_z += bz;
}

bool Point::IsPickUp(double x, double y, double z) {
  Q_UNUSED(z)
  RectF rect = m_boundingRect;
  rect.adjust(-m_hitRange, -m_hitRange, m_hitRange, m_hitRange);
  if (rect.contains(x, y)) { return true; }
  return false;
}

RectF Point::CalBoundingRect() {
  return RectF(m_drawableX - Entity::m_BoundingRectExSize,
      m_drawableY - Entity::m_BoundingRectExSize,
      2 * Entity::m_BoundingRectExSize, 2 * Entity::m_BoundingRectExSize);
}

} // namespace Entities
