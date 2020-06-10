#include "spline.h"
#define CONTROL_TYPE 8
#define FIT_TYPE 1064
#include <qdebug.h>
#include <tinysplinecpp.h>

namespace Entities {
Spline::Spline(uint degree, uint nKnots, uint nControl, uint nFit, int flags,
    const Point &startPoint, const Point &endPoint) {
  m_degree            = degree;
  m_nKnots            = nKnots;
  m_nControl          = nControl;
  m_nFit              = nFit;
  m_flags             = flags;
  m_tangentStartPoint = startPoint;
  m_tangentEndPoint   = endPoint;
}

Spline::Spline(const Spline &spline) {
  m_degree                             = spline.m_degree;
  m_nKnots                             = spline.m_nKnots;
  m_nControl                           = spline.m_nControl;
  m_nFit                               = spline.m_nFit;
  m_flags                              = spline.m_flags;
  m_tangentStartPoint                  = spline.m_tangentStartPoint;
  m_tangentEndPoint                    = spline.m_tangentEndPoint;
  std::vector<Point *> _controlPts     = spline.GetControlPts();
  std::vector<Point *> _fitPts         = spline.GetFitPts();
  std::vector<double> _knotPts         = spline.GetKnotPts();
  std::vector<Point *> _drawablePoints = spline.GetDrawablePoints();
  for (size_t i = 0; i < _controlPts.size(); ++i) {
    m_controlPts.push_back(dynamic_cast<Point *>(_controlPts[i]->Clone()));
  }
  for (size_t i = 0; i < _fitPts.size(); ++i) {
    m_fitPts.push_back(dynamic_cast<Point *>(_fitPts[i]->Clone()));
  }
  for (size_t i = 0; i < _knotPts.size(); ++i) {
    m_knotPts.push_back(_knotPts[i]);
  }
  for (size_t i = 0; i < _drawablePoints.size(); ++i) {
    m_drawabelPoints.push_back(
        dynamic_cast<Point *>(_drawablePoints[i]->Clone()));
  }
  m_uuid         = spline.GetId();
  m_boundingRect = spline.BoundingRect();
  SetAttributes(spline.GetAttributes());
}
Spline::~Spline() {
  DeleteObjects();
  DeleteDrawableObjects();
}

void Spline::Draw(QPainter &painter) {
  painter.setPen(m_painterAttributes.m_pen);
  PointF p1, p2;
  for (int i = 0; i < (int)m_drawabelPoints.size() - 1; i++) {
    p1.setX(m_drawabelPoints[(size_t)i]->GetDrawableX());
    p1.setY(m_drawabelPoints[(size_t)i]->GetDrawableY());
    p2.setX(m_drawabelPoints[(size_t)i + 1]->GetDrawableX());
    p2.setY(m_drawabelPoints[(size_t)i + 1]->GetDrawableY());
    painter.drawLine(p1, p2);
  }
}

Entity *Spline::Clone() {
  Entity *spline = new Spline(*this);
  return spline;
}

void Spline::Transform(double *params, int size) {
  for (size_t i = 0; i < m_controlPts.size(); ++i) {
    Entity *entity = m_controlPts[i];
    entity->Transform(params, size);
  }
  ExportSpline();
  m_boundingRect = CalBoundingRect();
}
void Spline::Scale(double ratio) {
  for (size_t i = 0; i < m_controlPts.size(); ++i) {
    Entity *entity = m_controlPts[i];
    entity->Scale(ratio);
  }
  ExportSpline();
  m_boundingRect = CalBoundingRect();
}

void Spline::Transfer(double dx, double dy, double dz) {
  for (size_t i = 0; i < m_controlPts.size(); ++i) {
    Entity *entity = m_controlPts[i];
    entity->Transfer(dx, dy, dz);
  }
  ExportSpline();
  m_boundingRect = CalBoundingRect();
}

void Spline::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(angle)
  Q_UNUSED(cx)
  Q_UNUSED(cy)
  Q_UNUSED(cz)
}
void Spline::AddKnots(double k) {
  m_knotPts.push_back(k);
}
void Spline::AddControlPts(Point *pt) {
  m_controlPts.push_back(pt);
}
void Spline::AddFitPts(Point *pt) {
  m_fitPts.push_back(pt);
}
void Spline::DeleteObjects() {
  DELETE_OBJS(m_controlPts);
  DELETE_OBJS(m_fitPts);
  m_knotPts.clear();
}
void Spline::DeleteDrawableObjects() {
  DELETE_OBJS(m_drawabelPoints);
}

