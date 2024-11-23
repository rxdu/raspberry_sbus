cmake_minimum_required(VERSION 3.9)
project(rpi_sbus VERSION 2.1.1)

option(USE_TTY_IMPL_DUMMY "Use dummy implementation of the driver" OFF)

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

if (USE_TTY_IMPL_DUMMY)
  target_compile_definitions(rpi_sbus PUBLIC RPISBUS_TTY_IMPL RPISBUS_TTY_IMPL_DUMMY)
endif ()

# build examples and tests if this is the main project
if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/examples")
  include(CTest)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif ()

add_library(rpi_sbus
    src/decoder/DecoderFSM.cpp
    src/decoder/packet_decoder.c
    src/driver/SBUS.cpp
    src/tty/sbus_low_latency_linux.c
    src/tty/sbus_low_latency_none.c
    src/tty/sbus_tty_dummy.c
    src/tty/sbus_tty_linux.c)
target_include_directories(rpi_sbus PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
set_target_properties(rpi_sbus PROPERTIES C_STANDARD 99 CXX_STANDARD 11)

# old name for compatibility
add_library(sbuslib ALIAS rpi_sbus)

## installation configuration
install(TARGETS rpi_sbus
    EXPORT xmotionTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

install(DIRECTORY include
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
