cmake_minimum_required(VERSION 3.11)
project(cliruntime DESCRIPTION "Простой daemon для Linux, общающийся с клиентом по UDP" LANGUAGES CXX)

set(PROJECT_VERSION "0.4.1")

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
file(GLOB ALLHEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
file(GLOB ALLSOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

#file(GLOB ALLHEADERS ${CMAKE_CURRENT_SOURCE_DIR}/lib/daemonpp/*.hpp) # include_directories озадачивает линковщик

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(${PROJECT_NAME} ${ALLSOURCES} ${ALLHEADERS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# --- Для установки сервиса ---

# Configure .service file
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}.service)
configure_file(${CMAKE_SOURCE_DIR}/bin/systemd/daemonpp.service.in ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}.service)
endif()

# Configure .conf file
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.conf)
configure_file(${CMAKE_SOURCE_DIR}/bin/systemd/daemonpp.conf.in ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}.conf)
endif()

# Install the config file .conf
install(FILES ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}.conf DESTINATION /etc/${PROJECT_NAME}/)

# Install the systemd file .service
install(FILES ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}.service DESTINATION /etc/systemd/system/)

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
