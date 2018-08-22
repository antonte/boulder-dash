#include "diamond.hpp"
#include "antman.hpp"
#include "fall_rate.hpp"
#include "map.hpp"
#include "mob.hpp"
#include <shade/library.hpp>
#include <shade/obj.hpp>

Diamond::Diamond(Library &lib, int x, int y, Map &map)
  : Entity(x, y, map), model(lib.getObj("diamond"))
{
}

float Diamond::moveK() const
{
  return 1.0f / fallRate();
}

void Diamond::tick()
{
  Entity::tick();
  auto newY = y + 1;
  auto under = (*map)(x, newY);
  if (under == ' ')
  {
    fallCount++;
    if (fallCount % fallRate() == 0)
    {
      map->moveTo(*this, x, newY);
      y = newY;
    }
  }
  else if (under != '=' && (*map)(x - 1, y) == ' ' && (*map)(x - 1, y + 1) == ' ')
  {
    fallCount++;
    if (fallCount % fallRate() == 0)
    {
      map->moveTo(*this, x - 1, y);
      x = x - 1;
    }
  }
  else if (under != '=' && (*map)(x + 1, y) == ' ' && (*map)(x + 1, y + 1) == ' ')
  {
    fallCount++;
    if (fallCount % fallRate() == 0)
    {
      map->moveTo(*this, x + 1, y);
      x = x + 1;
    }
  }
  else
    fallCount = 0;
}

void Diamond::draw(Var<glm::mat4> &mvp)
{
  mvp = glm::translate(glm::vec3(getDispX() * 2, 0.0f, getDispY() * 2)) *
        glm::rotate(SDL_GetTicks() / 1000.0f, glm::vec3(0, 0, 1));
  mvp.update();
  model->draw();
}
