#include "ray.h"

namespace Entities {
Ray::Ray() {
}

Ray::Ray(const Point &basePoint, const Point &dirVector) :
    XLine(basePoint, dirVector) {
}

Ray::Ray(const Ray &ray) {
  m_basePoint    = ray.GetBasePoint();
  m_dirVector    = ray.GetDirVector();
  m_uuid         = ray.GetId();
  m_boundingRect = ray.BoundingRect();
  SetAttributes(ray.GetAttributes());
}

Ray::~Ray() {
}

void Ray::Draw(QPainter &painter) {
  painter.setPen(m_painterAttributes.m_pen);
  PointF p1(m_drawablePoint1.GetDrawableX(), m_drawablePoint1.GetDrawableY());
  PointF p2(m_drawablePoint2.GetDrawableX(), m_drawablePoint2.GetDrawableY());
  painter.drawLine(p1, p2);
}

Entity *Ray::Clone() {
  Entity *ray = new Ray(*this);
  return ray;
}
void Ray::Transform(double *params, int size) {
  if (size > 4) {
    m_basePoint.Transform(params, size);
    m_yMax = params[1] + 1;
    m_xMax = params[4] + 1;
    ExportRay();
    m_boundingRect = CalBoundingRect();
  }
}
void Ray::Scale(double ratio) {
  m_basePoint.Scale(ratio);
  ExportRay();
  m_boundingRect = CalBoundingRect();
}
void Ray::Transfer(double dx, double dy, double dz) {
  m_basePoint.Transfer(dx, dy, dz);
  ExportRay();
  m_boundingRect = CalBoundingRect();
}
void Ray::Rotate(double angle, double cx, double cy, double cz) {
  m_basePoint.Rotate(angle, cx, cy, cz);
  ExportRay();
  m_boundingRect = CalBoundingRect();
}

void Ray::ExportRay() {
  int dx = (int)m_dirVector.GetX();
  int dy = (int)m_dirVector.GetY();
  m_drawablePoint1.SetDrawableX(m_basePoint.GetDrawableX());
  m_drawablePoint1.SetDrawableY(m_basePoint.GetDrawableY());
  if (dx == 1 && dy == 0) {
    m_drawablePoint2.SetDrawableX(m_xMax + EXTEND_LENGTH);
    m_drawablePoint2.SetDrawableY(m_basePoint.GetDrawableY());
  } else if (dx == -1 && dy == 0) {
    m_drawablePoint2.SetDrawableX(-EXTEND_LENGTH);
    m_drawablePoint2.SetDrawableY(m_basePoint.GetDrawableY());
  } else if (dx == 0 && dy == 1) {
    m_drawablePoint2.SetDrawableX(m_basePoint.GetDrawableX());
    m_drawablePoint2.SetDrawableY(-EXTEND_LENGTH);
  } else if (dx == 0 && dy == -1) {
    m_drawablePoint2.SetDrawableX(m_basePoint.GetDrawableX());
    m_drawablePoint2.SetDrawableY(m_yMax + EXTEND_LENGTH);
  } else {
    double k     = -m_dirVector.GetY() / m_dirVector.GetX();
    double baseX = m_basePoint.GetDrawableX();
    double baseY = m_basePoint.GetDrawableY();
    double b     = baseY - k * baseX;
    // y=kx+b
    if (k < 0) {
      if (m_dirVector.GetY() < 0) {
        m_drawablePoint2.SetDrawableX(-EXTEND_LENGTH);
        m_drawablePoint2.SetDrawableY(b - k * EXTEND_LENGTH);
      } else {
        m_drawablePoint2.SetDrawableX(-(b + EXTEND_LENGTH) / k);
        m_drawablePoint2.SetDrawableY(-EXTEND_LENGTH);
      }

    } else {
      if (m_dirVector.GetY() < 0) {
        m_drawablePoint2.SetDrawableX((m_yMax + EXTEND_LENGTH - b) / k);
        m_drawablePoint2.SetDrawableY(m_yMax + EXTEND_LENGTH);
      } else {
        m_drawablePoint2.SetDrawableX(-EXTEND_LENGTH);
        m_drawablePoint2.SetDrawableY(b - k * EXTEND_LENGTH);
      }
    }
  }
}

void Ray::CorrectCoord(double bx, double by, double bz, double sx, double sy,
    double sz, double rotaAngle) {
  m_basePoint.CorrectCoord(bx, by, bz, sx, sy, sz, rotaAngle);
}

bool Ray::IsPickUp(double x, double y, double z) {
  Q_UNUSED(z)
  RectF rect = m_boundingRect;
  rect.adjust(-m_hitRange, -m_hitRange, m_hitRange, m_hitRange);
  if (rect.contains(x, y)) {
    double A =
        (m_drawablePoint2.GetDrawableY() - m_drawablePoint1.GetDrawableY());
    double B =
        (m_drawablePoint1.GetDrawableX() - m_drawablePoint2.GetDrawableX());
    double C =
        (m_drawablePoint2.GetDrawableX() * m_drawablePoint1.GetDrawableY()) -
        (m_drawablePoint1.GetDrawableX() * m_drawablePoint2.GetDrawableY());
    double distance = fabs((A * x + B * y + C) / sqrt(A * A + B * B));
    if (distance <= m_hitRange) { return true; }
  }
  return false;
}

RectF Ray::CalBoundingRect() {
  double minX = std::min(
      m_drawablePoint1.GetDrawableX(), m_drawablePoint2.GetDrawableX());
  double maxX = std::max(
      m_drawablePoint1.GetDrawableX(), m_drawablePoint2.GetDrawableX());
  double minY = std::min(
      m_drawablePoint1.GetDrawableY(), m_drawablePoint2.GetDrawableY());
  double maxY = std::max(
      m_drawablePoint1.GetDrawableY(), m_drawablePoint2.GetDrawableY());
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
  rect = rect.intersected(RectF(0, 0, m_xMax, m_yMax));
  return rect;
}

} // namespace Entities
