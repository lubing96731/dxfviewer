#include "polyline.h"

#include "utility.h"
namespace Entities {
Polyline::Polyline() : m_flag(0), m_number(0), m_m(0), m_n(0), m_elevation(0) {
}

Polyline::Polyline(std::vector<Point *> &vertexes, uint m, uint n, int flag,
    double elevation) {
  for (size_t i = 0; i < vertexes.size(); ++i) {
    m_vertexes.push_back(dynamic_cast<Point *>(vertexes[i]->Clone()));
  }
  m_flag      = flag;
  m_number    = (uint)vertexes.size();
  m_m         = m;
  m_n         = n;
  m_elevation = elevation;
}

Polyline::Polyline(uint number, uint m, uint n, int flag, double elevation) {
  m_flag      = flag;
  m_number    = number;
  m_m         = m;
  m_n         = n;
  m_elevation = elevation;
}

Polyline::Polyline(const Polyline &ployline) {
  m_flag                        = ployline.m_flag;
  m_number                      = ployline.m_number;
  m_m                           = ployline.m_m;
  m_n                           = ployline.m_n;
  m_elevation                   = ployline.m_elevation;
  std::vector<Point *> vertexes = ployline.GetVertexes();
  for (size_t i = 0; i < vertexes.size(); ++i) {
    m_vertexes.push_back(dynamic_cast<Point *>(vertexes[i]->Clone()));
  }
  m_uuid         = ployline.GetId();
  m_boundingRect = ployline.BoundingRect();
  m_polyline     = ployline.GetPolyline();
  SetAttributes(ployline.GetAttributes());
}

Polyline::~Polyline() {
  DeleteObjects();
}

void Polyline::Draw(QPainter &painter) {
  painter.setPen(m_painterAttributes.m_pen);
  for (size_t i = 0; i < m_polyline.size(); ++i) {
    QPolygonF polygon;
    polygon.append(QVector<PointF>::fromStdVector(m_polyline[i]));
    painter.drawPolyline(polygon);
  }
}

Entity *Polyline::Clone() {
  Entity *polyline = new Polyline(*this);
  return polyline;
}

void Polyline::Transform(double *params, int size) {
  if (size >= 4) {
    for (size_t i = 0; i < m_vertexes.size(); ++i) {
      Entity *entity = m_vertexes[i];
      entity->Transform(params, 4);
    }
    ExportPolyline();
    m_boundingRect = CalBoundingRect();
  }
}

void Polyline::Scale(double ratio) {
  for (size_t i = 0; i < m_vertexes.size(); ++i) {
    Entity *entity = m_vertexes[i];
    entity->Scale(ratio);
  }
  ExportPolyline();
  m_boundingRect = CalBoundingRect();
}

void Polyline::Transfer(double dx, double dy, double dz) {
  for (size_t i = 0; i < m_vertexes.size(); ++i) {
    Entity *entity = m_vertexes[i];
    entity->Transfer(dx, dy, dz);
  }
  ExportPolyline();
  m_boundingRect = CalBoundingRect();
}

void Polyline::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(angle)
  Q_UNUSED(cx)
  Q_UNUSED(cy)
  Q_UNUSED(cz)
}
const std::vector<Point *> &Polyline::GetVertexes() const {
  return m_vertexes;
}

void Polyline::AddVertex(Point &vertex) {
  Point *pt = dynamic_cast<Point *>(vertex.Clone());
  m_vertexes.push_back(pt);
}

void Polyline::DeleteObjects() {
  DELETE_OBJS(m_vertexes);
}

void Polyline::CorrectCoord(double bx, double by, double bz, double sx,
    double sy, double sz, double rotaAngle) {
  for (size_t i = 0; i < m_vertexes.size(); ++i) {
    m_vertexes[i]->CorrectCoord(bx, by, bz, sx, sy, sz, rotaAngle);
  }
  ExportPolyline();
}

