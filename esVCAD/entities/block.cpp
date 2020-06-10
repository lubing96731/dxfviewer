#include "block.h"
namespace Entities {
Block::Block() : m_isUse(false), m_bEndBlock(false) {
  m_insertData.m_name  = "";
  m_insertData.m_angle = 0.0;
  m_insertData.m_cols  = 0;
  m_insertData.m_rows  = 0;
  m_insertData.m_colSp = 0.0;
  m_insertData.m_rowSp = 0.0;
  m_uuid               = Entity::CreateUuid();
}
Block::Block(const std::string &name, const Point &insertPoint,
    const Point &scalePoint, double angle, int cols, int rows, double colSp,
    double rowSp) {
  m_insertData.m_name        = name;
  m_insertData.m_insertPoint = insertPoint;
  m_insertData.m_scalePoint  = scalePoint;
  m_insertData.m_angle       = angle;
  m_insertData.m_cols        = cols;
  m_insertData.m_rows        = rows;
  m_insertData.m_colSp       = colSp;
  m_insertData.m_rowSp       = rowSp;
  m_isUse                    = false;
  m_bEndBlock                = false;
  m_uuid                     = Entity::CreateUuid();
}

Block::Block(const InsertData &insertData) {
  m_insertData = insertData;
  m_uuid       = Entity::CreateUuid();
}

Block::Block(const Block &block) {
  m_insertData.m_name        = block.GetName();
  m_insertData.m_insertPoint = block.GetInsertPoint();
  m_insertData.m_scalePoint  = block.GetScalePoint();
  m_insertData.m_angle       = block.GetAngle();
  m_insertData.m_cols        = block.GetCols();
  m_insertData.m_rows        = block.GetRows();
  m_insertData.m_colSp       = block.GetColSp();
  m_insertData.m_rowSp       = block.GetRowSp();
  m_isUse                    = block.IsUse();
  m_bEndBlock                = block.IsEndBlock();
  for (size_t k = 0; k < block.GetElementSize(); ++k) {
    m_entities.push_back(block.ElementAt(k)->Clone());
  }
  std::vector<Block *> &refBlocks = const_cast<Block &>(block).GetRefBlocks();
  for (size_t k = 0; k < refBlocks.size(); ++k) {
    m_refBlocks.push_back(refBlocks[k]->Clone());
  }
  m_uuid         = block.GetId();
  m_boundingRect = block.BoundingRect();
  SetAttributes(block.GetAttributes());
}

Block::~Block() {
  DeleteObjs();
}

void Block::Draw(QPainter &painter) {
  for (size_t i = 0; i < m_entities.size(); ++i) {
    Layer *layer = m_entities[i]->GetLayer();
    //没有图层，直接显示图形
    if (layer == nullptr) continue;
    int flags = layer->GetFlags();
    if (!layer->IsOff() || flags == 4 || flags == 0) //图层关闭不显示
    {
      m_entities[i]->Draw(painter);
    }
  }
}

Block *Block::Clone() {
  Block *block = new Block(*this);
  return block;
}

void Block::Transform(std::vector<Layer *> &layers, double *params, int size) {
  for (size_t i = 0; i < m_entities.size(); ++i) {
    std::string layerName = m_entities[i]->GetAttributes().layer_name;
    Layer *layer          = FindLayerByName(layerName, layers);
    m_entities[i]->SetLayer(layer);
    m_entities[i]->Transform(params, size);
  }
  m_boundingRect = CalBoundingRect();
}
Layer *Block::FindLayerByName(std::string &name, std::vector<Layer *> &layers) {
  for (size_t i = 0; i < layers.size(); ++i) {
    if (layers[i]->GetName() == name) { return layers[i]; }
  }
  return nullptr;
}
void Block::Scale(double ratio) {
  for (size_t i = 0; i < m_entities.size(); ++i) {
    m_entities[i]->Scale(ratio);
  }
  m_boundingRect = CalBoundingRect();
}

void Block::Transfer(double dx, double dy, double dz) {
  for (size_t i = 0; i < m_entities.size(); ++i) {
    m_entities[i]->Transfer(dx, dy, dz);
  }
  m_boundingRect = CalBoundingRect();
}

void Block::Rotate(double angle, double cx, double cy, double cz) {
  Q_UNUSED(angle)
  Q_UNUSED(cx)
  Q_UNUSED(cy)
  Q_UNUSED(cz)
}

void Block::CorrectCoord() {
  for (size_t i = 0; i < m_entities.size(); ++i) {
    m_entities[i]->CorrectCoord(m_insertData.m_insertPoint.GetX(),
        m_insertData.m_insertPoint.GetY(), m_insertData.m_insertPoint.GetZ(),
        m_insertData.m_scalePoint.GetX(), m_insertData.m_scalePoint.GetY(),
        m_insertData.m_scalePoint.GetZ(), m_insertData.m_angle);
  }
}

bool Block::IsPickUp(double x, double y, double z) {
  Q_UNUSED(x)
  Q_UNUSED(y)
  Q_UNUSED(z)
  return false;
}

RectF Block::CalBoundingRect() {
  std::vector<double> vx;
  std::vector<double> vy;
  for (size_t i = 0; i < m_entities.size(); ++i) {
    RectF rect   = m_entities[i]->BoundingRect();
    PointF point = rect.topLeft();
    vx.push_back(point.x());
    vy.push_back(point.y());
    point = rect.bottomRight();
    vx.push_back(point.x());
    vy.push_back(point.y());
  }
  if (vx.size() > 0 && vy.size() > 0) {
    double minX = *std::min_element(vx.begin(), vx.end());
    double maxX = *std::max_element(vx.begin(), vx.end());
    double minY = *std::min_element(vy.begin(), vy.end());
    double maxY = *std::max_element(vy.begin(), vy.end());
    RectF rect(minX, minY, maxX - minX, maxY - minY);
    double exSize = Entity::GetBoundingRectExSize();
    rect.adjust(-exSize, -exSize, exSize, exSize);
    return rect;
  } else {
    return RectF(0, 0, 0, 0);
  }
}

void Block::CopyEntityTo(Block *block) {
  for (size_t k = 0; k < m_entities.size(); ++k) {
    block->push_back(m_entities[k]->Clone());
  }
  std::vector<Block *> &refBlocks = block->GetRefBlocks();
  for (size_t k = 0; k < m_refBlocks.size(); ++k) {
    refBlocks.push_back(m_refBlocks[k]->Clone());
  }
}

void Block::DeleteObjs() {
  DELETE_OBJS(m_entities);
  DELETE_OBJS(m_refBlocks);
}
} // namespace Entities
