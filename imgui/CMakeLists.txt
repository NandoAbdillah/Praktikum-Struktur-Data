cmake_minimum_required(VERSION 3.10)

project(ImGuiApp)

set(CMAKE_CXX_STANDARD 17)

# GLFW
add_subdirectory(external/glfw)

# ImGui path
set(IMGUI_DIR external/imgui)

add_executable(app
    src/ui/style.cpp
    src/utils/helper.cpp
    src/data.cpp
    src/main.cpp

    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp

    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(app PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_SOURCE_DIR}
)

find_package(OpenGL REQUIRED)

target_link_libraries(app
    glfw
    OpenGL::GL
)