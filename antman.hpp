#pragma once
#include "map.hpp"
#include <shade/var.hpp>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

class Library;
class Obj;
class Antman
{
public:
  enum { Symb = 'A' };
  Antman(Library &, Map &map);
  void draw(Var<glm::mat4> &mvp);
  void run(int dir);
  void tick();
  void stop(int dir);
  int getX() const;
  int getY() const;
  float getDispX() const;
  float getDispY() const;

private:
  std::vector<Obj *> standAnim;
  std::vector<Obj *> walkAnim;
  std::vector<Obj *> pushAnim;
  int oldDir = 0;
  int dir = -1;
  int x = 1;
  int y = 1;
  float dispX = 1.0f;
  float dispY = 1.0f;
  Map *map;
  int ticks = 0;
  int coolDown = 0;
  Boulder *pushingBldr = nullptr;
};
