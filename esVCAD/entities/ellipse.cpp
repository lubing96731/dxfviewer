#include "ellipse.h"
#include "utility.h"
#include <qdebug.h>
#define USE_QT_DRAW 0
#define USE_CUSTOM_DRAW 1

namespace Entities {
Ellipse::Ellipse() {
  m_drawFlag           = 0;
  m_rotaAngle          = 0.0;
  m_ratio              = 0.0;
  m_startAngle         = 0.0;
  m_endAngle           = 0.0;
  m_a                  = 0.0;
  m_b                  = 0.0;
  m_drawableStartAngle = 0.0;
  m_drawableEndAngle   = 0.0;
  m_drawableAngleLen   = 0.0;
}
Ellipse::Ellipse(const Point &center, const Point &majorPos, double ratio,
    double startAngle, double endAngle) {
  m_center             = center;
  m_majorPos           = majorPos;
  m_ratio              = ratio;
  m_startAngle         = startAngle;
  m_endAngle           = endAngle;
  m_drawFlag           = 0;
  m_rotaAngle          = 0.0;
  m_a                  = 0.0;
  m_b                  = 0.0;
  m_drawableStartAngle = 0.0;
  m_drawableEndAngle   = 0.0;
  m_drawableAngleLen   = 0.0;
}

Ellipse::Ellipse(const Ellipse &ellipse) {
  m_center                            = ellipse.GetCenter();
  m_majorPos                          = ellipse.GetMajorPos();
  m_ratio                             = ellipse.GetRatio();
  m_startAngle                        = ellipse.GetStartAngle();
  m_endAngle                          = ellipse.GetEndAngle();
  m_drawFlag                          = ellipse.GetDrawFlag();
  m_rotaAngle                         = ellipse.GetRotaAngle();
  m_a                                 = ellipse.GetA();
  m_b                                 = ellipse.GetB();
  m_drawableStartAngle                = ellipse.GetDrawableStartAngle();
  m_drawableEndAngle                  = ellipse.GetDrawableEndAngle();
  m_drawableAngleLen                  = ellipse.GetDrawableAngleLen();
  std::vector<Point *> rotaEllipsePts = ellipse.GetRotaEllipsePts();
  for (size_t i = 0; i < rotaEllipsePts.size(); ++i) {
    m_rotaEllipsePts.push_back(
        dynamic_cast<Point *>(rotaEllipsePts[i]->Clone()));
  }
  m_uuid         = ellipse.GetId();
  m_boundingRect = ellipse.BoundingRect();
  SetAttributes(ellipse.GetAttributes());
}

Ellipse::~Ellipse() {
  DeleteObjects();
}
void Ellipse::DeleteObjects() {
  DELETE_OBJS(m_rotaEllipsePts);
}
void Ellipse::Draw(QPainter &painter) {
  painter.setPen(m_painterAttributes.m_pen);
  //		double centerX = m_center.GetDrawableX();
  //		double centerY = m_center.GetDrawableY();
  //		int majorPosXFlag = m_center.GetX() - m_majorPos.GetX();
  //椭圆或者圆弧没有旋转,用qt绘制
  // if (m_drawFlag == USE_QT_DRAW) {
  //	double w = m_a;
  //	double h = m_b;
  //	if (majorPosXFlag == 0) { std::swap(w, h); }
  //	RectF outRect(centerX - w, centerY - h, w * 2, h * 2);
  //	painter.drawArc(
  //		outRect, (int)m_drawableStartAngle, (int)m_drawableAngleLen);

  //}
  // else if (m_drawFlag == USE_CUSTOM_DRAW) {
  //有旋转角度，自己绘制
  PointF p1, p2;
  for (int i = 0; i < (int)m_rotaEllipsePts.size() - 1; ++i) {
    double x1 = m_rotaEllipsePts[(size_t)i]->GetDrawableX();
    double y1 = m_rotaEllipsePts[(size_t)i]->GetDrawableY();
    double x2 = m_rotaEllipsePts[(size_t)i + 1]->GetDrawableX();
    double y2 = m_rotaEllipsePts[(size_t)i + 1]->GetDrawableY();
    p1.setX(x1);
    p1.setY(y1);
    p2.setX(x2);
    p2.setY(y2);
    painter.drawLine(p1, p2);
  }
  //}
}

void Ellipse::CalAB() {
  double centerX = m_center.GetDrawableX();
  double centerY = m_center.GetDrawableY();
  double majorX  = m_majorPos.GetDrawableX();
  double majorY  = m_majorPos.GetDrawableY();
  m_a            = sqrt((majorX - centerX) * (majorX - centerX) +
      (majorY - centerY) * (majorY - centerY));
  m_b            = m_a * m_ratio;
}
void Ellipse::ExportEllipse() {
  DeleteObjects();
  int majorPosXFlag = (int)(m_center.GetX() - m_majorPos.GetX());
  double w          = m_a;
  double h          = m_b;
  if (majorPosXFlag == 0) { std::swap(w, h); }
  double centerX    = m_center.GetDrawableX();
  double centerY    = m_center.GetDrawableY();
  double stepAngle  = PI / 45;
  double cosValue   = cos(m_rotaAngle);
  double sinValue   = sin(m_rotaAngle);
  double startAngle = m_drawableStartAngle * PI / 2880;
  double endAngle   = m_drawableEndAngle * PI / 2880;
  double angle      = startAngle;
  for (;;) {
    double x     = w * cos(angle);
    double y     = h * sin(angle);
    double newX  = x * cosValue - y * sinValue;
    double newY  = x * sinValue + y * cosValue;
    Point *point = new Point;
    point->SetDrawableX(newX + centerX);
    point->SetDrawableY(centerY - newY);
    m_rotaEllipsePts.push_back(point);
    angle += stepAngle;
    if (angle >= endAngle) {
      x            = w * cos(endAngle);
      y            = h * sin(endAngle);
      newX         = x * cosValue - y * sinValue;
      newY         = x * sinValue + y * cosValue;
      Point *point = new Point;
      point->SetDrawableX(newX + centerX);
      point->SetDrawableY(centerY - newY);
      m_rotaEllipsePts.push_back(point);
      break;
    }
  }
}
Entity *Ellipse::Clone() {
  Entity *ellipse = new Ellipse(*this);
  return ellipse;
}

void Ellipse::Transform(double *params, int size) {
  m_center.Transform(params, size);
  m_majorPos.Transform(params, size);
  CalAB();

  int majorPosXFlag = (int)round(m_majorPos.GetX() - m_center.GetX());
  int majorPosYFlag = (int)round(m_majorPos.GetY() - m_center.GetY());
  if ((majorPosXFlag == 0 && majorPosYFlag != 0) ||
      (majorPosXFlag != 0 && majorPosYFlag == 0)) {
    m_drawFlag       = USE_QT_DRAW;
    double rotaAngle = 0.0;
    if (majorPosXFlag == 0 && majorPosYFlag > 0) {
      rotaAngle = PI * 0.5;
    } else if (majorPosXFlag == 0 && majorPosYFlag < 0) {
      rotaAngle = PI * 1.5;
    } else if (majorPosYFlag == 0 && majorPosXFlag > 0) {
      rotaAngle = 0.0;
    } else if (majorPosYFlag == 0 && majorPosXFlag < 0) {
      rotaAngle = PI;
    }
    m_drawableStartAngle = ((m_startAngle + rotaAngle) / PI) * 2880; // 180*16
    m_drawableEndAngle   = ((m_endAngle + rotaAngle) / PI) * 2880;
    m_drawableAngleLen   = m_drawableEndAngle - m_drawableStartAngle;
    //产生椭圆上的点
    ExportEllipse();
  } else if (majorPosXFlag != 0 && majorPosYFlag != 0) {
    //计算旋转角度
    double dy   = m_majorPos.GetY() - m_center.GetY();
    double dx   = m_majorPos.GetX() - m_center.GetX();
    double k    = dy / dx;
    m_rotaAngle = atan(k);
    if (dx < 0) {
      m_rotaAngle = PI + m_rotaAngle;
    } else if (dx > 0) {
      m_rotaAngle = PI * 2 + m_rotaAngle;
    }
    m_drawableStartAngle = (m_startAngle / PI) * 2880; // 180*16
    m_drawableEndAngle   = (m_endAngle / PI) * 2880;
    m_drawableAngleLen   = m_drawableEndAngle - m_drawableStartAngle;
    m_drawFlag           = USE_CUSTOM_DRAW;
    //产生椭圆上的点
    ExportEllipse();
  }
  m_boundingRect = CalBoundingRect();
}
void Ellipse::Scale(double ratio) {
  m_center.Scale(ratio);
  m_majorPos.Scale(ratio);
  CalAB();
  ExportEllipse();
  m_boundingRect = CalBoundingRect();
}

void Ellipse::Transfer(double dx, double dy, double dz) {
  m_center.Transfer(dx, dy, dz);
  m_majorPos.Transfer(dx, dy, dz);
  CalAB();
  ExportEllipse();
  m_boundingRect = CalBoundingRect();
}

void Ellipse::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(angle)
  Q_UNUSED(cx)
  Q_UNUSED(cy)
  Q_UNUSED(cz)
}

