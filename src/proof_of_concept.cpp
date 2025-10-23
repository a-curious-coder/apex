#include <algorithm>
#include <cmath>
#include <cstdio>
#include <imgui.h>

// --- Backends: Required Headers for GLFW/OpenGL ---
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// --- Emscripten Headers for Web Compatibility ---
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// --- WFTDA Regulation Track Constants (Feet) ---
constexpr float R_IN = 12.5f;           // Inside Arc Radius
constexpr float W_TRACK = 10.0f;        // Track Width
constexpr float R_OUT = R_IN + W_TRACK; // Outside Arc Radius (22.5 ft)
constexpr float L_CENTER_SEP = 35.0f;   // Distance between the two arc centers
constexpr float TRACK_L_MAX =
    2.0f * R_OUT + L_CENTER_SEP;            // Max length (80.0 ft)
constexpr float TRACK_W_MAX = 2.0f * R_OUT; // Max width (45.0 ft)

// Define M_PI if it's not automatically defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Calculation Functions (Using Fixed WFTDA Dimensions) ---

/**
 * @brief Calculates the circumference (perimeter) of the track at a given
 * radius R.
 * @param R The radius from the arc center (e.g., R_IN, R_OUT, or a line in
 * between).
 * @return The calculated circumference in feet.
 */
double calculate_track_circumference(double R) {
  // Circumference = (2 * Straightaway Length) + (2 * PI * R)
  double circumference = (2.0 * L_CENTER_SEP) + (2.0 * M_PI * R);
  return circumference;
}

/**
 * @brief Calculates the area of the playing surface between R_IN and R_OUT.
 * @return The calculated area in square feet.
 */
double calculate_track_area() {
  // Area of an obround (stadium shape): L_straight * 2R + pi * R^2
  // Area = Area_Outer - Area_Inner
  double area_rect = L_CENTER_SEP * (2.0 * R_OUT - 2.0 * R_IN);
  double area_circle = M_PI * (std::pow(R_OUT, 2) - std::pow(R_IN, 2));
  return area_rect + area_circle;
}

// --- Dear ImGui Display Function ---

/**
 * @brief Helper function to convert track coordinates (meters) to screen
 * coordinates (pixels).
 * @param track_x X coordinate in track space (center is 0).
 * @param track_y Y coordinate in track space (center is 0).
 * @param center_x Screen center X.
 * @param center_y Screen center Y.
 * @param scale Zoom scale factor.
 * @param pan_x X offset from panning.
 * @param pan_y Y offset from panning.
 * @return Screen position (ImVec2).
 */
ImVec2 TrackToScreen(float track_x, float track_y, float center_x,
                     float center_y, float scale, float pan_x, float pan_y) {
  float x = center_x + (track_x * scale) + pan_x;
  float y = center_y - (track_y * scale) + pan_y; // Invert Y for screen space
  return ImVec2(x, y);
}

/**
 * @brief Draws the WFTDA Track Visualization with Pan and Zoom controls.
 */
