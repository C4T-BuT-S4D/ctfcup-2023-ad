cmake_minimum_required(VERSION 3.16)
project(flexinpoint LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# add_compile_options(-D_GLIBCXX_USE_CXX11_ABI=0)
# add_compile_options(-Wall -Wextra -pedantic -Werror)
add_link_options(-Wl,-z,relro,-z,lazy)

include(${PROJECT_SOURCE_DIR}/conan/cmake)

set(CONAN_WORK_DIR ${CMAKE_BINARY_DIR}/conan)
file(MAKE_DIRECTORY ${CONAN_WORK_DIR})

conan_cmake_install(
  PATH_OR_REFERENCE
  ${PROJECT_SOURCE_DIR}/conan/deps
  PROFILE
  ${PROJECT_SOURCE_DIR}/conan/profile
  BUILD
  missing
  INSTALL_FOLDER
  ${CONAN_WORK_DIR})

include(${CONAN_WORK_DIR}/conan_paths.cmake)

find_package(gRPC REQUIRED)
find_package(libpqxx REQUIRED)

add_executable(app src/main.cpp)
target_include_directories(app PRIVATE ${PROJECT_SOURCE_DIR}/inc)
target_link_libraries(app PRIVATE libpqxx::libpqxx gRPC::gRPC)

add_executable(station src/station.cpp)
target_link_libraries(station PRIVATE gRPC::gRPC)
