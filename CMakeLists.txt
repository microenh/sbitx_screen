cmake_minimum_required(VERSION 3.0.0)
project(sbitx_screen VERSION 0.1.0)


# Add GTK3+
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

FILE(GLOB sources *.c)
add_executable(sbitx_screen ${sources})

target_link_libraries(sbitx_screen
  ${GTK3_LIBRARIES}
  asound
  fftw3f
  m
  pthread
  wiringPi
  )

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)