void Ellipse::CorrectCoord(double bx, double by, double bz, double sx,
    double sy, double sz, double rotaAngle) {
  m_center.CorrectCoord(bx, by, bz, sx, sy, sz, rotaAngle);
  m_majorPos.CorrectCoord(bx, by, bz, sx, sy, sz, rotaAngle);
}

bool Ellipse::IsPickUp(double x, double y, double z) {
  Q_UNUSED(z)
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
    for (int i = 0; i < (int)m_rotaEllipsePts.size() - 1; ++i) {
      Point *pt0 = m_rotaEllipsePts[(size_t)i];
      Point *pt1 = m_rotaEllipsePts[(size_t)i + 1];
      minX       = std::min(pt0->GetDrawableX(), pt1->GetDrawableX());
      maxX       = std::max(pt0->GetDrawableX(), pt1->GetDrawableX());
      minY       = std::min(pt0->GetDrawableY(), pt1->GetDrawableY());
      maxY       = std::max(pt0->GetDrawableY(), pt1->GetDrawableY());
      width      = maxX - minX;
      height     = maxY - minY;
      RectF innerRect(minX, minY, width, height);
      if (width < m_hitRange || height < m_hitRange) {
        innerRect.adjust(-m_hitRange, -m_hitRange, m_hitRange, m_hitRange);
      }
      if (!innerRect.contains(x, y)) continue;
      A = (pt1->GetDrawableY() - pt0->GetDrawableY());
      B = (pt0->GetDrawableX() - pt1->GetDrawableX());
      C = (pt1->GetDrawableX() * pt0->GetDrawableY()) -
          (pt0->GetDrawableX() * pt1->GetDrawableY());
      distance = fabs((A * x + B * y + C) / sqrt(A * A + B * B));
      if (distance <= m_hitRange) { return true; }
    }
  }
  return false;
}

RectF Ellipse::CalBoundingRect() {
  std::vector<double> vx;
  std::vector<double> vy;
  for (size_t i = 0; i < m_rotaEllipsePts.size(); ++i) {
    double x = m_rotaEllipsePts[i]->GetDrawableX();
    double y = m_rotaEllipsePts[i]->GetDrawableY();
    vx.push_back(x);
    vy.push_back(y);
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
    if (height < m_BoundingRectExSize) {
      rect.adjust(
          0, -Entity::m_BoundingRectExSize, 0, Entity::m_BoundingRectExSize);
    }
    return rect;
  } else {
    return RectF(0, 0, 0, 0);
  }
}

} // namespace Entities
