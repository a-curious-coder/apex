#ifndef USEIMGUI_HPP
#define USEIMGUI_HPP

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include "imgui_impl_opengl3.h"

class UseImGui
{
public:
  void init(GLFWwindow *window);
  void newFrame();
  virtual void update();
  void render();
  void shutdown();
};

#endif
