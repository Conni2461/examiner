cmake_minimum_required(VERSION 3.2)

project(examiner LANGUAGES C)

add_library(examiner src/examiner.c)
target_include_directories(examiner PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>
)
set_target_properties(examiner PROPERTIES PUBLIC_HEADER src/examiner.h)

include(GnuInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(
    examinerConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/examinerConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/examiner/cmake"
)

install(TARGETS examiner EXPORT examinerTargets)

install(EXPORT examinerTargets 
        FILE examinerTargets.cmake
        DESTINATION ${CMAKE_INSTALL_DATADIR}/examiner/cmake)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/examinerConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/examiner/cmake"
)
