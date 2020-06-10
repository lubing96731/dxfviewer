#include "entity.h"
#include <QUuid>

namespace Entities {
double Entity::m_BoundingRectExSize = 0.25;
void Entity::SetBoundingRectExSize(double exSize) {
  m_BoundingRectExSize = exSize;
}

double Entity::GetBoundingRectExSize() {
  return m_BoundingRectExSize;
}

std::string Entity::CreateUuid() {
  QUuid id = QUuid::createUuid();
  return id.toString().toStdString();
}

} // namespace Entities
