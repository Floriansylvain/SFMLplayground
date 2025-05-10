cmake_minimum_required(VERSION 3.28)
project(SFMLplayground LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS src/*.hpp)

add_executable(SFMLplayground ${SOURCES} ${HEADERS})
target_compile_features(SFMLplayground PRIVATE cxx_std_17)
target_link_libraries(SFMLplayground PRIVATE SFML::Graphics)
