cmake_minimum_required(VERSION 4.1.2)
project(APEX)

# System installed dependencies
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(IMGUI_DIR "imgui")

set(IMGUI_SOURCES
    # Core Files
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp

    # Backends
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

# Add all source files to a static library target named 'imgui'
add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui
  PUBLIC
    imgui
)

add_executable(${PROJECT_NAME} src/main.cpp src/UseImGui.cpp)

# Link ImGui, the external libraries, and set necessary include paths
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        imgui               # Links the ImGui library we created
  glfw                # Links GLFW
  ${OpenGL_LIBRARIES} # Links OpenGL (e.g., -lGL)
  ${GLEW_LIBRARIES}
  GL                  # Explicitly link GL for Arch Linux
)
target_include_directories(${PROJECT_NAME}
  PRIVATE
    imgui
    imgui/backends
    ${OpenGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)
install(TARGETS APEX DESTINATION bin)
