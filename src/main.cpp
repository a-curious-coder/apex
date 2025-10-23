#include "UseImGui.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "player/player.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>

// --- Emscripten Headers for Web Compatibility ---
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

GLFWwindow *window = nullptr;
UseImGui myimgui;
Player players[10];

void main_loop(void *arg)
{
  // Checks for key bindings and mouse clicks
  glfwPollEvents();
  glClearColor(0.45f, 0.55f, 0.60f, 1.00f);

  glClear(GL_COLOR_BUFFER_BIT);
  myimgui.newFrame();
  myimgui.update(players);
  myimgui.render();
  glfwSwapBuffers(window);
}

int main()
{
  // Setup window
  if (!glfwInit())
    return 1;

  // Set up OpenGL 3.3 Core Profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  // Create window with graphics
  window = glfwCreateWindow(1280, 720, "Apex Sim", NULL, NULL);

  if (window == NULL)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Vsync

  myimgui.init(window);

  players[0] = Player('j', false);
  players[1] = Player('p', false);
  players[2] = Player('b', false);
  players[3] = Player('b', false);
  players[4] = Player('b', false);
  players[5] = Player('j', true);
  players[6] = Player('p', true);
  players[7] = Player('b', true);
  players[8] = Player('b', true);
  players[9] = Player('b', true);

// Desktop loop (retains original behavior for non-Emscripten compilation)
#ifdef __EMSCRIPTEN__
  // Use Emscripten's main loop management for web compatibility
  // Passes the 'window' pointer as the 'arg' to main_loop(void* arg)
  emscripten_set_main_loop_arg(&main_loop, 0, 0, true);
#else
  while (!glfwWindowShouldClose(window))
  {
    main_loop(0);
  };
#endif

myimgui.shutdown();
return 0;
}
