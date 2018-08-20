#pragma once
#include "map.hpp"
#include <shade/var.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

class Library;

class Diamond
{
public:
  enum { Symb = '#' };
  Diamond(Library &, int x, int y, Map &map);
  int getX() const;
  int getY() const;
  float getDispX() const;
  float getDispY() const;
  void tick();
  void draw(Var<glm::mat4> &mvp);

private:
  Obj* model;
  int x;
  int y;
  float dispX;
  float dispY;
  Map *map;
  int fallCount = 0;
};
