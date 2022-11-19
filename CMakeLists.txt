cmake_minimum_required(VERSION 3.13)

#include(pico_sdk_import.cmake)

set(FAMILY rp2040)
set(BOARD pico_sdk)
set(PICO_SDK_PATH /home/alayalava/git/pico-sdk)
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/${FAMILY}/family.cmake)


project(YMK C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(ymk
  main.c
  usb_descriptors.c
)

pico_enable_stdio_usb(ymk 0)
pico_enable_stdio_uart(ymk 1)

target_include_directories(ymk PRIVATE ${CMAKE_CURRENT_LIST_DIR})

pico_add_extra_outputs(ymk)
target_link_libraries(ymk PRIVATE pico_stdlib tinyusb_device tinyusb_board)