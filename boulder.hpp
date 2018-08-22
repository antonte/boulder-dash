#pragma once
#include "map.hpp"
#include <shade/obj.hpp>
#include "entity.hpp"

class Library;

class Boulder: public Entity
{
public:
  enum { Symb = '@' };
  Boulder(Library &, int x, int y, Map &map);
  enum class Side { Left = 1, Nope = 0, Right = -1 };
  void push(Side);
  void tick() override;
  void draw(Var<glm::mat4> &mvp) override;

private:
  float moveK() const override;

  Obj *model;
  Side pushSide = Side::Nope;
  int pushCount = 0;
  int fallCount = 0;
};
