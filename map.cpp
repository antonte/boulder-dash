#include "map.hpp"
#include "antman.hpp"
#include "boulder.hpp"
#include <shade/library.hpp>
#include <shade/obj.hpp>

Map::Map(Library &aLib, const char *aMap, int aWidth)
  : antman(std::make_unique<Antman>(aLib, *this)),
    lib(&aLib),
    passiveMap(aMap),
    width(aWidth),
    height(passiveMap.size() / aWidth),
    diamond(lib->getObj("diamond")),
    dirt(lib->getObj("dirt")),
    wall(lib->getObj("wall"))
{
  passiveMap[antman->getX() + antman->getY() * width] = Antman::Symb;
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x)
    {
      auto ch = passiveMap[x + y * width];
      switch (ch)
      {
      case Boulder::Symb:
        boulders[mapXy(x, y)] = std::make_unique<Boulder>(*lib, x, y, *this);
        break;
      }
    }
}

Map::~Map() {}

char &Map::operator()(int x, int y)
{
  return passiveMap[x + y * width];
}

void Map::draw(Var<glm::mat4> &mvp)
{
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x)
    {
      switch (operator()(x, y))
      {
      case 'W':
        mvp = glm::translate(glm::vec3(x * 2, 0.0f, y * 2));
        mvp.update();
        wall->activate();
        break;
      // case '@':
      //   mvp = glm::translate(glm::vec3(x * 2, 0.0f, y * 2));
      //   mvp.update();
      //   boulder.activate();
      //   break;
      case '=':
        mvp = glm::translate(glm::vec3(x * 2, 0.0f, y * 2));
        mvp.update();
        dirt->activate();
        break;
      case '#':
        mvp = glm::translate(glm::vec3(x * 2, 0.0f, y * 2)) *
              glm::rotate(SDL_GetTicks() / 1000.0f, glm::vec3(0, 0, 1));
        mvp.update();
        diamond->activate();
        break;
      }
    }

  antman->draw(mvp);
  for (auto &&bould : boulders)
    bould.second->draw(mvp);
}
void Map::tick()
{
  antman->tick();

  std::vector<Boulder *> bouldCopy;
  for (auto &&bould : boulders)
    bouldCopy.push_back(bould.second.get());

  for (auto &&bould : bouldCopy)
    bould->tick();
}

template <>
Antman *Map::get<Antman>(int x, int y)
{
  if (antman->getX() != x || antman->getY() != y)
    return nullptr;
  return antman.get();
}

template <>
Boulder *Map::get<Boulder>(int x, int y)
{
  auto it = boulders.find(mapXy(x, y));
  if (it == std::end(boulders))
    return nullptr;
  return it->second.get();
}

int Map::mapXy(int x, int y) const
{
  return x + y * width;
}

template <>
void Map::moveTo<Boulder>(Boulder &unit, int x, int y)
{
  auto curXy = mapXy(unit.getX(), unit.getY());
  auto newXy = mapXy(x, y);
  passiveMap[curXy] = ' ';
  passiveMap[newXy] = Boulder::Symb;

  auto it = boulders.find(curXy);
  assert(it != std::end(boulders));
  assert(&unit == it->second.get());
  boulders[newXy] = std::move(it->second);
  boulders.erase(curXy);
}
