/*
 * @file Dxf_CreationClass.cpp
 */

#include "dxf_creationclass.h"

#include "entities/arc.h"
#include "entities/block.h"
#include "entities/circle.h"
#include "entities/ellipse.h"
#include "entities/face3d.h"
#include "entities/line.h"
#include "entities/point.h"
#include "entities/polyline.h"
#include "entities/ray.h"
#include "entities/spline.h"
#include "entities/xline.h"

#include <iostream>
#include <qdebug.h>

Dxf_CreationClass::Dxf_CreationClass() {
}

Dxf_CreationClass::~Dxf_CreationClass() {
  DELETE_OBJS(m_blocks);
  DELETE_OBJS(m_layers);
}
//处理组码
void Dxf_CreationClass::processCodeValuePair(
    unsigned int code, const std::string &value) {
  Q_UNUSED(code)
  static bool bFindExtMinOrMax = false;
  if (value == "$EXTMIN" || value == "$EXTMAX") {
    bFindExtMinOrMax = true;
    return;
  }
  if (bFindExtMinOrMax) {
    if (m_extMinAndMax.size() < 6) {
      m_extMinAndMax.push_back(atof(value.c_str()));
    } else {
      bFindExtMinOrMax = false;
    }
  }
}

void Dxf_CreationClass::endSection() {
}
//图层数据
void Dxf_CreationClass::addLayer(const DL_LayerData &data) {
  Layer *layer = new Layer(data.name, data.flags, data.off);
  m_layers.push_back(layer);
}

//点数据
void Dxf_CreationClass::addPoint(const DL_PointData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Entity *pt = new Point(data.x, data.y, data.z);
    pt->SetAttributes(GetAttributes());
    Block *block = m_blocks[size - 1];
    if (!block->IsEndBlock()) {
      block->push_back(pt);
    } else {
      InsertInToPs0Block(pt);
    }
  }
}

//线数据
void Dxf_CreationClass::addLine(const DL_LineData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Point p1(data.x1, data.y1, data.z1);
    Point p2(data.x2, data.y2, data.z2);
    Entity *line = new Line(p1, p2);
    line->SetAttributes(GetAttributes());
    Block *block = m_blocks[size - 1];
    if (!block->IsEndBlock()) {
      block->push_back(line);
    } else {
      InsertInToPs0Block(line);
    }
  }
}

//圆弧数据
void Dxf_CreationClass::addArc(const DL_ArcData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Point center(data.cx, data.cy, data.cz);
    Entity *arc = new Arc(center, data.radius, data.angle1, data.angle2);
    arc->SetAttributes(GetAttributes());
    Block *block = m_blocks[size - 1];
    if (!block->IsEndBlock()) {
      block->push_back(arc);
    } else {
      InsertInToPs0Block(arc);
    }
  }
}

//圆数据
void Dxf_CreationClass::addCircle(const DL_CircleData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Point center(data.cx, data.cy, data.cz);
    Entity *circle = new Circle(center, data.radius);
    circle->SetAttributes(GetAttributes());
    Block *block = m_blocks[size - 1];
    if (!block->IsEndBlock()) {
      block->push_back(circle);
    } else {
      InsertInToPs0Block(circle);
    }
  }
}

//椭圆数据
void Dxf_CreationClass::addEllipse(const DL_EllipseData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Point center(data.cx, data.cy, data.cz);
    Point majorPos(data.mx + data.cx, data.my + data.cy, data.mz + data.cz);
    Entity *ellipse =
        new Ellipse(center, majorPos, data.ratio, data.angle1, data.angle2);
    ellipse->SetAttributes(GetAttributes());
    Block *block = m_blocks[size - 1];
    if (!block->IsEndBlock()) {
      block->push_back(ellipse);
    } else {
      InsertInToPs0Block(ellipse);
    }
  }
}

//折线段数据
void Dxf_CreationClass::addPolyline(const DL_PolylineData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Entity *polyline =
        new Polyline(data.number, data.m, data.n, data.flags, data.elevation);
    polyline->SetAttributes(GetAttributes());
    Block *block = m_blocks[size - 1];
    if (!block->IsEndBlock()) {
      block->push_back(polyline);
    } else {
      InsertInToPs0Block(polyline);
    }
  }
}

