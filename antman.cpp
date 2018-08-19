#include "antman.hpp"

#include "boulder.hpp"
#include <log/log.hpp>
#include <sdlpp/sdlpp.hpp>
#include <shade/library.hpp>
#include <shade/obj.hpp>

Antman::Antman(Library &lib, Map &aMap) : map(&aMap)
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
  mvp = glm::translate(glm::vec3(dispX * 2.0f, 0.0f, dispY * 2.0f));
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

    auto &ch = (*map)(newX, newY);
    if (ch != '@')
      walkAnim[SDL_GetTicks() * 25 / 1000 % walkAnim.size()]->activate();
    else
      pushAnim[SDL_GetTicks() * 25 / 1000 % pushAnim.size()]->activate();
  }
  else
  {
    mvp.update();
    standAnim[SDL_GetTicks() * 25 / 1000 % standAnim.size()]->activate();
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

static float WalkK = 1.0f / 30.0f;

static int sign(float x)
{
  if (x > WalkK)
    return 1;
  if (x < -WalkK)
    return -1;
  return 0;
}

void Antman::tick()
{
  dispX += WalkK * sign(x - dispX);
  dispY += WalkK * sign(y - dispY);
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
  coolDown = 30;
  if (ticks++ % 30 != 0)
    return;

  int dxs[] = {1, 0, -1, 0};
  int dys[] = {0, 1, 0, -1};
  auto dx = dxs[dir];
  auto dy = dys[dir];

  auto newX = x + dx;
  auto newY = y + dy;

  auto &ch = (*map)(newX, newY);
  Boulder *bldr = nullptr;
  switch (ch)
  {
  case 'W': break;
  case '@':
    if (newY != y)
      break;
    {
      bldr = map->get<Boulder>(newX, newY);
      assert(bldr);
      bldr->push(newX - x > 0 ? Boulder::Side::Left : Boulder::Side::Right);
    }
    break;
  case '=':
    ch = ' ';
    map->moveTo(*this, newX, newY);
    x = newX;
    y = newY;
    break;
  case '#':
    ch = ' ';
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

int Antman::getX() const
{
  return x;
}

int Antman::getY() const
{
  return y;
}

float Antman::getDispX() const
{
  return dispX;
}

float Antman::getDispY() const
{
  return dispY;
}
