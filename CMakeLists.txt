cmake_minimum_required(VERSION 3.15...4.0)

project(Voxels VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_SOURCE_DIR}/ThirdParty/glfw" EXCLUDE_FROM_ALL)

set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_SOURCE_DIR}/ThirdParty/glm" EXCLUDE_FROM_ALL)

add_subdirectory("${CMAKE_SOURCE_DIR}/ThirdParty/glad" EXCLUDE_FROM_ALL)

file(GLOB_RECURSE SRC_FILES
	"${CMAKE_SOURCE_DIR}/Source/*.cpp"
	"${CMAKE_SOURCE_DIR}/Source/*.c"
	"${CMAKE_SOURCE_DIR}/Source/*.h"
	"${CMAKE_SOURCE_DIR}/Source/*.hpp"
)

add_executable(Voxels ${SRC_FILES})

target_include_directories(Voxels PRIVATE 
	"${CMAKE_SOURCE_DIR}/ThirdParty/glfw/include"
	"${CMAKE_SOURCE_DIR}/ThirdParty/glm/include"
	"${CMAKE_SOURCE_DIR}/ThirdParty/glad/include"
	"${CMAKE_SOURCE_DIR}/ThirdParty/stb_image/include"
)

target_link_libraries(Voxels PRIVATE
	glm::glm-header-only
	glad
	glfw
)

target_compile_definitions(Voxels PRIVATE ASSETS_PATH=\"${CMAKE_SOURCE_DIR}/assets/\")

target_include_directories(Voxels PRIVATE "${CMAKE_SOURCE_DIR}/Source")


