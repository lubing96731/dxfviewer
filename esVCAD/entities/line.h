#ifndef LINE_H
#define LINE_H
#include "point.h"

namespace Entities {
class Line : public Entity {
 public:
  Line();
  Line(const Point &p1, const Point &p2);
  Line(const Line &line);
  ~Line();

 public:
  virtual EntityType GetType() { return EntityType::LineType; }
  virtual void Draw(QPainter &painter);
  virtual Entity *Clone();
  // dxf cad坐标转换为窗口坐标
  virtual void Transform(double *params, int size = 9);
  virtual void Scale(double ratio);
  virtual void Transfer(double dx, double dy, double dz);
  virtual void Rotate(double angle, double cx, double cy, double cz);
  virtual const std::string ToString() { return "Line"; }
  virtual void CorrectCoord(double bx, double by, double bz, double sx,
      double sy, double sz, double rotaAngle);
  virtual bool IsPickUp(double x, double y, double z);

 private:
  RectF CalBoundingRect();

 private:
  Point m_startPoint;
  Point m_endPoint;

 public:
  void SetStartPoint(const Point &point) { m_startPoint = point; }
  const Point &GetStartPoint() const { return m_startPoint; }

  void SetEndPoint(const Point &point) { m_endPoint = point; }
  const Point &GetEndPoint() const { return m_endPoint; }
};

} // namespace Entities

#endif // LINE_H
