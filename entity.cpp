#include "entity.hpp"

Entity::Entity(int aX, int aY, Map &aMap) : map(&aMap), x(aX), y(aY), dispX(x), dispY(y) {}

int Entity::getX() const
{
  return x;
}

int Entity::getY() const
{
  return y;
}

float Entity::getDispX() const
{
  return dispX;
}

float Entity::getDispY() const
{
  return dispY;
}

float Entity::sign(float x) const
{
  if (x > moveK())
    return 1;
  if (x < -moveK())
    return -1;
  return x / moveK();
}

void Entity::tick()
{
  dispX += moveK() * sign(x - dispX);
  dispY += moveK() * sign(y - dispY);
}
