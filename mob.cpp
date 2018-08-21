#include "mob.hpp"
#include <coeff/coefficient_registry.hpp>

Mob::Mob(Library &lib, int aX, int aY, Map &aMap) : x(aX), y(aY), dispX(aX), dispY(aY), map(&aMap)
{
  for (int i = 1; i <= 29; ++i)
    model.push_back(lib.getObj("mob_" + std::to_string(i), "mob"));
}

COEFF(CoolDown, 20);

static float moveK()
{
  return 1.0f / CoolDown;
}

int Mob::getX() const
{
  return x;
}

int Mob::getY() const
{
  return y;
}

static float sign(float x)
{
  if (x > moveK())
    return 1;
  if (x < -moveK())
    return -1;
  return x / moveK();
}

void Mob::tick()
{
  dispX += moveK() * sign(x - dispX);
  dispY += moveK() * sign(y - dispY);

  if (ticks++ % CoolDown == 0)
  {
    int dxs[] = {1, 0, -1, 0};
    int dys[] = {0, 1, 0, -1};
    {
      int dir1 = (dir + 1) % 4;
      auto x1 = x + dxs[dir1];
      auto y1 = y + dys[dir1];
      int dir2 = (dir1 + 1) % 4;
      auto x2 = x1 + dxs[dir2];
      auto y2 = y1 + dys[dir2];
      if ((*map)(x1, y1) == ' ' && (*map)(x2, y2) != ' ')
      {
        dir = dir1;
        map->moveTo(*this, x1, y1);
        x = x1;
        y = y1;
        return;
      }
    }
    for (int i = 0; i < 3; ++i)
    {
      auto dx = dxs[dir];
      auto dy = dys[dir];
      auto newX = x + dx;
      auto newY = y + dy;
      if ((*map)(newX, newY) == ' ')
      {
        map->moveTo(*this, newX, newY);
        x = newX;
        y = newY;
        return;
      }
      dir = (dir + 3) % 4;
      return;
    }
  }
}

void Mob::draw(Var<glm::mat4> &mvp)
{
  mvp = glm::translate(glm::vec3(dispX * 2, 0.0f, dispY * 2));
  mvp.update();
  model[SDL_GetTicks() * 30 / 1000 % model.size()]->draw();
}