void Spline::CalXYMinMax(std::vector<Point *> &controlPts, double &xMin,
    double &xMax, double &yMin, double &yMax) {
  std::vector<double> vx;
  std::vector<double> vy;
  for (size_t i = 0; i < controlPts.size(); ++i) {
    vx.push_back(controlPts[i]->GetDrawableX());
    vy.push_back(controlPts[i]->GetDrawableY());
  }
  xMin = *std::min_element(vx.begin(), vx.end());
  xMax = *std::max_element(vx.begin(), vx.end());
  yMin = *std::min_element(vy.begin(), vy.end());
  yMax = *std::max_element(vy.begin(), vy.end());
}
void Spline::ExportSpline() {
  DeleteDrawableObjects();
  if (m_nControl == 2) {
    Point *pt0 = new Point;
    Point *pt1 = new Point;
    pt0->SetDrawableX(m_controlPts[0]->GetDrawableX());
    pt0->SetDrawableY(m_controlPts[0]->GetDrawableY());
    pt1->SetDrawableX(m_controlPts[1]->GetDrawableX());
    pt1->SetDrawableY(m_controlPts[1]->GetDrawableY());
    m_drawabelPoints.push_back(pt0);
    m_drawabelPoints.push_back(pt1);
  } else if (m_nControl > 2) //二阶
  {
    tinyspline::BSpline *spline = nullptr;
    if (m_nControl == 3) {
      spline = new tinyspline::BSpline(m_nControl, 2, 2);
    } else if (m_nControl > 3) {
      spline = new tinyspline::BSpline(m_nControl);
    }
    std::vector<tinyspline::real> ctrlpts = spline->controlPoints();
    for (uint i = 0; i < m_nControl; ++i) {
      double x           = m_controlPts[i]->GetDrawableX();
      double y           = m_controlPts[i]->GetDrawableY();
      ctrlpts[2 * i]     = x;
      ctrlpts[2 * i + 1] = y;
    }
    spline->setControlPoints(ctrlpts);
    std::vector<tinyspline::real> result;
    double xMin = 0.0, xMax = 0.0, yMin = 0.0, yMax = 0.0;
    CalXYMinMax(m_controlPts, xMin, xMax, yMin, yMax);
    double sampleCount = qMax(fabs(xMax - xMin), fabs(yMax - yMin));
    double u           = 0.0;
    if (sampleCount < 100) sampleCount = 100;
    double du   = 1.0 / (sampleCount);
    double endU = 1.0;
    for (;;) {
      result = spline->eval(u).result();
      if (result.size() == 0) continue;
      Point *point = new Point;
      point->SetDrawableX(result[0]);
      point->SetDrawableY(result[1]);
      m_drawabelPoints.push_back(point);
      result.clear();
      u += du;
      if (u >= 1.0) {
        result = spline->eval(endU).result();
        if (result.size() != 0) {
          Point *point = new Point;
          point->SetDrawableX(result[0]);
          point->SetDrawableY(result[1]);
          m_drawabelPoints.push_back(point);
          result.clear();
        }
        break;
      }
    }
    delete spline;
    spline = nullptr;
  }
}

void Spline::CorrectCoord(double bx, double by, double bz, double sx, double sy,
    double sz, double rotaAngle) {
  for (size_t i = 0; i < m_controlPts.size(); ++i) {
    m_controlPts[i]->CorrectCoord(bx, by, bz, sx, sy, sz, rotaAngle);
  }
}

bool Spline::IsPickUp(double x, double y, double z) {
  Q_UNUSED(z)
  RectF rect = m_boundingRect;
  rect.adjust(-m_hitRange, -m_hitRange, m_hitRange, m_hitRange);
  if (rect.contains(x, y)) {
    if (m_nControl == 2) {
      Point *startPoint = m_drawabelPoints[0];
      Point *endPoint   = m_drawabelPoints[1];
      double A = (endPoint->GetDrawableY() - startPoint->GetDrawableY());
      double B = (startPoint->GetDrawableX() - endPoint->GetDrawableX());
      double C = (endPoint->GetDrawableX() * startPoint->GetDrawableY()) -
          (startPoint->GetDrawableX() * endPoint->GetDrawableY());
      double distance = fabs((A * x + B * y + C) / sqrt(A * A + B * B));
      if (distance <= m_hitRange) { return true; }
    } else if (m_nControl > 2) {
      RectF hitRect(
          x - m_hitRange, y - m_hitRange, 2 * m_hitRange, 2 * m_hitRange);
      for (size_t i = 0; i < m_drawabelPoints.size(); i++) {
        if (hitRect.contains(m_drawabelPoints[i]->GetDrawableX(),
                m_drawabelPoints[i]->GetDrawableY())) {
          return true;
        }
      }
    }
  }
  return false;
}

RectF Spline::CalBoundingRect() {
  std::vector<double> vx, vy;
  for (size_t i = 0; i < m_drawabelPoints.size(); i++) {
    vx.push_back(m_drawabelPoints[i]->GetDrawableX());
    vy.push_back(m_drawabelPoints[i]->GetDrawableY());
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
  }
  return RectF(0, 0, 0, 0);
}

} // namespace Entities
