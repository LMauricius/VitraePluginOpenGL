cmake_minimum_required(VERSION 3.7.0)
project(VitraePluginOpenGL VERSION 0.1.0)

include(CTest)
enable_testing()

file(GLOB_RECURSE SrcFiles CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS include/*.h include/*.hpp)
set(GLAD_SrcFile dependencies/glad/src/glad.c)

add_library(VitraePluginOpenGL ${SrcFiles} ${HeaderFiles} ${GLAD_SrcFile})
target_compile_features(VitraePluginOpenGL PUBLIC cxx_std_20)
target_include_directories(VitraePluginOpenGL PUBLIC include)

target_include_directories(VitraePluginOpenGL PUBLIC dependencies/)
target_include_directories(VitraePluginOpenGL PUBLIC dependencies/glad/include)
target_link_libraries(VitraePluginOpenGL PUBLIC glfw)
target_link_libraries(VitraePluginOpenGL PUBLIC VitraeEngine)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
