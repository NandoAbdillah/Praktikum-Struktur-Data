cmake_minimum_required(VERSION 3.10)

project(ImGuiApp)

set(CMAKE_CXX_STANDARD 17)

# Mengatur path untuk ImGui
set(IMGUI_DIR external/imgui)

# Menambahkan GLFW dari subdirectory
add_subdirectory(external/glfw)

# Mengelompokkan file sumber milikmu sendiri (Lognuts)
set(APP_SOURCES
    src/main.cpp
    src/data.cpp
    src/utils/helper.cpp
    src/ui/style.cpp
    src/ui/view/customer_view.cpp
    src/ui/view/admin_view.cpp
    src/controller/customer.cpp
    src/controller/service.cpp
    src/controller/mechanic.cpp
)

# Mengelompokkan file sumber bawaan ImGui
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Membuat target executable gabungan
add_executable(app ${APP_SOURCES} ${IMGUI_SOURCES})

# Mendaftarkan direktori root agar tidak perlu pakai "../../" (Include Hell)
target_include_directories(app PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# Mencari dan menghubungkan OpenGL & GLFW
find_package(OpenGL REQUIRED)

target_link_libraries(app
    glfw
    OpenGL::GL
)