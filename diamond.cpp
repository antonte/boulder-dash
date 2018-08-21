#include "diamond.hpp"
#include "fall_rate.hpp"
#include <shade/library.hpp>
#include <shade/obj.hpp>

Diamond::Diamond(Library &lib, int aX, int aY, Map &aMap)
  : model(lib.getObj("diamond")), x(aX), y(aY), dispX(aX), dispY(aY), map(&aMap)
{
}

int Diamond::getX() const
{
  return x;
}

int Diamond::getY() const
{
  return y;
}

float Diamond::getDispX() const
{
  return dispX;
}

float Diamond::getDispY() const
{
  return dispY;
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

void Diamond::tick()
{
  dispX += walkK() * sign(x - dispX);
  dispY += walkK() * sign(y - dispY);
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

void Diamond::draw(Var<glm::mat4> &mvp)
{
  mvp = glm::translate(glm::vec3(dispX * 2, 0.0f, dispY * 2)) *
        glm::rotate(SDL_GetTicks() / 1000.0f, glm::vec3(0, 0, 1));
  mvp.update();
  model->draw();
}
