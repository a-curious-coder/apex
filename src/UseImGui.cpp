#include "UseImGui.hpp"

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

void render_test_logo()
{
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  float sz = 300.0f;
  static ImVec4 col1 = ImVec4(68.0 / 255.0, 83.0 / 255.0, 89.0 / 255.0, 1.0f);
  static ImVec4 col2 = ImVec4(40.0 / 255.0, 60.0 / 255.0, 80.0 / 255.0, 1.0f);
  static ImVec4 col3 = ImVec4(50.0 / 255.0, 65.0 / 255.0, 82.0 / 255.0, 1.0f);
  static ImVec4 col4 = ImVec4(20.0 / 255.0, 40.0 / 255.0, 60.0 / 255.0, 1.0f);
  const ImVec2 p = ImGui::GetCursorScreenPos();
  float x = p.x + 4.0f, y = p.y + 4.0f;
  draw_list->AddQuadFilled(ImVec2(x, y + 0.25 * sz), ImVec2(x + 0.5 * sz, y + 0.5 * sz), ImVec2(x + sz, y + 0.25 * sz), ImVec2(x + 0.5 * sz, y), ImColor(col1));
  draw_list->AddQuadFilled(ImVec2(x, y + 0.25 * sz), ImVec2(x + 0.5 * sz, y + 0.5 * sz), ImVec2(x + 0.5 * sz, y + 1.0 * sz), ImVec2(x, y + 0.75 * sz), ImColor(col2));
  draw_list->AddQuadFilled(ImVec2(x + 0.5 * sz, y + 0.5 * sz), ImVec2(x + sz, y + 0.25 * sz), ImVec2(x + sz, y + 0.75 * sz), ImVec2(x + 0.5 * sz, y + 1.0 * sz), ImColor(col3));
  draw_list->AddLine(ImVec2(x + 0.75 * sz, y + 0.375 * sz), ImVec2(x + 0.75 * sz, y + 0.875 * sz), ImColor(col4));
  draw_list->AddBezierCubic(
      ImVec2(x + 0.72f * sz, y + 0.24f * sz),
      ImVec2(x + 0.68f * sz, y + 0.15f * sz),
      ImVec2(x + 0.48f * sz, y + 0.13f * sz),
      ImVec2(x + 0.39f * sz, y + 0.17f * sz),
      ImColor(col4),
      10.0f,
      18);
  draw_list->AddBezierCubic(
      ImVec2(x + 0.39f * sz, y + 0.17f * sz),
      ImVec2(x + 0.2f * sz, y + 0.25f * sz),
      ImVec2(x + 0.3f * sz, y + 0.35f * sz),
      ImVec2(x + 0.49f * sz, y + 0.38f * sz),
      ImColor(col4),
      10.0f,
      18);
}

void UseImGui::update()
{
  ImGui::Begin("Test logo");
  render_test_logo();
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
