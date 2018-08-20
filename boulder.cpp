#include "boulder.hpp"
#include "fall_rate.hpp"
#include <shade/library.hpp>
#include <shade/obj.hpp>

Boulder::Boulder(Library &lib, int aX, int aY, Map &aMap)
  : model(lib.getObj("boulder")), x(aX), y(aY), dispX(aX), dispY(aY), map(&aMap)
{
}

int Boulder::getX() const
{
  return x;
}

int Boulder::getY() const
{
  return y;
}

float Boulder::getDispX() const
{
  return dispX;
}

float Boulder::getDispY() const
{
  return dispY;
}

void Boulder::push(Side side)
{
  pushSide = side;
  if (side == Side::Nope)
    pushCount = 0;
  else
    ++pushCount;
}

static float walkK()
{
  return 1.0f / fallRate();
}

static float sign(float x)
{
  if (x > walkK())
    return 1;
  if (x < -walkK())
    return -1;
  return x / walkK();
}

void Boulder::tick()
{
  dispX += walkK() * sign(x - dispX);
  dispY += walkK() * sign(y - dispY);
  if (pushCount % 4 == 0 && pushSide != Side::Nope)
  {
    ++pushCount;
    auto newX = x + static_cast<int>(pushSide);
    if ((*map)(newX, y) == ' ')
    {
      map->moveTo(*this, newX, y);
      x = newX;
    }
  }
  {
    auto newY = y + 1;
    auto under = (*map)(x, newY);
    if (under == ' ')
    {
      fallCount++;
      if (fallCount % fallRate() == 0)
      {
        map->moveTo(*this, x, newY);
        y = newY;
      }
    }
    else if (under != '=' && (*map)(x - 1, y) == ' ' && (*map)(x - 1, y + 1) == ' ')
    {
      fallCount++;
      if (fallCount % fallRate() == 0)
      {
        map->moveTo(*this, x - 1, y);
        x = x - 1;
      }
    }
    else if (under != '=' && (*map)(x + 1, y) == ' ' && (*map)(x + 1, y + 1) == ' ')
    {
      fallCount++;
      if (fallCount % fallRate() == 0)
      {
        map->moveTo(*this, x + 1, y);
        x = x + 1;
      }
    }
    else
      fallCount = 0;
  }
}

void Boulder::draw(Var<glm::mat4> &mvp)
{
  mvp = glm::translate(glm::vec3(dispX * 2, 0.0f, dispY * 2));
  mvp.update();
  model->draw();
}
