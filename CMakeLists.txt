cmake_minimum_required(VERSION 3.11)
project(cliruntime DESCRIPTION "Простой daemon для Linux, общающийся с клиентом по UDP" LANGUAGES CXX)

set(PROJECT_VERSION "0.5.0")

enable_testing()


# --- Для сборщика ---

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -pedantic -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "-DNDEBUG -Wall -Wextra -pedantic -O3")
endif()

# --- Директории ---

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/defines.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/src/defines.hpp
    @ONLY
)

include_directories(${CMAKE_SOURCE_DIR}/lib/daemonpp/)
#file(GLOB ALLHEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
#file(GLOB ALLSOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# --- GTest ---

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1  # Using a stable release tag
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/extern/googletest-src"
  BINARY_DIR "${CMAKE_SOURCE_DIR}/extern/googletest-build"
  CMAKE_ARGS 
    -DCMAKE_BUILD_TYPE=Release
    -DBUILD_GMOCK=OFF
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${CMAKE_SOURCE_DIR}/extern/googletest-install/lib
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_SOURCE_DIR}/extern/googletest-install/lib
)

# Set this to OFF to avoid building tests for gtest itself
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)

add_library(udp 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/udp.cpp
)

add_library(comutils 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/command_utils.cpp
)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/defines.hpp)
target_link_libraries(${PROJECT_NAME} udp comutils)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# --- Для установки сервиса ---

# Configure .service file
configure_file(${CMAKE_SOURCE_DIR}/bin/systemd/daemonpp.service.in ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}.service)

# Configure .conf file
configure_file(${CMAKE_SOURCE_DIR}/bin/systemd/daemonpp.conf.in ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}.conf)

# Install the config file .conf
install(FILES ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}.conf DESTINATION /etc/${PROJECT_NAME}/)

# Install the systemd file .service
install(FILES ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}.service DESTINATION /etc/systemd/system/)

# Установка библиотек
install(FILES ${CMAKE_SOURCE_DIR}/bin/libcomutils.a DESTINATION /usr/lib/)
install(FILES ${CMAKE_SOURCE_DIR}/bin/libudp.a DESTINATION /usr/lib/)

# Install the binary program
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin/)

# make uninstall
add_custom_target("uninstall" COMMENT "Uninstall daemon files")
add_custom_command(
        TARGET "uninstall"
        POST_BUILD
        COMMENT "Uninstall files with install_manifest.txt"
        COMMAND xargs rm -vf < install_manifest.txt || echo Nothing in install_manifest.txt to be uninstalled!
)
