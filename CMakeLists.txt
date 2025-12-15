cmake_minimum_required(VERSION 3.10.0)
project(aoc2025 VERSION 0.1.0 LANGUAGES C CXX)

# Cpp version 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Dependencies
include(cmake/CPM.cmake)
#CPMAddPackage(
#        GITHUB_REPOSITORY glfw/glfw
#        VERSION 3.4
#        GIT_TAG 3.4
#)
#add_library(glad_lib STATIC vendor/glad/src/gl.c)
#target_include_directories(glad_lib PUBLIC vendor/glad/include)
#CPMAddPackage(gh:ocornut/imgui@1.92.5)
#add_library(imgui STATIC
#        ${imgui_SOURCE_DIR}/imgui.cpp
#        ${imgui_SOURCE_DIR}/imgui_demo.cpp # optionally comment this out
#        ${imgui_SOURCE_DIR}/imgui_draw.cpp
#        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
#        ${imgui_SOURCE_DIR}/imgui_tables.cpp
#        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
#        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
#)
#target_link_libraries(imgui PUBLIC glfw)
#target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
#target_compile_definitions(imgui PUBLIC -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS) # optional imgui setting
#set_target_properties(imgui PROPERTIES FOLDER third-party) # optional IDE dir

CPMAddPackage(
        GITLAB_REPOSITORY libeigen/eigen
        VERSION 5.0.1
        GIT_TAG 5.0.1
)

CPMAddPackage(gh:foonathan/lexy@2025.05.0)

#CPMAddPackage(gh:madler/zlib@1.3.1)

#set(STATIC_GMP ON CACHE BOOL "" FORCE)
#set(STATIC_MPFR ON CACHE BOOL "" FORCE)
#
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)
#set(GMP_DIR "C:/msys64/mingw64")
#find_package(GMP REQUIRED)
#set(MPFR_DIR "C:/msys64/mingw64")
#find_package(MPFR REQUIRED)
#
#set(AUTOBUILD OFF CACHE BOOL "" FORCE)
#
## Required for exact solving
#set(GMP ON CACHE BOOL "" FORCE)
#set(MPFR ON CACHE BOOL "" FORCE)
#
## Disable optional / unwanted features
#set(ZLIB OFF CACHE BOOL "" FORCE)
#set(READLINE OFF CACHE BOOL "" FORCE)
#set(PAPILO OFF CACHE BOOL "" FORCE)
#set(ZIMPL OFF CACHE BOOL "" FORCE)
#set(AMPL OFF CACHE BOOL "" FORCE)
#set(IPOPT OFF CACHE BOOL "" FORCE)
#set(LAPACK OFF CACHE BOOL "" FORCE)
#set(WORHP OFF CACHE BOOL "" FORCE)
#set(CONOPT OFF CACHE BOOL "" FORCE)
#set(LPSCHECK OFF CACHE BOOL "" FORCE)
#
## Reasonable defaults
#set(THREADSAFE ON CACHE BOOL "" FORCE)
#set(DEBUGSOL OFF CACHE BOOL "" FORCE)
#set(SANITIZE OFF CACHE BOOL "" FORCE)
#set(COVERAGE OFF CACHE BOOL "" FORCE)
#set(LTO OFF CACHE BOOL "" FORCE)
#
#CPMAddPackage(gh:scipopt/soplex@8.0.0)
#CPMAddPackage(gh:scipopt/scip@10.0.0)

# Work around broken FindQuadmath.cmake in papilo/soplex
set(Quadmath_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "" FORCE)
# Create an IMPORTED library for linking
if (NOT TARGET Quadmath::quadmath)
    add_library(Quadmath::quadmath INTERFACE IMPORTED)
    set_target_properties(Quadmath::quadmath PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${Quadmath_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "quadmath"   # <- system library
    )
endif ()
find_package(papilo REQUIRED)
find_package(soplex REQUIRED)
find_package(scip REQUIRED)

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
add_subdirectory(d10)
