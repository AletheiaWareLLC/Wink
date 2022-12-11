cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

string(APPEND CMAKE_CXX_FLAGS " -Wall")
string(APPEND CMAKE_CXX_FLAGS " -Werror")
string(APPEND CMAKE_CXX_FLAGS " -pedantic")

set(PROJECT_NAME Wink)

project(${PROJECT_NAME} VERSION 0.0.1)

set(CMAKE_STATIC_LIBRARY_PREFIX "")

add_subdirectory("src")

add_subdirectory("samples")

include(FetchContent)
FetchContent_Declare(
  GoogleTest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(GoogleTest)

enable_testing()

add_subdirectory("test/src")
