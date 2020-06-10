#include "utility.h"

#include <qmath.h>
Utility::Utility() {
}

double Utility::CalAngle(const Point &center, const Point &vertex0) {
  double dx         = vertex0.GetDrawableX() - center.GetDrawableX();
  double dy         = vertex0.GetDrawableY() - center.GetDrawableY();
  double angle      = 0.0;
  static double eps = 1e-8;
  if (fabs(dx) < eps) {
    if (dy <= 0) {
      angle = M_PI_2;
    } else {
      angle = M_PI * 1.5;
    }
  } else {
    double atanVal = fabs(qAtan(dy / dx));
    if (dx > 0 && dy <= 0) {
      angle = atanVal;
    } else if (dx < 0 && dy <= 0) {
      angle = M_PI - atanVal;
    } else if (dx < 0 && dy >= 0) {
      angle = M_PI + atanVal;
    } else if (dx > 0 && dy >= 0) {
      angle = M_PI * 2 - atanVal;
    }
  }
  return angle;
}
Utility::ArcInfo Utility::CalArcInfo(
    const Point &vertex0, const Point &vertex1) {
  bool clockWise = true; //顺时针
  ArcInfo arcInfo;
  double bulge = vertex0.GetBulge();
  if (bulge < 0.0) {
    clockWise = true;
  } else if (bulge > 0) {
    clockWise = false;
  } else if (bulge == 0.0) {
    arcInfo.bUse = false;
    return arcInfo;
  }
  //计算顶点角度
  double cicleAngle = fabs(atan(bulge)) * 4;
  //两点之间的距离
  double distance = Distance(vertex0, vertex1);
  //根据正玄值反推
  double halfAngle = cicleAngle / 2;
  double q         = distance / 2;
  arcInfo.radius   = q / sin(halfAngle);

  Point vertex2;
  double x0 = vertex0.GetDrawableX();
  double y0 = vertex0.GetDrawableY();
  double x1 = vertex1.GetDrawableX();
  double y1 = vertex1.GetDrawableY();

  double x2 = (x0 + x1) / 2;
  double y2 = (y0 + y1) / 2;

  double cx0 =
      x2 + sqrt(arcInfo.radius * arcInfo.radius - q * q) * (y0 - y1) / distance;
  double cy0 =
      y2 + sqrt(arcInfo.radius * arcInfo.radius - q * q) * (x1 - x0) / distance;
  double cx1 = x2 -
      sqrt((arcInfo.radius * arcInfo.radius) - q * q) * (y0 - y1) / distance;
  double cy1 = y2 -
      sqrt((arcInfo.radius * arcInfo.radius) - q * q) * (x1 - x0) / distance;

  if (clockWise) {
    if (cicleAngle < M_PI) {
      vertex2.SetDrawableX(cx0);
      vertex2.SetDrawableY(cy0);
    } else {
      vertex2.SetDrawableX(cx1);
      vertex2.SetDrawableY(cy1);
    }

  } else {
    if (cicleAngle < M_PI) {
      vertex2.SetDrawableX(cx1);
      vertex2.SetDrawableY(cy1);
    } else {
      vertex2.SetDrawableX(cx0);
      vertex2.SetDrawableY(cy0);
    }
  }

  arcInfo.center = vertex2;
  arcInfo.bUse   = true;

  double angle0     = CalAngle(vertex2, vertex0);
  double angle1     = CalAngle(vertex2, vertex1);
  double startAngle = 0.0;
  double endAngle   = 0.0;
  if (clockWise) {
    startAngle = angle1;
    endAngle   = startAngle + cicleAngle;
  } else {
    startAngle = angle0;
    endAngle   = startAngle + cicleAngle;
  }
  double step = M_PI / 45;
  PointF point;
  for (;;) {
    double x = arcInfo.radius * cos(startAngle) + vertex2.GetDrawableX();
    double y = vertex2.GetDrawableY() - arcInfo.radius * sin(startAngle);
    point.setX(x);
    point.setY(y);
    arcInfo.arc.push_back(point);
    startAngle += step;
    if (startAngle >= endAngle) {
      x = arcInfo.radius * cos(endAngle) + vertex2.GetDrawableX();
      y = vertex2.GetDrawableY() - arcInfo.radius * sin(endAngle);
      point.setX(x);
      point.setY(y);
      arcInfo.arc.push_back(point);
      break;
    }
  }
  return arcInfo;
}

double Utility::Distance(const Point &vertex0, const Point &vertex1) {
  double dx       = vertex0.GetDrawableX() - vertex1.GetDrawableX();
  double dy       = vertex0.GetDrawableY() - vertex1.GetDrawableY();
  double distance = sqrt(dx * dx + dy * dy);
  return distance;
}