bool Polyline::IsPickUp(double x, double y, double z) {
  RectF rect = m_boundingRect;
  rect.adjust(-m_hitRange, -m_hitRange, m_hitRange, m_hitRange);
  if (rect.contains(x, y)) {
    double minX     = 0.0;
    double maxX     = 0.0;
    double minY     = 0.0;
    double maxY     = 0.0;
    double width    = 0.0;
    double height   = 0.0;
    double A        = 0.0;
    double B        = 0.0;
    double C        = 0.0;
    double distance = 0.0;
    for (size_t i = 0; i < m_polyline.size(); ++i) {
      std::vector<PointF> &points = m_polyline[i];
      for (int n = 0; n < (int)points.size() - 1; ++n) {
        PointF &point0 = points[(size_t)n];
        PointF &point1 = points[(size_t)n + 1];
        minX           = std::min(point0.x(), point1.x());
        maxX           = std::max(point0.x(), point1.x());
        minY           = std::min(point0.y(), point1.y());
        maxY           = std::max(point0.y(), point1.y());
        width          = maxX - minX;
        height         = maxY - minY;
        RectF innerRect(minX, minY, width, height);
        if (width < m_hitRange || height < m_hitRange) {
          innerRect.adjust(-m_hitRange, -m_hitRange, m_hitRange, m_hitRange);
        }
        if (!innerRect.contains(x, y)) continue;
        A        = (point1.y() - point0.y());
        B        = (point0.x() - point1.x());
        C        = (point1.x() * point0.y()) - (point0.x() * point1.y());
        distance = fabs((A * x + B * y + C) / sqrt(A * A + B * B));
        if (distance <= m_hitRange) { return true; }
      }
    }
  }
  return false;
}

RectF Polyline::CalBoundingRect() {
  std::vector<double> vx;
  std::vector<double> vy;
  for (size_t i = 0; i < m_polyline.size(); ++i) {
    std::vector<PointF> &points = m_polyline[i];
    for (size_t n = 0; n < points.size(); ++n) {
      PointF &point = points[n];
      vx.push_back(point.x());
      vy.push_back(point.y());
    }
  }
  if (vx.size() > 0 && vy.size() > 0) {
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
    if (height < Entity::m_BoundingRectExSize) {
      rect.adjust(
          0, -Entity::m_BoundingRectExSize, 0, Entity::m_BoundingRectExSize);
    }
    return rect;
  } else {
    return RectF(0, 0, 0, 0);
  }
}

void Polyline::ExportPolyline() {
  m_polyline.clear();
  if (m_flag == 1 && m_vertexes.size() > 0) {
    m_vertexes.push_back(dynamic_cast<Point *>(m_vertexes[0]->Clone()));
  }
  int ptSize = (int)m_vertexes.size();
  for (int i = 0; i < ptSize - 1; ++i) {
    Point *pt1               = dynamic_cast<Point *>(m_vertexes[(size_t)i]);
    Point *pt2               = dynamic_cast<Point *>(m_vertexes[(size_t)i + 1]);
    Utility::ArcInfo arcInfo = Utility::CalArcInfo(*pt1, *pt2);
    if (!arcInfo.bUse) {
      PointF startPt(pt1->GetDrawableX(), pt1->GetDrawableY());
      PointF endPt(pt2->GetDrawableX(), pt2->GetDrawableY());
      std::vector<PointF> line;
      line.push_back(startPt);
      line.push_back(endPt);
      m_polyline.push_back(line);
    } else {
      m_polyline.push_back(arcInfo.arc);
    }
  }
  //闭合,8是三维多线段,1是圆环，flag=2：拟合
  if (m_flag == 1) {
    size_t ptSize = m_vertexes.size();
    if (ptSize >= 2) {
      Point *pt1 = dynamic_cast<Point *>(m_vertexes[0]);
      Point *pt2 = dynamic_cast<Point *>(m_vertexes[ptSize - 1]);
      PointF startPt(pt1->GetDrawableX(), pt1->GetDrawableY());
      PointF endPt(pt2->GetDrawableX(), pt2->GetDrawableY());
      std::vector<PointF> line;
      line.push_back(startPt);
      line.push_back(endPt);
      m_polyline.push_back(line);
    }
  }
}

} // namespace Entities
