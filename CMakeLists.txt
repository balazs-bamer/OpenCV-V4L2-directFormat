cmake_minimum_required(VERSION 2.8)
project( OpenCV_V4L2_directFormat )
# The version number.
set (OpenCV_V4L2_directFormat_VERSION_MAJOR 0)
set (OpenCV_V4L2_directFormat_VERSION_MINOR 1)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/OpenCV_V4L2_directFormat_Config.h.in"
  "${PROJECT_BINARY_DIR}/OpenCV_V4L2_directFormat_Config.h"
  )
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories(
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/OpenCV_V4L2_directFormat_videoio"
)

add_subdirectory (OpenCV_V4L2_directFormat_videoio) 
 
find_package( OpenCV REQUIRED )
find_package( Curses REQUIRED )
add_executable( test-modified main-mod.cpp )
target_link_libraries( test-modified ${OpenCV_LIBS} ${Curses_LIBS} videoio_mod)
add_executable( test-original main-cv.cpp )
target_link_libraries( test-original ${OpenCV_LIBS} ${Curses_LIBS} )
