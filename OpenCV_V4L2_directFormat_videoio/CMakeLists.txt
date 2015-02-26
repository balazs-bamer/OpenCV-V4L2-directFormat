set(the_description "Media I/O")

# ----------------------------------------------------------------------------
#  CMake file for videoio. See root CMakeLists.txt
#   Some parts taken from version of Hartmut Seichter, HIT Lab NZ.
#   Jose Luis Blanco, 2008
# ----------------------------------------------------------------------------

set(videoio_hdrs
    ${CMAKE_CURRENT_LIST_DIR}/precomp.hpp
    ${CMAKE_CURRENT_LIST_DIR}/opencv2/retrieve.hpp
    ${CMAKE_CURRENT_LIST_DIR}/opencv2/videoio_mod.hpp
    )

set(videoio_srcs
    ${CMAKE_CURRENT_LIST_DIR}/cap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cap_images.cpp
    )

#if(HAVE_LIBV4L)
#  list(APPEND videoio_srcs ${CMAKE_CURRENT_LIST_DIR}/cap_libv4l.cpp)
#endif()
#if(HAVE_CAMV4L OR HAVE_CAMV4L2 OR HAVE_VIDEOIO)
  list(APPEND videoio_srcs ${CMAKE_CURRENT_LIST_DIR}/cap_v4l.cpp)
#endif()

add_library(videoio_mod ${videoio_srcs} ${videoio_hdrs})

