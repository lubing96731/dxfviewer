#include "arc.h"

#include "utility.h"

namespace Entities {
Arc::Arc(
    const Point &center, double radius, double startAngle, double endAngle) {
  m_center         = center;
  m_radius         = radius;
  m_startAngle     = startAngle;
  m_endAngle       = endAngle;
  m_drawableRadius = 0.0;
}

Arc::Arc(const Arc &arc) {
  m_center         = arc.GetCenter();
  m_radius         = arc.GetRadius();
  m_drawableRadius = arc.GetDrawableRadius();
  m_startAngle     = arc.GetStartAngle();
  m_endAngle       = arc.GetEndAngle();
  m_uuid           = arc.GetId();
  m_boundingRect   = arc.BoundingRect();
  SetAttributes(arc.GetAttributes());
}

Arc::~Arc() {
}

Arc::Arc() {
}

void Arc::Draw(QPainter &painter) {
  painter.setPen(m_painterAttributes.m_pen);
  double d       = 2 * m_drawableRadius;
  double centerX = m_center.GetDrawableX();
  double centerY = m_center.GetDrawableY();
  RectF rect(centerX - m_drawableRadius, centerY - m_drawableRadius, d, d);
  double spanAngle = m_endAngle - m_startAngle;
  if (m_startAngle > m_endAngle) { spanAngle += 360; }
  double startAngle = m_startAngle * 16;
  spanAngle *= 16;
  painter.drawArc(rect, (int)startAngle, (int)spanAngle);
}

Entity *Arc::Clone() {
  Entity *arc = new Arc(*this);
  return arc;
}

void Arc::Transform(double *params, int size) {
  if (size > 0) {
    double scale = params[0];
    m_center.Transform(params, size);
    m_drawableRadius = m_radius / scale;
    m_boundingRect   = CalBoundingRect();
  }
}

void Arc::Scale(double ratio) {
  m_center.Scale(ratio);
  m_drawableRadius *= ratio;
  m_boundingRect = CalBoundingRect();
}

void Arc::Transfer(double dx, double dy, double dz) {
  m_center.Transfer(dx, dy, dz);
  m_boundingRect = CalBoundingRect();
}

void Arc::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(angle)
  Q_UNUSED(cx)
  Q_UNUSED(cy)
  Q_UNUSED(cz)
}

void Arc::CorrectCoord(double bx, double by, double bz, double sx, double sy,
    double sz, double rotaAngle) {
  m_center.CorrectCoord(bx, by, bz, sx, sy, sz, rotaAngle);
  m_radius *= sx;
  double angle = rotaAngle / PI * 180;
  m_startAngle += angle;
  m_endAngle += angle;
}

bool Arc::IsPickUp(double x, double y, double z) {
  Q_UNUSED(z)
  RectF rect = m_boundingRect;
  rect.adjust(-m_hitRange, -m_hitRange, m_hitRange, m_hitRange);
  if (rect.contains(x, y)) {
    Point pt;
    pt.SetDrawableX(x);
    pt.SetDrawableY(y);
    double angle      = Utility::CalAngle(m_center, pt) / PI * 180;
    double dis        = Utility::Distance(pt, m_center);
    double startAngle = (double)(fmodf((float)m_startAngle, 360));
    double endAngle   = (double)(fmodf((float)m_endAngle, 360));
    if (startAngle < endAngle) {
      if (angle >= startAngle && angle <= endAngle) {
        if (fabs(dis - m_drawableRadius) <= m_hitRange) { return true; }
      }
    } else {
      if (angle >= startAngle || angle <= endAngle) {
        if (fabs(dis - m_drawableRadius) <= m_hitRange) { return true; }
      }
    }
  }
  return false;
}

RectF Arc::CalBoundingRect() {
  double startAngle = m_startAngle;
  double endAngle   = m_endAngle;
  if (startAngle > endAngle) { endAngle += 360; }
  std::vector<double> vx;
  std::vector<double> vy;
  double angles[]   = {startAngle, endAngle, 0.0, 90.0, 180.0, 270.0, 360.0,
      450.0, 540.0, 630.0, 720.0};
  static double eps = 1e-6;
  for (int i = 0; i < (int)(sizeof(angles) / sizeof(double)); ++i) {
    double angle = angles[i];
    if (angle >= startAngle - eps && angle <= endAngle + eps) {
      double arcAngle = angle * PI / 180;
      double x = m_drawableRadius * cos(arcAngle) + m_center.GetDrawableX();
      double y = m_center.GetDrawableY() - m_drawableRadius * sin(arcAngle);
      vx.push_back(x);
      vy.push_back(y);
    }
  }
  double minX   = *std::min_element(vx.begin(), vx.end());
  double maxX   = *std::max_element(vx.begin(), vx.end());
  double minY   = *std::min_element(vy.begin(), vy.end());
  double maxY   = *std::max_element(vy.begin(), vy.end());
  double width  = maxX - minX;
  double height = maxY - minY;
  RectF rect(minX, minY, width, height);
  if (width < Entity::m_BoundingRectExSize) {
    rect.adjust(
        -Entity::m_BoundingRectExSize, 0, Entity::m_BoundingRectExSize, 0);
  }
  if (height < m_BoundingRectExSize) {
    rect.adjust(
        0, -Entity::m_BoundingRectExSize, 0, Entity::m_BoundingRectExSize);
  }
  return rect;
}
} // namespace Entities
