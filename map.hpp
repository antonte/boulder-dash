#pragma once
#include <memory>
#include <shade/var.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

class Antman;
class Boulder;
class Diamond;
class Library;
class Mob;
class Obj;

class Map
{
public:
  Map(Library &, const char *map, int width);
  ~Map();
  char operator()(int x, int y) const;
  int getDiamondsCount() const;
  void draw(Var<glm::mat4> &mvp);
  void tick();

  template <typename T>
  T *get(int x, int y);
  template <typename T>
  void moveTo(T &entity, int x, int y);

  std::unique_ptr<Antman> antman;

private:
  int mapXy(int x, int y) const;

  Library *lib;
  std::string passiveMap;
  int width;
  int height;
  Obj *dirt;
  Obj *wall;
  std::unordered_map<int, std::unique_ptr<Boulder>> boulders;
  std::unordered_map<int, std::unique_ptr<Diamond>> diamonds;
  std::unordered_map<int, std::unique_ptr<Mob>> mobs;
  int diamondsCount = 0;
};

template <>
Antman *Map::get<Antman>(int x, int y);

template <>
Boulder *Map::get<Boulder>(int x, int y);

template <>
Diamond *Map::get<Diamond>(int x, int y);

template <>
void Map::moveTo<Antman>(Antman &entity, int x, int y);

template <>
void Map::moveTo<Boulder>(Boulder &entity, int x, int y);

template <>
void Map::moveTo<Diamond>(Diamond &entity, int x, int y);

template <>
void Map::moveTo<Mob>(Mob &entity, int x, int y);
