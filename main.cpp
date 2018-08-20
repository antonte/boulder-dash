#include "antman.hpp"
#include "map.hpp"
#include <coeff/coefficient_registry.hpp>
#include <log/log.hpp>
#include <sdlpp/sdlpp.hpp>
#include <shade/obj.hpp>
#include <shade/shader_program.hpp>
#include <shade/text.hpp>
#include <shade/var.hpp>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

static const char Map1[] = "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
                           "W ======#===============================#==========#================W"
                           "W==  @   =#==========@================@============@==========@=====W"
                           "W=======WWWWWWWWWWWWWW==========WWWWWWWWWWWWWWWWWWWW=WWWWWWWWWWW====W"
                           "W=#=====W==#=========W====#=====W===============W==============W====W"
                           "W=======W============#==========W==###======###=W==###==###====W====W"
                           "W=======W==#=========W=============###======###=W==###==###====W====W"
                           "W=======W============W==========W==###======###=W==###==###====W====W"
                           "W=======WWWWWWWWWWWWWW===@@@====W===============W==============W====W"
                           "W========================@@@====WWWWWWWW=WWWWWWWWWWWWWWW=WWWWWWW====W"
                           "W===========@=======@====@@@====W===============W==============W==#=W"
                           "W=======WWWWWWWWWWWWWW==========W=###=======###=W===###====###=W====W"
                           "W=======W==#====#====W=====#======###=======###=W===###====###=W====W"
                           "W=======W============#==========W=###=======###=W===###====###=W====W"
                           "W=======W===#=====#==W==========W========@======W==============W====W"
                           "W==#====WWWWWWWWWWWWWW======#===WWWWWWWW=WWWWWWWWWWWWWWW=WWWWWWW====W"
                           "W==================================================#================W"
                           "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW";

static const auto Map1Width = 72 - 3;
static_assert((sizeof(Map1) - 1) % Map1Width == 0, "Incorrect map width");

COEFF(TextX, 64);
COEFF(TextY, 40);
COEFF(TextZoom, 1.0f);
COEFF(MapZ, 20.0f);

int main()
{
  CoefficientRegistry::instance().load();
  sdl::Init init(SDL_INIT_EVERYTHING);
  const auto Width = 1280;
  const auto Height = 720;
  sdl::Window win("Boulder Dash", 64, 100, Width, Height, SDL_WINDOW_OPENGL);
  sdl::Renderer rend(win.get(), -1, 0);

  // set up OpenGL flags
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Var<glm::mat4> proj("proj");
  Var<glm::mat4> view("view");
  Var<glm::mat4> mvp("mvp");

  ShaderProgram shad("shad", "shad", mvp, proj, view);

  Library lib(rend.get());

  Map map(lib, Map1, Map1Width);

  sdl::EventHandler evHand;
  auto done = false;
  evHand.quit = [&done](const SDL_QuitEvent &) { done = true; };
  Text coeffText(lib, "font", 59, 115);
  coeffText.setText(CoefficientRegistry::instance().display());
  evHand.keyDown = [&map, &coeffText](const SDL_KeyboardEvent &keyEv) {
    if (CoefficientRegistry::instance().onKeyDown(keyEv.keysym.sym))
    {
      coeffText.setText(CoefficientRegistry::instance().display());
      return;
    }
    switch (keyEv.keysym.sym)
    {
    case SDLK_RIGHT: map.antman->run(0); break;
    case SDLK_DOWN: map.antman->run(1); break;
    case SDLK_LEFT: map.antman->run(2); break;
    case SDLK_UP: map.antman->run(3); break;
    }
  };
  evHand.keyUp = [&map](const SDL_KeyboardEvent &keyEv) {
    switch (keyEv.keysym.sym)
    {
    case SDLK_RIGHT: map.antman->stop(0); break;
    case SDLK_DOWN: map.antman->stop(1); break;
    case SDLK_LEFT: map.antman->stop(2); break;
    case SDLK_UP: map.antman->stop(3); break;
    }
  };

  Text diamondsCount(lib, "font", 59, 115);
  auto simulClock = SDL_GetTicks();
  while (!done)
  {
    while (evHand.poll()) {}
    glClearColor(0.1f, 0.04f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shad.use();
    view = glm::lookAt(glm::vec3(2.0f * map.antman->getDispX(), MapZ, 2.0f * map.antman->getDispY()),
                       glm::vec3(2.0f * map.antman->getDispX(),
                                 0,
                                 2.0f * map.antman->getDispY()), // and looks at the origin
                       glm::vec3(0, 0, -1));
    view.update();
    proj = glm::perspective(glm::radians(45.0f), 1.0f * Width / Height, 0.1f, 100.0f);
    proj.update();
    map.draw(mvp);

    view = glm::mat4(1.0f);
    view.update();
    proj = glm::ortho(0.0f, 128.0f, 0.0f, 72.0f);
    proj.update();

    mvp = glm::translate(glm::vec3(TextX, TextY, 0.0f)) *
          glm::scale(glm::vec3(TextZoom, TextZoom, TextZoom));
    mvp.update();
    coeffText.draw();

    mvp = glm::translate(glm::vec3(0.0f, 2.0f * TextZoom, 0.0f)) * mvp.get();
    mvp.update();
    diamondsCount.setText("Diamonds: " + std::to_string(map.getDiamondsCount()));
    diamondsCount.draw();


    rend.present();

    if (SDL_GetTicks() > simulClock)
    {
      map.tick();
      simulClock += 10;
    }
  }
}
