#!/bin/bash

# Configuration
MAIN_SRC="main.cpp"
IMGUI_CORE_SRCS="imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_tables.cpp imgui/imgui_demo.cpp"
IMGUI_BACKENDS_SRCS="imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp"
INCLUDES="-Iimgui -Iimgui/backends"
OUTPUT_FILE="index.html"

echo "Starting Emscripten compilation..."

# Execute the compilation command
emcc \
    $MAIN_SRC \
    $IMGUI_CORE_SRCS \
    $IMGUI_BACKENDS_SRCS \
    $INCLUDES \
    -s USE_GLFW=3 \
    -s USE_WEBGL2=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s GL_ASSERTIONS=1 \
    --no-entry \
    -o $OUTPUT_FILE

if [ $? -eq 0 ]; then
    echo "Compilation successful. Output files generated: index.html, index.wasm, index.js"
    echo "To run, you must use a local web server (e.g., python3 -m http.server)."
else
    echo "Compilation failed."
fi

