#include "UseImGui.hpp"
#include <cstdlib>

void UseImGui::init(GLFWwindow *window)
{
  // Start utilising ImGui now GLFW is setup.
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard
  const char *glsl_version = "#version 300 es";
  // Initialise ImGui Backends (Platform/Renderer)
  // GLSL version for WebGL 2.0 compatibility
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImGui::StyleColorsDark();
}

void UseImGui::newFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void render_player(ImDrawList *draw_list, Player *player)
{
  float playerSize = 15.0;
  ImU32 playerColour = player->team ? IM_COL32(255, 255, 255, 255) : IM_COL32(127, 127, 127, 255);
  auto pos = player->getPosition();
  draw_list->AddCircleFilled(ImVec2(static_cast<float>(pos.first), static_cast<float>(pos.second)), playerSize, playerColour, 20);
}

void UseImGui::update(Player *players)
{
  ImGui::Begin("Apex Multi-agent Reinforcement Learning Arena");
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  for (int i = 0; i < 10; ++i)
  {
    render_player(draw_list, &players[i]);
    int dx = (rand() % 11) - 5; // random step between -5 and 5
    int dy = (rand() % 11) - 5;
    auto pos = players[i].getPosition();
    players[i].move(pos.first + dx, pos.second + dy);
  }
  ImGui::End();
}

void UseImGui::render()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UseImGui::shutdown()
{
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
