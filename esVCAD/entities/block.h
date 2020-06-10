#ifndef BLOCK_H
#define BLOCK_H
#include "point.h"

namespace Entities {
class Block {
 public:
  typedef struct _tagInsertData {
    std::string m_name;  //块名
    Point m_insertPoint; //插入基点
    Point m_scalePoint;  //缩放系数
    double m_angle;      //旋转角度
    int m_cols;          //块数组x索引,未使用
    int m_rows;          //块数组y索引,未使用
    double m_colSp;      // x边缘间距,未使用
    double m_rowSp;      // y边缘间距,未使用
  } InsertData;
  Block();
  Block(const std::string &name, const Point &insertPoint,
      const Point &scalePoint, double angle, int cols, int rows, double colSp,
      double rowSp);
  Block(const InsertData &insertData);
  Block(const Block &block);
  ~Block();

 private:
  InsertData m_insertData;
  bool m_isUse;
  Attributes m_attributes;
  bool m_bEndBlock;
  std::vector<Block *> m_refBlocks; //引用的block
 private:
  EntityContainer m_entities;
  double m_hitRange;

 protected:
  std::string m_uuid;
  RectF m_boundingRect;

 public:
  void SetInsertData(const InsertData &insertData) {
    m_insertData = insertData;
  }
  const InsertData &GetInsertData() { return m_insertData; }

  const std::string &GetName() const { return m_insertData.m_name; }
  const Point &GetInsertPoint() const { return m_insertData.m_insertPoint; }
  const Point &GetScalePoint() const { return m_insertData.m_scalePoint; }
  double GetAngle() const { return m_insertData.m_angle; }

  int GetCols() const { return m_insertData.m_cols; }
  int GetRows() const { return m_insertData.m_rows; }
  double GetColSp() const { return m_insertData.m_colSp; }
  double GetRowSp() const { return m_insertData.m_rowSp; }
  bool IsUse() const { return m_isUse; }
  bool IsEndBlock() const { return m_bEndBlock; }
  const Attributes GetAttributes() const { return m_attributes; }

  void SetName(const std::string &name) { m_insertData.m_name = name; }
  void SetInsertPoint(const Point &insertPoint) {
    m_insertData.m_insertPoint = insertPoint;
  }
  void SetScalePoint(const Point &scalePoint) {
    m_insertData.m_scalePoint = scalePoint;
  }
  void SetAngle(double angle) { m_insertData.m_angle = angle; }
  void SetIsUse(bool bUse) { m_isUse = bUse; }
  void SetIsEndBlock(bool bEndBlock) { m_bEndBlock = bEndBlock; }
  void SetCols(int cols) { m_insertData.m_cols = cols; }
  void SetRows(int rows) { m_insertData.m_rows = rows; }
  void SetColSp(double colSp) { m_insertData.m_colSp = colSp; }
  void SetRowSp(double rowSp) { m_insertData.m_rowSp = rowSp; }
  void SetAttributes(const Attributes &attributes) {
    m_attributes = attributes;
  }
  bool IsEmpty() { return m_entities.size() == 0; }
  size_t GetElementSize() const { return m_entities.size(); }
  Entity *ElementAt(size_t index) const { return m_entities[index]; }
  void push_back(Entity *entity) { m_entities.push_back(entity); }
  Entity *operator[](size_t index) { return m_entities[index]; }

  std::vector<Block *> &GetRefBlocks() { return m_refBlocks; }

  const std::string &GetId() const { return m_uuid; }
  void SetId(const std::string &id) { m_uuid = id; }

 public:
  void CorrectCoord(); //矫正块内图元的坐标，包括偏移，比例和角度
  bool IsPickUp(double x, double y, double z);

 public:
  void Draw(QPainter &painter);
  Block *Clone();
  void Transform(std::vector<Layer *> &layers, double *params, int size = 9);
  void Scale(double ratio);
  void Transfer(double dx, double dy, double dz);
  void Rotate(double angle, double cx, double cy, double cz);
  const std::string ToString() { return "Block"; }
  Layer *FindLayerByName(std::string &name, std::vector<Layer *> &layers);
  void CopyEntityTo(Block *block);
  void DeleteObjs();
  const RectF &BoundingRect() const { return m_boundingRect; }
  double GetHitRange() { return m_hitRange; }
  void SetHitRange(double hitRange) { m_hitRange = hitRange; }

 private:
  RectF CalBoundingRect();
};

} // namespace Entities

#endif // BLOCK_H
