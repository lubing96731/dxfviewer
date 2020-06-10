#include "face3d.h"

namespace Entities {
Face3d::Face3d() {
  m_thickness = 0.0;
}

Face3d::Face3d(const Point &p1, const Point &p2, const Point &p3,
    const Point &p4, double thickNess) {
  m_pts[0]    = p1;
  m_pts[1]    = p2;
  m_pts[2]    = p3;
  m_pts[3]    = p4;
  m_thickness = thickNess;
}
Face3d::Face3d(const Point pts[4], double thickNess) {
  m_pts[0]    = pts[0];
  m_pts[1]    = pts[1];
  m_pts[2]    = pts[2];
  m_pts[3]    = pts[3];
  m_thickness = thickNess;
}
Face3d::Face3d(const Face3d &face3d) {
  const Point *pts = face3d.GetPts();
  m_pts[0]         = pts[0];
  m_pts[1]         = pts[1];
  m_pts[2]         = pts[2];
  m_pts[3]         = pts[3];
  m_thickness      = face3d.GetThickess();
  m_uuid           = face3d.GetId();
  SetAttributes(face3d.GetAttributes());
  m_boundingRect = face3d.BoundingRect();
}
Face3d::~Face3d() {
}

void Face3d::Draw(QPainter &painter) {
  Q_UNUSED(painter)
}

Entity *Face3d::Clone() {
  Entity *face3d = new Face3d(*this);
  return face3d;
}

void Face3d::Transform(double *params, int size) {
  Q_UNUSED(params)
  Q_UNUSED(size)
  m_boundingRect = CalBoundingRect();
}

void Face3d::Scale(double ratio) {
  Q_UNUSED(ratio)
  m_boundingRect = CalBoundingRect();
}

void Face3d::Transfer(double dx, double dy, double dz) {
  Q_UNUSED(dx)
  Q_UNUSED(dy)
  Q_UNUSED(dz)
  m_boundingRect = CalBoundingRect();
}

void Face3d::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(angle)
  Q_UNUSED(cx)
  Q_UNUSED(cy)
  Q_UNUSED(cz)
}

void Face3d::CorrectCoord(double bx, double by, double bz, double sx, double sy,
    double sz, double rotaAngle) {
  for (int i = 0; i < 4; ++i) {
    m_pts[i].CorrectCoord(bx, by, bz, sx, sy, sz, rotaAngle);
  }
}

bool Face3d::IsPickUp(double x, double y, double z) {
  Q_UNUSED(x)
  Q_UNUSED(y)
  Q_UNUSED(z)
  return false;
}

RectF Face3d::CalBoundingRect() {
  return RectF(0, 0, 0, 0);
}

} // namespace Entities
