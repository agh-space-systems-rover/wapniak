cmake_minimum_required(VERSION 3.10)
project(Spectrograph CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# You can turn off this if you want release builds
set(CMAKE_BUILD_TYPE Debug)

# Options for ImGui and ImPlot
option(IMGUI_BUNDLE_IMPLOT "Include ImPlot with ImGui" ON)

# Find OpenGL and GLFW
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories(
    ${GLFW_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/external/implot
)

# ImGui source files
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

# ImPlot source files
set(IMPLOT_SOURCES
    external/implot/implot.cpp
    external/implot/implot_items.cpp
)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/gui_app.cpp
    src/queue.cpp
    src/data.cpp
    src/static_data.S
    ${IMGUI_SOURCES}
    ${IMPLOT_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY} ${GLFW_LIBRARIES})
