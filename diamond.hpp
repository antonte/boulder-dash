#pragma once
#include "entity.hpp"
#include <shade/var.hpp>

class Library;
class Obj;

class Diamond : public Entity
{
public:
  enum { Symb = '#' };
  Diamond(Library &, int x, int y, Map &map);
  void tick() override;
  void draw(Var<glm::mat4> &mvp) override;

private:
  float moveK() const override;

  Obj *model;
  int fallCount = 0;
  bool isFalling = false;
};