//顶点数据
void Dxf_CreationClass::addVertex(const DL_VertexData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Block *block = m_blocks[size - 1];
    if (block->IsEndBlock()) {
      block = FindBlockByName("*Paper_Space0");
      block->SetIsUse(true);
    }
    if (block == nullptr) return;
    if (!block->IsEmpty()) {
      Entity *elemEntity = block->ElementAt(block->GetElementSize() - 1);
      if (elemEntity->GetType() == EntityType::PolylineType) {
        Polyline *polyline = dynamic_cast<Polyline *>(elemEntity);
        Point vertex(data.x, data.y, data.z, data.bulge);
        vertex.SetAttributes(GetAttributes());
        polyline->AddVertex(vertex);
      }
    }
  }
}

// 3d面数据
void Dxf_CreationClass::add3dFace(const DL_3dFaceData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Point pts[4];
    for (int i = 0; i < 4; i++) {
      pts[i].SetX(data.x[i]);
      pts[i].SetY(data.y[i]);
      pts[i].SetZ(data.z[i]);
    }
    Entity *face3d = new Face3d(pts, data.thickness);
    face3d->SetAttributes(GetAttributes());
    Block *block = m_blocks[size - 1];
    if (!block->IsEndBlock()) {
      block->push_back(face3d);
    } else {
      InsertInToPs0Block(face3d);
    }
  }
}
//无限直线
void Dxf_CreationClass::addXLine(const DL_XLineData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Entity *xLine = new XLine(
        Point(data.bx, data.by, data.bz), Point(data.dx, data.dy, data.dz));
    xLine->SetAttributes(GetAttributes());
    Block *block = m_blocks[size - 1];
    if (!block->IsEndBlock()) {
      block->push_back(xLine);
    } else {
      InsertInToPs0Block(xLine);
    }
  }
}
//射线
void Dxf_CreationClass::addRay(const DL_RayData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Entity *ray = new Ray(
        Point(data.bx, data.by, data.bz), Point(data.dx, data.dy, data.dz));
    ray->SetAttributes(GetAttributes());
    Block *block = m_blocks[size - 1];
    if (!block->IsEndBlock()) {
      block->push_back(ray);
    } else {
      InsertInToPs0Block(ray);
    }
  }
}
//样条曲线
void Dxf_CreationClass::addSpline(const DL_SplineData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Point startPoint(
        data.tangentStartX, data.tangentStartY, data.tangentStartZ);
    Point endPoint(data.tangentEndX, data.tangentEndY, data.tangentEndZ);
    Entity *spline = new Spline(data.degree, data.nKnots, data.nControl,
        data.nFit, data.flags, startPoint, endPoint);
    spline->SetAttributes(GetAttributes());
    Block *block = m_blocks[size - 1];
    if (!block->IsEndBlock()) {
      block->push_back(spline);
    } else {
      InsertInToPs0Block(spline);
    }
  }
}
//控制点
void Dxf_CreationClass::addControlPoint(const DL_ControlPointData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Block *block = m_blocks[size - 1];
    if (block->IsEndBlock()) {
      block = FindBlockByName("*Paper_Space0");
      block->SetIsUse(true);
    }
    if (block == nullptr) return;
    if (!block->IsEmpty()) {
      Entity *elemEntity = block->ElementAt(block->GetElementSize() - 1);
      if (elemEntity->GetType() == EntityType::SplineType) {
        Spline *spline = dynamic_cast<Spline *>(elemEntity);
        Point controlPt(data.x, data.y, data.z, data.w);
        controlPt.SetAttributes(GetAttributes());
        spline->AddControlPts(dynamic_cast<Point *>(controlPt.Clone()));
      }
    }
  }
}
//拟合点
void Dxf_CreationClass::addFitPoint(const DL_FitPointData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Block *block = m_blocks[size - 1];
    if (block->IsEndBlock()) {
      block = FindBlockByName("*Paper_Space0");
      block->SetIsUse(true);
    }
    if (block == nullptr) return;
    if (!block->IsEmpty()) {
      Entity *elemEntity = block->ElementAt(block->GetElementSize() - 1);
      if (elemEntity->GetType() == EntityType::SplineType) {
        Spline *spline = dynamic_cast<Spline *>(elemEntity);
        Point fitPt(data.x, data.y, data.z);
        fitPt.SetAttributes(GetAttributes());
        spline->AddFitPts(dynamic_cast<Point *>(fitPt.Clone()));
      }
    }
  }
}
//节点矢量
void Dxf_CreationClass::addKnot(const DL_KnotData &data) {
  size_t size  = m_blocks.size();
  Block *block = m_blocks[size - 1];
  if (block->IsEndBlock()) {
    block = FindBlockByName("*Paper_Space0");
    block->SetIsUse(true);
  }
  if (block == nullptr) return;
  if (!block->IsEmpty()) {
    Entity *elemEntity = block->ElementAt(block->GetElementSize() - 1);
    if (elemEntity->GetType() == EntityType::SplineType) {
      Spline *spline = dynamic_cast<Spline *>(elemEntity);
      spline->AddKnots(data.k);
    }
  }
}

