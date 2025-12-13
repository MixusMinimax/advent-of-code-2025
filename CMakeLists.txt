cmake_minimum_required(VERSION 3.10.0)
project(aoc2025 VERSION 0.1.0 LANGUAGES C CXX)

# Cpp version 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Dependencies
include(cmake/CPM.cmake)
CPMAddPackage(
        GITHUB_REPOSITORY glfw/glfw
        VERSION 3.4
        GIT_TAG 3.4
)
add_library(glad_lib STATIC vendor/glad/src/gl.c)
target_include_directories(glad_lib PUBLIC vendor/glad/include)
CPMAddPackage(gh:ocornut/imgui@1.92.5)
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp # optionally comment this out
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(imgui PUBLIC glfw)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_compile_definitions(imgui PUBLIC -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS) # optional imgui setting
set_target_properties(imgui PROPERTIES FOLDER third-party) # optional IDE dir

CPMAddPackage(
        GITLAB_REPOSITORY libeigen/eigen
        VERSION 5.0.1
        GIT_TAG 5.0.1
)

# Subdirectories
add_subdirectory(util)

add_subdirectory(d01)
add_subdirectory(d02)
add_subdirectory(d03)
add_subdirectory(d04)
add_subdirectory(d05)
add_subdirectory(d06)
add_subdirectory(d07)
add_subdirectory(d08)
add_subdirectory(d09)