void ShowRollerDerbyCalculatorWindow() {

  // Static state for visualization controls
  static float zoom_scale = 5.0f; // Initial scale factor
  static float pan_x = 0.0f;
  static float pan_y = 0.0f;
  int num_segments = 60; // Smoothness of arcs
  char buf[128];

  ImGui::Begin("WFTDA Regulation Track Analyzer");

  // --- TRACK VISUALIZATION CONTROLS ---
  ImGui::Text("Visualization Controls:");
  ImGui::SameLine();
  if (ImGui::Button("Reset View")) {
    zoom_scale = 5.0f;
    pan_x = 0.0f;
    pan_y = 0.0f;
  }
  ImGui::SliderFloat("Zoom Scale", &zoom_scale, 1.0f, 50.0f, "%.1f");

  // --- TRACK VISUALIZATION DRAWING ---
  ImGui::Spacing();
  ImGui::SeparatorText("WFTDA Track Geometry");

  // Setup the drawing area (fixed size)
  float visualization_w = ImGui::GetContentRegionAvail().x;
  float visualization_h = 450.0f;

  ImGui::BeginChild("TrackView", ImVec2(visualization_w, visualization_h), true,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
  ImDrawList *draw_list = ImGui::GetWindowDrawList();

  // Calculate visualization center relative to the ImGui child window
  ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
  float center_x = canvas_pos.x + visualization_w / 2.0f;
  float center_y = canvas_pos.y + visualization_h / 2.0f;

  // Handle Mouse Input for Panning/Zooming
  ImGuiIO &io = ImGui::GetIO();
  if (ImGui::IsWindowHovered()) {
    // Zoom (Mouse Wheel)
    if (io.MouseWheel != 0.0f) {
      zoom_scale += io.MouseWheel * (zoom_scale * 0.1f);
      if (zoom_scale < 1.0f)
        zoom_scale = 1.0f;
      if (zoom_scale > 50.0f)
        zoom_scale = 50.0f;
    }

    // Panning (Middle Mouse Button Drag)
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
      pan_x += io.MouseDelta.x;
      pan_y += io.MouseDelta.y;
    }
  }

  // --- Track Coordinates for Drawing ---

  // Arc Centers in track space (x, y)
  float arc_center_dist = L_CENTER_SEP / 2.0f;
  ImVec2 c1_track =
      ImVec2(-arc_center_dist, 0.0f);              // Left arc center (-17.5, 0)
  ImVec2 c2_track = ImVec2(arc_center_dist, 0.0f); // Right arc center (17.5, 0)

  // Arc Centers in screen space (pixels)
  ImVec2 c1_screen = TrackToScreen(c1_track.x, c1_track.y, center_x, center_y,
                                   zoom_scale, pan_x, pan_y);
  ImVec2 c2_screen = TrackToScreen(c2_track.x, c2_track.y, center_x, center_y,
                                   zoom_scale, pan_x, pan_y);

  // Scaled radii
  float R_in_scaled = R_IN * zoom_scale;
  float R_out_scaled = R_OUT * zoom_scale;

  // Straightaway corners in track space (y coordinates are R_out and -R_out)
  // Top Straight (inner)
  ImVec2 inner_top_left = TrackToScreen(c1_track.x, R_IN, center_x, center_y,
                                        zoom_scale, pan_x, pan_y);
  ImVec2 inner_top_right = TrackToScreen(c2_track.x, R_IN, center_x, center_y,
                                         zoom_scale, pan_x, pan_y);
  // Bottom Straight (inner)
  ImVec2 inner_bottom_left = TrackToScreen(c1_track.x, -R_IN, center_x,
                                           center_y, zoom_scale, pan_x, pan_y);
  ImVec2 inner_bottom_right = TrackToScreen(c2_track.x, -R_IN, center_x,
                                            center_y, zoom_scale, pan_x, pan_y);

  // Top Straight (outer)
  ImVec2 outer_top_left = TrackToScreen(c1_track.x, R_OUT, center_x, center_y,
                                        zoom_scale, pan_x, pan_y);
  ImVec2 outer_top_right = TrackToScreen(c2_track.x, R_OUT, center_x, center_y,
                                         zoom_scale, pan_x, pan_y);
  // Bottom Straight (outer)
  ImVec2 outer_bottom_left = TrackToScreen(c1_track.x, -R_OUT, center_x,
                                           center_y, zoom_scale, pan_x, pan_y);
  ImVec2 outer_bottom_right = TrackToScreen(c2_track.x, -R_OUT, center_x,
                                            center_y, zoom_scale, pan_x, pan_y);

  // ----------------------------------------------------------------------
  // 1. Draw Track Surface Fill
  // ----------------------------------------------------------------------

  // A. Draw Straightaway Fill (Rectangle)
  draw_list->AddRectFilled(
      outer_top_left, outer_bottom_right,
      IM_COL32(0, 0, 100, 150) // Outer Rectangle (including inner hole)
  );
  draw_list->AddRectFilled(
      inner_top_left, inner_bottom_right,
      IM_COL32(15, 15, 15, 255) // Inner Hole (Dark background color)
  );

  // B. Draw Arc Fill (Left and Right Ends)
  // Left Arc Fill (Outer)
  draw_list->PathClear();
  draw_list->PathArcTo(c1_screen, R_out_scaled, M_PI * 0.5f, M_PI * 1.5f,
                       num_segments);
  draw_list->PathFillConvex(IM_COL32(0, 0, 100, 150));
  // Right Arc Fill (Outer)
  draw_list->PathClear();
  draw_list->PathArcTo(c2_screen, R_out_scaled, M_PI * -0.5f, M_PI * 0.5f,
                       num_segments);
  draw_list->PathFillConvex(IM_COL32(0, 0, 100, 150));

  // Draw Arc Hole Fill (Left and Right Inner)
  // Left Arc Fill (Inner Hole)
  draw_list->PathClear();
  draw_list->PathArcTo(c1_screen, R_in_scaled, M_PI * 0.5f, M_PI * 1.5f,
                       num_segments);
  draw_list->PathFillConvex(IM_COL32(15, 15, 15, 255)); // Inner Hole color
  // Right Arc Fill (Inner Hole)
  draw_list->PathClear();
  draw_list->PathArcTo(c2_screen, R_in_scaled, M_PI * -0.5f, M_PI * 0.5f,
                       num_segments);
  draw_list->PathFillConvex(IM_COL32(15, 15, 15, 255)); // Inner Hole color

  // ----------------------------------------------------------------------
  // 2. Draw Track Boundaries (White Outlines)
  // ----------------------------------------------------------------------

  float line_thickness = 2.0f;

  // A. Inner Boundary
  draw_list->AddLine(inner_top_left, inner_top_right,
                     IM_COL32(255, 255, 255, 255), line_thickness);
  draw_list->AddLine(inner_bottom_left, inner_bottom_right,
                     IM_COL32(255, 255, 255, 255), line_thickness);

  // Drawing inner left arc using PathStroke (fix for missing AddArc)
  draw_list->PathClear();
  draw_list->PathArcTo(c1_screen, R_in_scaled, M_PI * 0.5f, M_PI * 1.5f,
                       num_segments);
  draw_list->PathStroke(IM_COL32(255, 255, 255, 255), 0, line_thickness);

  // Drawing inner right arc using PathStroke (fix for missing AddArc)
  draw_list->PathClear();
  draw_list->PathArcTo(c2_screen, R_in_scaled, M_PI * -0.5f, M_PI * 0.5f,
                       num_segments);
  draw_list->PathStroke(IM_COL32(255, 255, 255, 255), 0, line_thickness);

  // B. Outer Boundary
  draw_list->AddLine(outer_top_left, outer_top_right,
                     IM_COL32(255, 255, 255, 255), line_thickness);
  draw_list->AddLine(outer_bottom_left, outer_bottom_right,
                     IM_COL32(255, 255, 255, 255), line_thickness);

  // Drawing outer left arc using PathStroke (fix for missing AddArc)
  draw_list->PathClear();
  draw_list->PathArcTo(c1_screen, R_out_scaled, M_PI * 0.5f, M_PI * 1.5f,
                       num_segments);
  draw_list->PathStroke(IM_COL32(255, 255, 255, 255), 0, line_thickness);

  // Drawing outer right arc using PathStroke (fix for missing AddArc)
  draw_list->PathClear();
  draw_list->PathArcTo(c2_screen, R_out_scaled, M_PI * -0.5f, M_PI * 0.5f,
                       num_segments);
  draw_list->PathStroke(IM_COL32(255, 255, 255, 255), 0, line_thickness);

  // C. Horizontal Center Line
  ImVec2 center_left = TrackToScreen(c1_track.x, 0.0f, center_x, center_y,
                                     zoom_scale, pan_x, pan_y);
  ImVec2 center_right = TrackToScreen(c2_track.x, 0.0f, center_x, center_y,
                                      zoom_scale, pan_x, pan_y);
  draw_list->AddLine(center_left, center_right, IM_COL32(255, 50, 50, 255),
                     1.0f);
  draw_list->AddCircleFilled(c1_screen, 3.0f, IM_COL32(255, 50, 50, 255));
  draw_list->AddCircleFilled(c2_screen, 3.0f, IM_COL32(255, 50, 50, 255));

  // ----------------------------------------------------------------------
  // 3. Draw Key Track Markings (Pivot and Jammer Lines)
  // ----------------------------------------------------------------------

  // Standard WFTDA lines are 30 ft apart. We center the 35ft straightaway
  // around the track's X-center. X-range of straightaway: -17.5 ft to 17.5 ft
  constexpr float X_PIVOT = -12.5f; // Position of the Pivot Line (Pack Line)
  constexpr float X_JAMMER =
      17.5f; // Position of the Jammer Line / Start/Finish (Tangent point)

  // Jammer Line (Thick, Solid Line - Start/Finish)
  ImVec2 jammer_top = TrackToScreen(X_JAMMER, R_OUT, center_x, center_y,
                                    zoom_scale, pan_x, pan_y);
  ImVec2 jammer_bottom = TrackToScreen(X_JAMMER, -R_OUT, center_x, center_y,
                                       zoom_scale, pan_x, pan_y);
  draw_list->AddLine(jammer_top, jammer_bottom, IM_COL32(255, 255, 255, 255),
                     4.0f); // Thicker white line

  // Pivot Line (Thinner Line - Pack Line)
  ImVec2 pivot_top = TrackToScreen(X_PIVOT, R_OUT, center_x, center_y,
                                   zoom_scale, pan_x, pan_y);
  ImVec2 pivot_bottom = TrackToScreen(X_PIVOT, -R_OUT, center_x, center_y,
                                      zoom_scale, pan_x, pan_y);
  draw_list->AddLine(pivot_top, pivot_bottom, IM_COL32(200, 200, 200, 255),
                     1.0f); // Thinner gray line

  // --- 4. Draw Infield Markers (Pivot and Jammer Boxes) ---
  float pivot_box_radius = 2.0f * zoom_scale; // Small circle in the infield

  // Infield Pivot Star Box (at X_PIVOT, centered on Y=0)
  ImVec2 pivot_center_screen = TrackToScreen(X_PIVOT, 0.0f, center_x, center_y,
                                             zoom_scale, pan_x, pan_y);
  draw_list->AddCircleFilled(pivot_center_screen, pivot_box_radius,
                             IM_COL32(255, 255, 0, 255),
                             20); // Yellow/Gold marker

  // Infield Jammer Star Box (at X_JAMMER, centered on Y=0)
  ImVec2 jammer_center_screen = TrackToScreen(
      X_JAMMER, 0.0f, center_x, center_y, zoom_scale, pan_x, pan_y);
  draw_list->AddCircleFilled(jammer_center_screen, pivot_box_radius * 1.5f,
                             IM_COL32(255, 255, 255, 255),
                             20); // Larger White marker

  ImGui::EndChild();

  // --- CALCULATIONS DISPLAY ---

  ImGui::Spacing();
  ImGui::SeparatorText("WFTDA Regulation Results (Fixed Dimensions)");

  // Area Result
  double area = calculate_track_area();
  ImGui::Text("Playing Surface Area:");
  std::snprintf(buf, sizeof(buf), "%.2f sq ft", (float)area);
  ImGui::SameLine(ImGui::GetWindowWidth() * 0.6f);
  ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "%s", buf);

  ImGui::Separator();

  // Inside Circumference Result
  double c_in = calculate_track_circumference(R_IN);
  ImGui::Text("Inside Circumference (R=%.1f ft):", R_IN);
  std::snprintf(buf, sizeof(buf), "%.2f ft", (float)c_in);
  ImGui::SameLine(ImGui::GetWindowWidth() * 0.6f);
  ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.9f, 1.0f), "%s", buf);

  // Outside Circumference Result
  double c_out = calculate_track_circumference(R_OUT);
  ImGui::Text("Outside Circumference (R=%.1f ft):", R_OUT);
  std::snprintf(buf, sizeof(buf), "%.2f ft", (float)c_out);
  ImGui::SameLine(ImGui::GetWindowWidth() * 0.6f);
  ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.9f, 1.0f), "%s", buf);

  ImGui::Spacing();

  // Instructions
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
  ImGui::TextWrapped("Zoom: Use the slider or the mouse wheel while hovering "
                     "over the visualization.");
  ImGui::TextWrapped("Pan: Drag with the Middle Mouse Button (MMB).");
  ImGui::PopStyleColor();

  ImGui::End();
}

