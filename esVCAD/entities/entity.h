#ifndef ENTITY_H
#define ENTITY_H
#include <QPainter>
#include <entities/layer.h>
#include <string>
#include <vector>

namespace Entities {
class Entity;
typedef std::vector<Entities::Entity *> EntityContainer;

#define DELETE_OBJS(entities) \
  for (size_t i = 0; i < entities.size(); ++i) { \
    if (entities[i]) { \
      delete entities[i]; \
      entities[i] = NULL; \
    } \
  } \
  entities.clear();

//图元类型
enum EntityType {
  BaseType,
  PointType,
  LineType,
  CircleType,
  EllipseType,
  ArcType,
  RectType,
  SplineType,
  PolylineType,
  XLineType,
  RayType,
  CoordXYType,
  Face3dType,
  Total

};
//图元属性
//绘制状态
enum DrawState { Normal, Drawing, Select, Hover, Hide };
typedef char byte;
typedef QColor Color;
typedef QPointF PointF;
typedef QPoint PointI;
typedef QRectF RectF;
typedef QRect RectI;
typedef QPen Pen;
#define PI 3.141592653589793
#define EXTEND_LENGTH 1E8
typedef struct _tagAttributes {
  _tagAttributes() :
      layer_name(""), layer_block_color_flag(-1), color(-1),
      layer_block_color_width(-1), line_width(0), line_type("") {}
  std::string layer_name;      //图层名
  byte layer_block_color_flag; //判断是layer、block,还是颜色值
  int color;
  byte layer_block_color_width; //判断是layer、block、default,还是width
  int line_width;
  std::string line_type; //线型

} Attributes;

/**
 *绘图属性
 */
typedef struct _tagPainterAttributes {
  Pen m_pen;
  _tagPainterAttributes() {
    m_pen.setColor(Qt::white);
    m_pen.setWidthF(2.0);
    m_pen.setStyle(Qt::SolidLine);
  }
} PainterAttributes;

//图元基类
class Entity {
 public:
  Entity() : m_drawState(DrawState::Normal), m_layer(nullptr), m_hitRange(1.0) {
    m_uuid = CreateUuid();
  }
  virtual ~Entity() {}

 public:
  virtual EntityType GetType() { return EntityType::BaseType; }
  virtual void Draw(QPainter &painter) {}
  virtual Entity *Clone() {
    Entity *entity = new Entity;
    entity->SetAttributes(m_attributes);
    entity->SetId(m_uuid);
    return entity;
  }
  virtual void Transform(double *params, int size = 9) {}
  virtual void Scale(double ratio) {}
  virtual void Transfer(double dx, double dy, double dz) {}
  virtual void Rotate(double angle, double cx, double cy, double cz) {}
  virtual bool IsPickUp(double x, double y, double z) { return false; }
  virtual const std::string ToString() { return "BaseEntity"; }
  virtual const RectF &BoundingRect() const { return m_boundingRect; }
  const Attributes &GetAttributes() const { return m_attributes; }
  void SetAttributes(const Attributes &attributes) {
    m_attributes.layer_name              = attributes.layer_name;
    m_attributes.layer_block_color_flag  = attributes.layer_block_color_flag;
    m_attributes.color                   = attributes.color;
    m_attributes.layer_block_color_width = attributes.layer_block_color_width;
    m_attributes.line_width              = attributes.line_width;
    m_attributes.line_type               = attributes.line_type;
  }
  const PainterAttributes &GetPainterAttributes() const {
    return m_painterAttributes;
  }
  void SetPainterAttributes(const PainterAttributes &painterAttributes) {
    m_painterAttributes = painterAttributes;
  }
  const std::string &GetId() const { return m_uuid; }
  void SetId(const std::string &id) { m_uuid = id; }
  virtual DrawState GetDrawState() const { return m_drawState; }
  virtual void SetDrawState(DrawState drawState) { m_drawState = drawState; }
  //处理插入块的坐标
  virtual void CorrectCoord(double bx, double by, double bz, double sx,
      double sy, double sz, double rotaAngle) {}
  double GetHitRange() { return m_hitRange; }
  void SetHitRange(double hitRange) { m_hitRange = hitRange; }

 protected:
  Attributes m_attributes;
  PainterAttributes m_painterAttributes;
  DrawState m_drawState;
  Layer *m_layer; //图元所属图层
  std::string m_uuid;
  double m_hitRange;
  RectF m_boundingRect;
  static double m_BoundingRectExSize;

 public:
  void SetLayer(Layer *layer) { m_layer = layer; }
  Layer *GetLayer() const { return m_layer; }

 public:
  static std::string CreateUuid();
  static void SetBoundingRectExSize(double exSize);
  static double GetBoundingRectExSize();
};
} // namespace Entities
#endif // ENTITY_H
