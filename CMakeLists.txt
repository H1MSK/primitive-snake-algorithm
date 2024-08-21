cmake_minimum_required(VERSION 3.14)

project(AutoSnakeQt LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)

add_executable(AutoSnakeQt
  src/main.cpp
  src/find_path.cpp
  src/io.cpp
  src/visual.cpp
  include/common.hpp
  include/map.hpp
  include/snake.hpp
)
include_directories(include)
target_link_libraries(AutoSnakeQt Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui)

include(GNUInstallDirs)
install(TARGETS AutoSnakeQt
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