// --- Helper function for setting up GLFW/OpenGL ---
GLFWwindow *setup_window() {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return NULL;
  }

  // Set up OpenGL 3.3 Core Profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(1280, 720, "WFTDA Track Analyzer", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to create GLFW window\n");
    glfwTerminate();
    return NULL;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Initialize GLEW (or GLAD if you prefer)
  // IMPORTANT: glewExperimental = GL_TRUE; is sometimes needed for older GLEW
  // versions
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    glfwTerminate();
    return NULL;
  }

  return window;
}

// --- Main application loop function ---
/**
 * @brief This function contains the logic that runs once per frame.
 * @param window_ptr A void* pointer to the GLFWwindow structure (required by
 * Emscripten API).
 */
void main_loop(
    void *window_ptr) { // <-- FIX: Changed signature from GLFWwindow* to void*
  // Cast the generic void* back to the specific GLFWwindow* pointer
  GLFWwindow *window = (GLFWwindow *)window_ptr;

  // 4. Process Events and Input
  glfwPollEvents();

  // 5. Start the ImGui Frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // 6. Call the calculator display function
  ShowRollerDerbyCalculatorWindow();

  // 7. Rendering
  ImGui::Render();
  int display_w, display_h;
  glfwGetFramebufferSize(window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);

  // Clear the screen (Dark gray background)
  glClearColor(0.15f, 0.15f, 0.15f, 1.00f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Render ImGui Draw Data
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // 8. Swap Buffers
  glfwSwapBuffers(window);
}

// --- Main Application Entry Point ---
int main() {
  // 1. Initialize Window/Graphics Backend (GLFW/OpenGL)
  GLFWwindow *window = setup_window();
  if (window == NULL)
    return 1;

  // 2. Initialize Dear ImGui Context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_None; // Enable Docking for better layout

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // 3. Initialize ImGui Backends (Platform/Renderer)
  // GLSL version for WebGL 2.0 compatibility
  const char *glsl_version = "#version 300 es";
  ImGui_ImplGlfw_InitForOpenGL(window, true); // true = install callbacks
  ImGui_ImplOpenGL3_Init(glsl_version);

  // --- Main Application Loop ---
#ifdef __EMSCRIPTEN__
  // Use Emscripten's main loop management for web compatibility
  // Passes the 'window' pointer as the 'arg' to main_loop(void* arg)
  emscripten_set_main_loop_arg(main_loop, window, 0, 1);
#else
  // Desktop loop (retains original behavior for non-Emscripten compilation)
  while (!glfwWindowShouldClose(window)) {
    main_loop(window);
  }
#endif

  // --- Cleanup ---
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
