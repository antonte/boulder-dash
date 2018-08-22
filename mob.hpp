#pragma once
#include "entity.hpp"
#include <shade/obj.hpp>

class Library;

class Mob: public Entity
{
public:
  enum { Symb = 'X' };
  Mob(Library &, int x, int y, Map &map);
  void tick() override;
  void draw(Var<glm::mat4> &mvp) override;

private:
  float moveK() const override;

  std::vector<Obj *> model;
  int ticks = 0;
  int dir = 0;
};