void Dxf_CreationClass::endEntity() {
}
void Dxf_CreationClass::addInsert(const DL_InsertData &data) {
  size_t size = m_blocks.size();
  if (size > 0) {
    Block *curBlock    = m_blocks[size - 1];
    Block *block       = FindBlockByName(data.name);
    Block *insertBlock = new Block();
    insertBlock->SetName(data.name);
    insertBlock->SetInsertPoint(Point(data.ipx, data.ipy, data.ipz));
    insertBlock->SetAngle(data.angle / 180.0 * PI);
    insertBlock->SetScalePoint(
        Point(fabs(data.sx), fabs(data.sy), fabs(data.sz)));
    insertBlock->SetCols(data.cols);
    insertBlock->SetRows(data.rows);
    insertBlock->SetColSp(data.colSp);
    insertBlock->SetRowSp(data.rowSp);
    insertBlock->SetAttributes(GetAttributes());

    if (block != nullptr) {
      if (!curBlock->IsEndBlock()) {
        insertBlock->SetIsUse(false);
        curBlock->GetRefBlocks().push_back(insertBlock);
      } else {
        block->CopyEntityTo(insertBlock);
        insertBlock->SetIsUse(true);
        //对块内的图元进行坐标变换
        std::vector<Block *> &refBlocks = insertBlock->GetRefBlocks();
        size_t refBlockSize             = refBlocks.size();
        Block *insertBlockEntity        = new Block();
        for (size_t m = 0; m < refBlockSize; ++m) {
          Block *refBlock    = refBlocks[m];
          Block *blockEntity = FindBlockByName(refBlock->GetName());

          if (blockEntity == nullptr) continue;

          insertBlockEntity->SetInsertData(refBlock->GetInsertData());
          insertBlockEntity->DeleteObjs();
          blockEntity->CopyEntityTo(insertBlockEntity);
          insertBlockEntity->CorrectCoord();

          size_t elemSize = insertBlockEntity->GetElementSize();
          for (size_t n = 0; n < elemSize; ++n) {
            insertBlock->push_back(insertBlockEntity->ElementAt(n)->Clone());
          }
        }
        insertBlock->CorrectCoord();
        insertBlock->SetIsEndBlock(true);
        m_blocks.push_back(insertBlock);
        delete insertBlockEntity;
      }
    } else {
      insertBlock->SetIsUse(false);
      curBlock->GetRefBlocks().push_back(insertBlock);
    }
  }
}
void Dxf_CreationClass::addBlock(const DL_BlockData &data) {
  Block *block = new Block();
  block->SetName(data.name);
  block->SetInsertPoint(Point(data.bpx, data.bpy, data.bpz));
  block->SetAttributes(GetAttributes());
  m_blocks.push_back(block);
}

