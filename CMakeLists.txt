cmake_minimum_required(VERSION 3.10)

# download glew
include(./3rdparty/DownloadProject/DownloadProject.cmake)
download_project(
	PROJ glew_src
	URL https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0.tgz
)

# create project
project(GPGPU)

# configuration
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add libraries subdirectory
set(LIBRARIES_PATH "${CMAKE_SOURCE_DIR}/3rdparty")
add_subdirectory(${glew_src_SOURCE_DIR}/build/cmake ${glew_src_BINARY_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${LIBRARIES_PATH}/glfw)
add_subdirectory(${LIBRARIES_PATH}/glm)

# add source files
file(GLOB_RECURSE ALL_CPP_FILES "${CMAKE_SOURCE_DIR}/main.cpp")
add_executable(GPGPU
	"${ALL_CPP_FILES}"
)

# set include directories
target_include_directories(GPGPU PRIVATE
	"${glew_src_SOURCE_DIR}/include"
)

# set link libraries
target_link_libraries(GPGPU
	glew
	glfw
	glm
)
