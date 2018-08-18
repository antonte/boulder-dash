#include "antman.hpp"
#include <sdlpp/sdlpp.hpp>
#include <shade/obj.hpp>
#include <shade/shader_program.hpp>
#include <shade/var.hpp>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

static const char Map[] = "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
                          "W ======#===============================#==========#================W"
                          "W=========#==========@================@============@==========@=====W"
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

static const auto MapWidth = 72 - 3;
static const auto MapHeight = (sizeof(Map) - 1) / MapWidth;
static_assert((sizeof(Map) - 1) % MapWidth == 0, "Incorrect map width");

int main()
{
  std::string map = Map;
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

  Var<glm::mat4> proj("proj",
                      glm::perspective(glm::radians(45.0f), 1.0f * Width / Height, 0.1f, 100.0f));
  Var<glm::mat4> view("view");
  Var<glm::mat4> mvp("mvp");

  ShaderProgram shad("shad", "shad", mvp, proj, view);

  TextureLibrary texLib(rend.get());

  Obj dirt(texLib, "dirt");
  Obj boulder(texLib, "boulder");
  Obj diamond(texLib, "diamond");
  Obj wall(texLib, "wall");

  Antman antman(texLib, [&map](int x, int y) -> char & { return map[x + y * MapWidth]; });

  sdl::EventHandler evHand;
  auto done = false;
  evHand.quit = [&done](const SDL_QuitEvent &) { done = true; };
  evHand.keyDown = [&antman](const SDL_KeyboardEvent &keyEv) {
    switch (keyEv.keysym.sym)
    {
    case SDLK_RIGHT: antman.run(0); break;
    case SDLK_DOWN: antman.run(1); break;
    case SDLK_LEFT: antman.run(2); break;
    case SDLK_UP: antman.run(3); break;
    }
  };
  evHand.keyUp = [&antman](const SDL_KeyboardEvent &keyEv) {
    switch (keyEv.keysym.sym)
    {
    case SDLK_RIGHT: antman.stop(0); break;
    case SDLK_DOWN: antman.stop(1); break;
    case SDLK_LEFT: antman.stop(2); break;
    case SDLK_UP: antman.stop(3); break;
    }
  };


  auto simulClock = SDL_GetTicks();
  while (!done)
  {
    while (evHand.poll()) {}
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shad.use();
    view = glm::lookAt(glm::vec3(2.0f * antman.getX(), 20, 2.0f * antman.getY()),
                       glm::vec3(2.0f * antman.getX(), 0, 2.0f * antman.getY()), // and looks at the origin
                       glm::vec3(0, 0, -1));
    view.update();
    proj.update();

    for (auto y = 0u; y < MapHeight; ++y)
      for (auto x = 0u; x < MapWidth; ++x)
      {
        switch (map[x + y * MapWidth])
        {
        case 'W':
          mvp = glm::translate(glm::vec3(x * 2, 0.0f, y * 2));
          mvp.update();
          wall.activate();
          break;
        case '@':
          mvp = glm::translate(glm::vec3(x * 2, 0.0f, y * 2));
          mvp.update();
          boulder.activate();
          break;
        case '=':
          mvp = glm::translate(glm::vec3(x * 2, 0.0f, y * 2));
          mvp.update();
          dirt.activate();
          break;
        case '#':
          mvp = glm::translate(glm::vec3(x * 2, 0.0f, y * 2)) *
                glm::rotate(SDL_GetTicks() / 1000.0f, glm::vec3(0, 0, 1));
          mvp.update();
          diamond.activate();
          break;
        }
      }

    antman.draw(mvp);
    rend.present();

    if (SDL_GetTicks() > simulClock)
    {
      antman.tick();
      simulClock += 10;
    }
  }
}
