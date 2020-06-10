#include "coordxy.h"
#define RECT_SIZE 0
namespace Entities {
CoordXY::CoordXY() : Rect() {
  m_maxX = 0.0;
  m_maxY = 0.0;
}

CoordXY::~CoordXY() {
}

CoordXY::CoordXY(const Point &leftTop, double width, double height) :
    Rect(leftTop, width, height) {
  m_maxX = 0.0;
  m_maxY = 0.0;
}

CoordXY::CoordXY(const Point &leftTop, const Point &rightBottom) :
    Rect(leftTop, rightBottom) {
}

CoordXY::CoordXY(const CoordXY &coordXY) {
  m_leftTop      = coordXY.GetLeftTop();
  m_rightBottom  = coordXY.GetRightBottom();
  m_uuid         = coordXY.GetId();
  m_boundingRect = coordXY.BoundingRect();
  SetAttributes(coordXY.GetAttributes());
}

void CoordXY::Draw(QPainter &painter) {
  double centerX = m_origin.GetDrawableX();
  double centerY = m_origin.GetDrawableY();
  if (centerX < RECT_SIZE) centerX = RECT_SIZE;
  if (centerY < RECT_SIZE) centerY = RECT_SIZE;
  if (centerX >= m_maxX - RECT_SIZE) centerX = m_maxX - RECT_SIZE;
  if (centerY >= m_maxY - RECT_SIZE) centerY = m_maxY - RECT_SIZE;

  QPen pen;
  pen.setColor(Qt::white);
  pen.setWidthF(3);
  pen.setStyle(Qt::SolidLine);
  painter.setPen(pen);

  pen.setColor(Qt::green);
  painter.setPen(pen);
  painter.drawLine(PointF(centerX, centerY), PointF(centerX, 0));
  pen.setColor(Qt::red);
  painter.setPen(pen);
  painter.drawLine(PointF(centerX, centerY), PointF(m_maxX, centerY));

  pen.setColor(Qt::blue);
  painter.setPen(pen);
  RectF rect(
      centerX - RECT_SIZE, centerY - RECT_SIZE, RECT_SIZE << 1, RECT_SIZE << 1);
  painter.drawRect(rect);

  pen.setColor(Qt::white);
  painter.setPen(pen);
  // draw x coord
  painter.drawLine(PointF(centerX, centerY), PointF(centerX + 100, centerY));
  painter.drawText(PointF(centerX + 50, centerY + 25), "X");
  // draw y coord
  painter.drawLine(PointF(centerX, centerY), PointF(centerX, centerY - 100));
  painter.drawText(PointF(centerX - 25, centerY - 50), "Y");

  pen.setColor(Qt::blue);
  painter.setPen(pen);
  RectF rgn(0, 0, m_maxX, m_maxY);
  painter.drawRect(rgn);
}

Entity *CoordXY::Clone() {
  Entity *coordXY = new CoordXY(*this);
  return coordXY;
}

void CoordXY::Transform(double *params, int size) {
  //计算原点坐标
  if (size > 4) {
    m_maxY = params[1];
    m_maxX = params[4];
    m_leftTop.Transform(params, size);
    m_rightBottom.Transform(params, size);
    m_origin.Transform(params, size);
    m_boundingRect = CalBoundingRect();
  }
}

void CoordXY::Scale(double ratio) {
  m_origin.Scale(ratio);
  // m_leftTop.Scale(ratio);
  // m_rightBottom.Scale(ratio);
  m_boundingRect = CalBoundingRect();
}

void CoordXY::Transfer(double dx, double dy, double dz) {
  m_origin.Transfer(dx, dy, dz);
  // m_leftTop.Transfer(dx,dy,dz);
  // m_rightBottom.Transfer(dx,dy,dz);
  m_boundingRect = CalBoundingRect();
}

void CoordXY::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(angle)
  Q_UNUSED(cx)
  Q_UNUSED(cy)
  Q_UNUSED(cz)
}

void CoordXY::CorrectCoord(double bx, double by, double bz, double sx,
    double sy, double sz, double rotaAngle) {
  Q_UNUSED(bx)
  Q_UNUSED(by)
  Q_UNUSED(bz)
  Q_UNUSED(sx)
  Q_UNUSED(sy)
  Q_UNUSED(sz)
  Q_UNUSED(rotaAngle)
}

bool CoordXY::IsPickUp(double x, double y, double z) {
  // RectF rect = BoundingRect();
  // rect.adjust(-HIT_RANGE, -HIT_RANGE, HIT_RANGE, HIT_RANGE);
  // if (rect.contains(x, y)) {
  //	return true;
  //}
  Q_UNUSED(x)
  Q_UNUSED(y)
  Q_UNUSED(z)
  return false;
}

RectF CoordXY::CalBoundingRect() {
  return RectF(0, 0, 0, 0); // RectF(0, 0, m_maxX, m_maxY);
}

} // namespace Entities
