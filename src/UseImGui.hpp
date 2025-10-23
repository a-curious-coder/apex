#ifndef USEIMGUI_HPP
#define USEIMGUI_HPP

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include "imgui_impl_opengl3.h"
#include "player/player.hpp"

class UseImGui
{
public:
  void init(GLFWwindow *window);
  void newFrame();
  virtual void update(Player *players);
  void render();
  void shutdown();
};

#endif
