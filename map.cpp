#include "map.hpp"
#include "antman.hpp"
#include "boulder.hpp"
#include "diamond.hpp"
#include <shade/library.hpp>
#include <shade/obj.hpp>

Map::Map(Library &aLib, const char *aMap, int aWidth)
  : antman(std::make_unique<Antman>(aLib, *this)),
    lib(&aLib),
    passiveMap(aMap),
    width(aWidth),
    height(passiveMap.size() / aWidth),
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
      case Diamond::Symb:
        diamonds[mapXy(x, y)] = std::make_unique<Diamond>(*lib, x, y, *this);
        break;
      }
    }
}

Map::~Map() {}

char Map::operator()(int x, int y) const
{
  return passiveMap[x + y * width];
}

int Map::getDiamondsCount() const
{
  return diamondsCount;
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
        wall->draw();
        break;
      case '=':
        mvp = glm::translate(glm::vec3(x * 2, 0.0f, y * 2));
        mvp.update();
        dirt->draw();
        break;
      }
    }

  antman->draw(mvp);
  for (auto &&entity : boulders)
    entity.second->draw(mvp);
  for (auto &&entity : diamonds)
    entity.second->draw(mvp);
}
void Map::tick()
{
  antman->tick();

  std::vector<Boulder *> bouldCopy;
  for (auto &&bould : boulders)
    bouldCopy.push_back(bould.second.get());

  for (auto &&entity : bouldCopy)
    entity->tick();

  std::vector<Diamond *> diamondCopy;
  for (auto &&entity : diamonds)
    diamondCopy.push_back(entity.second.get());

  for (auto &&entity : diamondCopy)
    entity->tick();
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

template <>
Diamond *Map::get<Diamond>(int x, int y)
{
  auto it = diamonds.find(mapXy(x, y));
  if (it == std::end(diamonds))
    return nullptr;
  return it->second.get();
}

int Map::mapXy(int x, int y) const
{
  return x + y * width;
}


template <>
void Map::moveTo<Antman>(Antman &entity, int x, int y)
{
  auto curXy = mapXy(entity.getX(), entity.getY());
  auto newXy = mapXy(x, y);
  passiveMap[curXy] = ' ';
  passiveMap[newXy] = Antman::Symb;

  auto it = diamonds.find(newXy);
  if (it == std::end(diamonds))
    return;
  diamonds.erase(it);
  ++diamondsCount;
}

template <>
void Map::moveTo<Boulder>(Boulder &entity, int x, int y)
{
  auto curXy = mapXy(entity.getX(), entity.getY());
  auto newXy = mapXy(x, y);
  passiveMap[curXy] = ' ';
  passiveMap[newXy] = Boulder::Symb;

  auto it = boulders.find(curXy);
  assert(it != std::end(boulders));
  assert(&entity == it->second.get());
  boulders[newXy] = std::move(it->second);
  boulders.erase(curXy);
}

template <>
void Map::moveTo<Diamond>(Diamond &entity, int x, int y)
{
  auto curXy = mapXy(entity.getX(), entity.getY());
  auto newXy = mapXy(x, y);
  passiveMap[curXy] = ' ';
  passiveMap[newXy] = Diamond::Symb;

  auto it = diamonds.find(curXy);
  assert(it != std::end(diamonds));
  assert(&entity == it->second.get());
  diamonds[newXy] = std::move(it->second);
  diamonds.erase(curXy);
}
