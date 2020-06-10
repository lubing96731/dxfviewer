#include "esVCAD.h"

#include "dxf_parser/dxf_creationclass.h"
#include "entities/rect.h"

#include <dl_dxf.h>

double EsvCad::m_boundingRectExSize = 0.25;

void EsvCad::SetBoundingRectExSize(double exSize) {
  m_boundingRectExSize = exSize;
}

double EsvCad::GetBoundingRectExSize() {
  return m_boundingRectExSize;
}

EsvCad::EsvCad() {
}

EsvCad::~EsvCad() {
  DeleteEntities();
}
void EsvCad::DeleteEntities() {
  DELETE_OBJS(m_blocks);
  DELETE_OBJS(m_layers);
}

void EsvCad::ParseDxf(
    const std::string &fileName, int viewWidth, int viewHeight) {
  Entity::SetBoundingRectExSize(m_boundingRectExSize);
  DL_Dxf dxf;
  Dxf_CreationClass dxf_creationClass;
  // QTime time0;
  // time0.start();
  if (!dxf.in(fileName, &dxf_creationClass)) { // if file open failed
    // std::cerr << file << " could not be opened.\n";
    return;
  } else {
    // qDebug()<<"parse cost time:"<<time0.elapsed()/1000.0;
    DeleteEntities();
    std::vector<Block *> &blocks = dxf_creationClass.GetBlocks();
    std::vector<Layer *> &layers = dxf_creationClass.GetLayers();
    CoordXY coordXY = m_coordXY = dxf_creationClass.GetCoordRange();
    //进行坐标转换
    int width     = viewWidth;
    int height    = viewHeight;
    double xScale = coordXY.GetWidth() / width;
    double yScale = coordXY.GetHeihgt() / height;
    double scale  = std::max(xScale, yScale);
    Point leftTop(coordXY.GetLeftTop());
    double params[5] = {
        scale, (double)height, leftTop.GetX(), leftTop.GetY(), (double)width};

    //构造一个block,把坐标系放到里面，接口可以统一
    Block *coordBlock = new Block;
    coordBlock->SetName("CoordXY");
    coordBlock->SetIsUse(true);

    std::string layerName = "coord_layer";
    Layer *coordLayer     = new Layer(layerName, 0, false);
    coordXY.SetLayer(coordLayer);
    m_layers.push_back(coordLayer);

    Attributes attr;
    attr            = coordXY.GetAttributes();
    attr.layer_name = layerName;
    coordXY.SetAttributes(attr);

    coordBlock->push_back(coordXY.Clone());
    coordBlock->Transform(m_layers, params, 5);
    m_blocks.push_back(coordBlock);
    //构造一个block,把坐标系放到里面，接口可以统一
    //图元变换到窗口坐标系
    // QTime time1;
    // time1.start();
    for (size_t i = 0; i < layers.size(); ++i) {
      m_layers.push_back(layers[i]->Clone());
    }
    for (size_t i = 0; i < blocks.size(); ++i) {
      if (!blocks[i]->IsEmpty() && blocks[i]->IsUse()) {
        Block *block = blocks[i]->Clone();
        block->Transform(m_layers, params, 5);
        m_blocks.push_back(block);
      }
    }
    // qDebug()<<"clone cost time:"<<time1.elapsed()/1000.0;
  }
}
