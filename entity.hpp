#pragma once

#include <shade/var.hpp>
#include <glm/mat4x4.hpp>

class Map;

class Entity
{
public:
  Entity(int x, int y, Map &);
  virtual ~Entity() = default;
  int getX() const;
  int getY() const;
  float getDispX() const;
  float getDispY() const;
  virtual void tick();
  virtual void draw(Var<glm::mat4> &mvp) = 0;

protected:
  virtual float moveK() const = 0;

  Map *map;
  int x;
  int y;

private:
  float sign(float x) const;

  float dispX;
  float dispY;
};
