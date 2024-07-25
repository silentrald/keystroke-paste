cmake_minimum_required(VERSION 3.12.1)

project(kp VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 17)

include_directories(src)

find_package(X11)
if (X11_FOUND)
  message("Using X11")
  add_compile_definitions(KP_LINUX_X11)

  add_executable(${PROJECT_NAME}
    src/x11/clip.cpp
    src/x11/data.cpp
    src/x11/key.cpp
    src/x11/types.cpp
    src/main.cpp
  )

  include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
  # If XTest.h is needed, add Xtst below
  target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})
endif ()

