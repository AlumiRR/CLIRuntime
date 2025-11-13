project(cliruntime_tests LANGUAGES CXX)

add_executable(udp_tests test_udp.cpp)
target_link_libraries(udp_tests gtest_main udp)
target_include_directories(udp_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(comutils_tests test_comutils.cpp)
target_link_libraries(comutils_tests gtest_main comutils)
target_include_directories(comutils_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_include_directories(udp_tests PRIVATE
    ${GTEST_INCLUDE_DIR}
)

target_include_directories(comutils_tests PRIVATE
    ${GTEST_INCLUDE_DIR}
)

add_test(NAME udp_tests COMMAND udp_tests --gtest_repeat=1000)
add_test(NAME comutils_tests COMMAND comutils_tests)

set_tests_properties(udp_tests PROPERTIES TIMEOUT 30)
set_tests_properties(comutils_tests PROPERTIES TIMEOUT 30)