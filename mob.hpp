#pragma once
#include "map.hpp"
#include <shade/obj.hpp>
#include <shade/var.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

class Library;

class Mob
{
public:
  enum { Symb = 'X' };
  Mob(Library &, int x, int y, Map &map);
  void tick();
  void draw(Var<glm::mat4> &mvp);
  int getX() const;
  int getY() const;

private:
  int x;
  int y;
  float dispX;
  float dispY;
  Map *map;
  std::vector<Obj *> model;
  int ticks = 0;
  int dir = 0;
};
