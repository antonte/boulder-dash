#pragma once
#include <functional>
#include <memory>
#include <shade/var.hpp>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

class TextureLibrary;
class Obj;
class Antman
{
public:
  Antman(TextureLibrary &, std::function<char &(int x, int y)> &&mapFunc);
  void draw(Var<glm::mat4> &mvp);
  void run(int dir);
  void tick();
  void stop(int dir);
  float getX() const;
  float getY() const;

private:
  std::vector<std::unique_ptr<Obj>> standAnim;
  std::vector<std::unique_ptr<Obj>> walkAnim;
  std::vector<std::unique_ptr<Obj>> pushAnim;
  int oldDir = 0;
  int dir = -1;
  int x = 1;
  int y = 1;
  float dispX = 1.0f;
  float dispY = 1.0f;
  std::function<char &(int x, int y)> mapFunc;
  int ticks = 0;
  int coolDown = 0;
};
