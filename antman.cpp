#include "antman.hpp"

#include "boulder.hpp"
#include "diamond.hpp"
#include <log/log.hpp>
#include <sdlpp/sdlpp.hpp>
#include <shade/library.hpp>
#include <shade/obj.hpp>
#include <coeff/coefficient_registry.hpp>

Antman::Antman(Library &lib, Map &map) : Entity(1, 1, map)
{
  for (int i = 1; i <= 9; ++i)
    standAnim.push_back(lib.getObj("antman_stand_" + std::to_string(i), "antman"));
  for (int i = 1; i <= 12; ++i)
    walkAnim.push_back(lib.getObj("antman_walk_" + std::to_string(i), "antman"));
  for (int i = 1; i <= 24; ++i)
    pushAnim.push_back(lib.getObj("antman_push_" + std::to_string(i), "antman"));
}

void Antman::draw(Var<glm::mat4> &mvp)
{
  mvp = glm::translate(glm::vec3(getDispX() * 2.0f, 0.0f, getDispY() * 2.0f));
  if (coolDown > 0)
  {
    switch (oldDir)
    {
    case 0: break;
    case 1: mvp = mvp.get() * glm::rotate(3.1415926f / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f)); break;
    case 2: mvp = mvp.get() * glm::rotate(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f)); break;
    case 3: mvp = mvp.get() * glm::rotate(3.1415926f / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f)); break;
    }
    mvp.update();
    int dxs[] = {1, 0, -1, 0};
    int dys[] = {0, 1, 0, -1};
    auto dx = dxs[oldDir];
    auto dy = dys[oldDir];
    auto newX = x + dx;
    auto newY = y + dy;

    auto ch = (*map)(newX, newY);
    if (ch != Boulder::Symb)
      walkAnim[SDL_GetTicks() * 25 / 1000 % walkAnim.size()]->draw();
    else
      pushAnim[SDL_GetTicks() * 25 / 1000 % pushAnim.size()]->draw();
  }
  else
  {
    mvp.update();
    standAnim[SDL_GetTicks() * 25 / 1000 % standAnim.size()]->draw();
  }
}

void Antman::run(int value)
{
  if (dir == value)
    return;
  dir = value;
  oldDir = value;
  if (coolDown == 0)
    ticks = 0;
}

void Antman::stop(int value)
{
  if (value == dir)
    dir = -1;
}

COEFF(WalkCoolDown, 30)

float Antman::moveK() const
{
  return 1.0f / WalkCoolDown;
}

void Antman::tick()
{
  Entity::tick();
  if (dir < 0)
  {
    if (pushingBldr)
    {
      pushingBldr->push(Boulder::Side::Nope);
      pushingBldr = nullptr;
    }
    if (coolDown > 0)
      --coolDown;
    return;
  }
  coolDown = WalkCoolDown;
  if (ticks++ % WalkCoolDown != 0)
    return;

  int dxs[] = {1, 0, -1, 0};
  int dys[] = {0, 1, 0, -1};
  auto dx = dxs[dir];
  auto dy = dys[dir];

  auto newX = x + dx;
  auto newY = y + dy;

  auto ch = (*map)(newX, newY);
  Boulder *bldr = nullptr;
  switch (ch)
  {
  case 'W': break;
  case Boulder::Symb:
    if (newY != y)
      break;
    {
      bldr = map->get<Boulder>(newX, newY);
      assert(bldr);
      bldr->push(newX - x > 0 ? Boulder::Side::Left : Boulder::Side::Right);
    }
    break;
  case '=':
    map->moveTo(*this, newX, newY);
    x = newX;
    y = newY;
    break;
  case Diamond::Symb:
    map->moveTo(*this, newX, newY);
    x = newX;
    y = newY;
    break;
  case ' ':
    map->moveTo(*this, newX, newY);
    x = newX;
    y = newY;
    break;
  }
  if (bldr)
    pushingBldr = bldr;
  else if (pushingBldr)
  {
    pushingBldr->push(Boulder::Side::Nope);
    pushingBldr = nullptr;
  }
}