void Dxf_CreationClass::endBlock() {
  size_t size = m_blocks.size();
  if (size > 0) {
    Block *block = m_blocks[size - 1];
    block->SetIsEndBlock(true);
  }
}

void Dxf_CreationClass::InsertInToPs0Block(Entity *entity) {
  Block *block = FindBlockByName("*Paper_Space0");
  if (block == nullptr) {
    block = new Block();
    block->SetName("*Paper_Space0");
    m_blocks.push_back(block);
  }
  block->push_back(entity);
  block->SetIsUse(true);
}

Block *Dxf_CreationClass::FindBlockByName(const std::string &name) {
  Block *block = nullptr;
  for (size_t i = 0; i < m_blocks.size(); ++i) {
    if (m_blocks[i]->GetName() == name) {
      block = m_blocks[i];
      break;
    }
  }
  return block;
}
Entities::Attributes &Dxf_CreationClass::GetAttributes() {
  m_attributes.layer_name =
      attributes.getLayer(); // QString::fromStdString(attributes.getLayer());
  if (attributes.getColor() == 256) {
    m_attributes.layer_block_color_flag = 0;
    // printf("BYLAYER");
  } else if (attributes.getColor() == 0) {
    m_attributes.layer_block_color_flag = 1;
    // printf("BYBLOCK");
  } else {
    // printf("%d", attributes.getColor());
    m_attributes.layer_block_color_flag = 2;
    m_attributes.color                  = attributes.getColor();
  }

  if (attributes.getWidth() == -1) {
    m_attributes.layer_block_color_width = 0;
    // printf("BYLAYER");
  } else if (attributes.getWidth() == -2) {
    // printf("BYBLOCK");
    m_attributes.layer_block_color_width = 1;
  } else if (attributes.getWidth() == -3) {
    // printf("DEFAULT");
    m_attributes.layer_block_color_width = 2;
  } else {
    m_attributes.layer_block_color_width = 3;
    // printf("%d", attributes.getWidth());
    m_attributes.line_width = attributes.getWidth();
  }
  // printf(" Type: %s\n", attributes.getLinetype().c_str());
  m_attributes.line_type =
      attributes
          .getLinetype(); // QString::fromStdString(attributes.getLinetype());
  return m_attributes;
}

CoordXY Dxf_CreationClass::GetCoordRange() {
  if (m_extMinAndMax.size() == 6) {
    Point leftTop(m_extMinAndMax[0], m_extMinAndMax[1], m_extMinAndMax[2]);
    Point rightBottom(m_extMinAndMax[3], m_extMinAndMax[4], m_extMinAndMax[5]);
    return CoordXY(leftTop, rightBottom);
  } else {
    return CoordXY();
  }
}

std::vector<Block *> &Dxf_CreationClass::GetBlocks() {
  Block *blockEntity = new Block();
  for (size_t i = 0; i < m_blocks.size(); ++i) {
    Block *block = m_blocks[i];
    if (block->IsEndBlock()) continue;
    std::vector<Block *> &refBlocks = block->GetRefBlocks();
    size_t k                        = 0;
    for (; k < refBlocks.size(); ++k) {
      Block *refBlock = refBlocks[k];
      if (!refBlock->IsUse()) {
        Block *findedBlock = FindBlockByName(refBlock->GetName());
        if (findedBlock != nullptr) {
          blockEntity->SetInsertData(refBlock->GetInsertData());
          blockEntity->DeleteObjs();
          findedBlock->CopyEntityTo(blockEntity);
          blockEntity->SetAttributes(refBlock->GetAttributes());
          blockEntity->CorrectCoord();

          blockEntity->SetInsertData(block->GetInsertData());
          blockEntity->CorrectCoord();

          size_t elemSize = blockEntity->GetElementSize();
          for (size_t n = 0; n < elemSize; ++n) {
            block->push_back(blockEntity->ElementAt(n)->Clone());
          }
        }
      }
    }
    if (refBlocks.size() > 0 && k > 0) block->SetIsEndBlock(true);
  }
  delete blockEntity;
  return m_blocks;
}

// EOF
