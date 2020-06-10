#include "xline.h"

namespace Entities {
XLine::XLine() {
  m_xMax = 0.0;
  m_yMax = 0.0;
}

XLine::~XLine() {
}

XLine::XLine(const Point &basePoint, const Point &dirVector) {
  m_basePoint = basePoint;
  m_dirVector = dirVector;
}

XLine::XLine(const XLine &xLine) {
  m_basePoint      = xLine.GetBasePoint();
  m_dirVector      = xLine.GetDirVector();
  m_drawablePoint1 = xLine.GetDrawablePoint1();
  m_drawablePoint2 = xLine.GetDrawablePoint2();
  m_xMax           = xLine.GetXMax();
  m_yMax           = xLine.GetYMax();
  m_uuid           = xLine.GetId();
  m_boundingRect   = xLine.BoundingRect();
  SetAttributes(xLine.GetAttributes());
}
void XLine::Draw(QPainter &painter) {
  painter.setPen(m_painterAttributes.m_pen);
  PointF p1(m_drawablePoint1.GetDrawableX(), m_drawablePoint1.GetDrawableY());
  PointF p2(m_drawablePoint2.GetDrawableX(), m_drawablePoint2.GetDrawableY());
  painter.drawLine(p1, p2);
}

Entity *XLine::Clone() {
  Entity *xLine = new XLine(*this);
  return xLine;
}

void XLine::Transform(double *params, int size) {
  if (size > 4) {
    m_basePoint.Transform(params, size);
    m_yMax = params[1] + 1;
    m_xMax = params[4] + 1;
    ExportXLine();
    m_boundingRect = CalBoundingRect();
  }
}

void XLine::ExportXLine() {
  int dx = abs((int)m_dirVector.GetX());
  int dy = abs((int)m_dirVector.GetY());
  if (dx == 1 && dy == 0) {
    m_drawablePoint1.SetDrawableX(-EXTEND_LENGTH);
    m_drawablePoint1.SetDrawableY(m_basePoint.GetDrawableY());
    m_drawablePoint2.SetDrawableX(m_xMax + EXTEND_LENGTH);
    m_drawablePoint2.SetDrawableY(m_basePoint.GetDrawableY());
  } else if (dx == 0 && dy == 1) {
    m_drawablePoint1.SetDrawableX(m_basePoint.GetDrawableX());
    m_drawablePoint1.SetDrawableY(-EXTEND_LENGTH);
    m_drawablePoint2.SetDrawableX(m_basePoint.GetDrawableX());
    m_drawablePoint2.SetDrawableY(m_yMax + EXTEND_LENGTH);
  } else {
    double k     = -m_dirVector.GetY() / m_dirVector.GetX();
    double baseX = m_basePoint.GetDrawableX();
    double baseY = m_basePoint.GetDrawableY();
    double b     = baseY - k * baseX;
    // y=kx+b
    if (k < 0) {
      m_drawablePoint1.SetDrawableX(-EXTEND_LENGTH);
      m_drawablePoint1.SetDrawableY(b - k * EXTEND_LENGTH);
      m_drawablePoint2.SetDrawableX(-(b + EXTEND_LENGTH) / k);
      m_drawablePoint2.SetDrawableY(-EXTEND_LENGTH);
    } else {
      m_drawablePoint1.SetDrawableX(-EXTEND_LENGTH);
      m_drawablePoint1.SetDrawableY(b - k * EXTEND_LENGTH);
      m_drawablePoint2.SetDrawableX((m_yMax + EXTEND_LENGTH - b) / k);
      m_drawablePoint2.SetDrawableY(m_yMax + EXTEND_LENGTH);
    }
  }
}

void XLine::Scale(double ratio) {
  m_basePoint.Scale(ratio);
  ExportXLine();
  m_boundingRect = CalBoundingRect();
}

void XLine::Transfer(double dx, double dy, double dz) {
  m_basePoint.Transfer(dx, dy, dz);
  ExportXLine();
  m_boundingRect = CalBoundingRect();
}

void XLine::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(angle)
  Q_UNUSED(cx)
  Q_UNUSED(cy)
  Q_UNUSED(cz)
}

void XLine::CorrectCoord(double bx, double by, double bz, double sx, double sy,
    double sz, double rotaAngle) {
  m_basePoint.CorrectCoord(bx, by, bz, sx, sy, sz, rotaAngle);
}

bool XLine::IsPickUp(double x, double y, double z) {
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

RectF XLine::CalBoundingRect() {
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
