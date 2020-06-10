#ifndef ESVCAD_H
#define ESVCAD_H

#include "entities/block.h"
#include "entities/coordxy.h"
#include "entities/layer.h"

#include <string>
#include <vector>
using namespace Entities;

class EsvCad {
 public:
  EsvCad();
  ~EsvCad();

 public:
  void ParseDxf(const std::string &fileName, int viewWidth, int viewHeight);
  std::vector<Block *> &GetBlocks() { return m_blocks; }
  std::vector<Layer *> &GetLayers() { return m_layers; }
  CoordXY &GetCoordXY() { return m_coordXY; }

 public:
  static void SetBoundingRectExSize(double exSize);
  static double GetBoundingRectExSize();

 private:
  std::vector<Block *> m_blocks;
  std::vector<Layer *> m_layers;
  CoordXY m_coordXY;
  static double m_boundingRectExSize;

 private:
  void DeleteEntities();
};

#endif // ESVCAD_H
