cmake_minimum_required(VERSION 3.22)
project(
  dumbutf
  VERSION 0.1.0
  LANGUAGES C CXX)

option(DUMBUTF_BUILD_TESTS "dumbutf tests." ${PROJECT_IS_TOP_LEVEL})
option(
  DUMBUTF_BUILD_TESTS_SLOW
  "dumbutf tests ... but all codepoints (required: simdutf and utf8proc to test)."
  OFF)

option(DUMBUTF_CLI "dumbutf cli (just for small testing)" ${PROJECT_IS_TOP_LEVEL})

set(DUMBUTF_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib/UTF.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/lib/CharWidth.cpp)

add_library(dumbutf SHARED ${DUMBUTF_SOURCES})
add_library(dumbutf::dumbutf ALIAS dumbutf)

target_compile_features(dumbutf PRIVATE cxx_std_20)
set_target_properties(dumbutf PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS
                                                                  OFF)
target_include_directories(
  dumbutf
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>)

if(DUMBUTF_BUILD_TESTS)
  enable_testing()

  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.18.0)
  FetchContent_MakeAvailable(googletest)

  set(DUMBUTF_UNITTESTS_SOURCES
              ${CMAKE_CURRENT_SOURCE_DIR}/unittests/UTF/IterateTests.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/unittests/CharWidthTests.cpp
  )

  add_executable(dumbutf_unittests ${DUMBUTF_UNITTESTS_SOURCES})
  target_compile_features(dumbutf_unittests PRIVATE cxx_std_23)
  target_link_libraries(dumbutf_unittests dumbutf)
  target_include_directories(dumbutf_unittests
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/unittests)
  target_link_libraries(dumbutf_unittests GTest::gtest_main GTest::gmock)

  if(DUMBUTF_BUILD_TESTS_SLOW)
    target_compile_definitions(dumbutf_unittests PRIVATE DUMBUTF_TESTS_FULL=1)

    find_package(simdutf REQUIRED CONFIG)
    target_link_libraries(dumbutf_unittests simdutf)

    add_library(utf8proc STATIC ${CMAKE_CURRENT_SOURCE_DIR}/utf8proc/utf8proc.c)
    target_include_directories(utf8proc
                               PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utc8proc)
    target_compile_features(utf8proc PRIVATE c_std_99)
    set_target_properties(utf8proc PROPERTIES C_STANDARD_REQUIRED ON
                                              C_EXTENSIONS OFF)

    target_link_libraries(dumbutf_unittests utf8proc)
  endif()

  include(GoogleTest)
  gtest_discover_tests(dumbutf_unittests)
endif()

if(DUMBUTF_CLI)
  add_executable(dumbutf_cli ${CMAKE_CURRENT_SOURCE_DIR}/tests/Main.cpp)
  target_compile_features(dumbutf_cli PRIVATE cxx_std_23)
  set_target_properties(dumbutf PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS
                                                                  OFF)
  include(FetchContent)
  FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        12.2.0)
  FetchContent_MakeAvailable(fmt)

  target_link_libraries(dumbutf_cli PRIVATE dumbutf)
  target_link_libraries(dumbutf_cli PRIVATE fmt::fmt)

  find_package(simdutf REQUIRED CONFIG)
  target_link_libraries(dumbutf_cli PRIVATE simdutf)
endif()
