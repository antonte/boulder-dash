#pragma once
#include "entity.hpp"
#include <vector>

class Library;
class Obj;
class Boulder;

class Antman : public Entity
{
public:
  enum { Symb = 'A' };
  Antman(Library &, Map &map);
  void draw(Var<glm::mat4> &mvp) override;
  void run(int dir);
  void stop(int dir);
  void tick() override;
  void die();

private:
  float moveK() const override;

  std::vector<Obj *> standAnim;
  std::vector<Obj *> walkAnim;
  std::vector<Obj *> pushAnim;
  int oldDir = 0;
  int dir = -1;
  int ticks = 0;
  int coolDown = 0;
  Boulder *pushingBldr = nullptr;
};
