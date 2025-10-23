#include "UseImGui.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "player/player.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>

int main()
{
  // Setup window
  if (!glfwInit())
    return 1;

  // Set up OpenGL 3.3 Core Profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  // Create window with graphics
  GLFWwindow *window = glfwCreateWindow(1280, 720, "Apex Sim", NULL, NULL);

  if (window == NULL)
    return 1;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Vsync

  UseImGui myimgui;
  myimgui.init(window);

  Player players[10] = {
      Player('j', false),
      Player('p', false),
      Player('b', false),
      Player('b', false),
      Player('b', false),
      Player('j', true),
      Player('p', true),
      Player('b', true),
      Player('b', true),
      Player('b', true)};

  while (!glfwWindowShouldClose(window))
  {
    // Checks for key bindings and mouse clicks
    glfwPollEvents();
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);

    glClear(GL_COLOR_BUFFER_BIT);
    myimgui.newFrame();
    myimgui.update(players);
    myimgui.render();
    glfwSwapBuffers(window);
  };
  myimgui.shutdown();

  return 0;
}
