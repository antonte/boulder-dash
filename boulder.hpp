#pragma once
#include "map.hpp"
#include <shade/var.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

class Library;

class Boulder
{
public:
  enum { Symb = '@' };
  Boulder(Library &, int x, int y, Map &map);
  int getX() const;
  int getY() const;
  float getDispX() const;
  float getDispY() const;
  enum class Side { Left = 1, Nope = 0, Right = -1 };
  void push(Side);
  void tick();
  void draw(Var<glm::mat4> &mvp);

private:
  Obj* model;
  int x;
  int y;
  float dispX;
  float dispY;
  Map *map;
  Side pushSide = Side::Nope;
  int pushCount = 0;
  int fallCount = 0;
};
