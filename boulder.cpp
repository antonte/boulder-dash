#include "boulder.hpp"
#include "antman.hpp"
#include "fall_rate.hpp"
#include "mob.hpp"
#include <shade/library.hpp>
#include <shade/obj.hpp>

Boulder::Boulder(Library &lib, int x, int y, Map &map)
  : Entity(x, y, map), model(lib.getObj("boulder"))
{
}

void Boulder::push(Side side)
{
  pushSide = side;
  if (side == Side::Nope)
    pushCount = 0;
  else
    ++pushCount;
}

float Boulder::moveK() const
{
  return 1.0f / fallRate();
}

void Boulder::tick()
{
  Entity::tick();
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
    if (isFalling && (under == Antman::Symb || under == Mob::Symb))
    {
      map->kill(x, newY);
      return;
    }
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
    isFalling = (under == ' ');
  }
}

void Boulder::draw(Var<glm::mat4> &mvp)
{
  mvp = glm::translate(glm::vec3(getDispX() * 2, 0.0f, getDispY() * 2));
  mvp.update();
  model->draw();
}
