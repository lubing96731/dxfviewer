﻿#ifndef DXF_CREATIONCLASS_H
#define DXF_CREATIONCLASS_H

#include "entities/block.h"

#include <dl_creationadapter.h>
#include <entities/coordxy.h>
#include <entities/layer.h>
using namespace Entities;

class Dxf_CreationClass : public DL_CreationAdapter {
 public:
  Dxf_CreationClass();
  ~Dxf_CreationClass();

 public:
  virtual void processCodeValuePair(
      unsigned int code, const std::string &value);
  virtual void endSection();
  virtual void addLayer(const DL_LayerData &data);
  virtual void addPoint(const DL_PointData &data);
  virtual void addLine(const DL_LineData &data);
  virtual void addArc(const DL_ArcData &data);
  virtual void addCircle(const DL_CircleData &data);
  virtual void addEllipse(const DL_EllipseData &data);
  virtual void addPolyline(const DL_PolylineData &data);
  virtual void addVertex(const DL_VertexData &data);
  virtual void add3dFace(const DL_3dFaceData &data);
  virtual void addXLine(const DL_XLineData &data);
  virtual void addRay(const DL_RayData &data);
  virtual void addSpline(const DL_SplineData &data);
  virtual void addControlPoint(const DL_ControlPointData &data);
  virtual void addFitPoint(const DL_FitPointData &data);
  virtual void addKnot(const DL_KnotData &data);
  virtual void endEntity();

  virtual void addInsert(const DL_InsertData &data);
  virtual void addBlock(const DL_BlockData &data);
  virtual void endBlock();

 private:
  std::vector<Block *> m_blocks;
  std::vector<Layer *> m_layers;
  Entities::Attributes m_attributes;
  std::vector<double> m_extMinAndMax;

 public:
  std::vector<Block *> &GetBlocks();
  std::vector<Layer *> &GetLayers() { return m_layers; }
  CoordXY GetCoordRange(); //得到dxf的坐标范围
 private:
  Attributes &GetAttributes();
  //插入到Paper_Space0块
  void InsertInToPs0Block(Entity *entity);
  Block *FindBlockByName(const std::string &name);
};

#endif
