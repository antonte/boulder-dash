#pragma once
#include <memory>
#include <shade/var.hpp>
#include <string>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

class Antman;
class Boulder;
class Obj;
class Library;

class Map
{
public:
  Map(Library &, const char *map, int width);
  ~Map();
  char &operator()(int x, int y);
  void draw(Var<glm::mat4> &mvp);
  void tick();

  template <typename T>
  T *get(int x, int y);
  template <typename T>
  void moveTo(T &unit, int x, int y)
  {
    passiveMap[mapXy(unit.getX(), unit.getY())] = ' ';
    passiveMap[mapXy(x, y)] = T::Symb;
  }

  std::unique_ptr<Antman> antman;

private:
  int mapXy(int x, int y) const;

  Library *lib;
  std::string passiveMap;
  int width;
  int height;
  Obj *diamond;
  Obj *dirt;
  Obj *wall;
  std::unordered_map<int, std::unique_ptr<Boulder>> boulders;
};

template <>
Antman *Map::get<Antman>(int x, int y);

template <>
Boulder *Map::get<Boulder>(int x, int y);

template <>
void Map::moveTo<Boulder>(Boulder &unit, int x, int y);
