cmake_minimum_required(VERSION 3.13)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools")
include(arm-none-eabi-gcc)


set(PRJ_NAME at32f403a_cli)


project(${PRJ_NAME}
  LANGUAGES ASM C CXX
)


set(EXECUTABLE ${PRJ_NAME}.elf)


file(GLOB SRC_FILES CONFIGURE_DEPENDS
  src/bsp/*.c
  src/bsp/device/*.c
  src/hw/*.c
  src/hw/driver/*.c  

  src/hw/driver/usb/*.c
  src/hw/driver/usb/usb_cdc/*.c

  src/lib/usbd_drivers/src/*.c
)

file(GLOB_RECURSE SRC_FILES_RECURSE CONFIGURE_DEPENDS
  src/ap/*.cpp
  src/ap/*.c
  src/common/*.c    

  src/lib/cmsis/*.c
  src/lib/drivers/*.c
)

add_executable(${EXECUTABLE} 
  src/main.cpp
  src/bsp/startup/startup_at32f403a_407.s

  ${SRC_FILES}
  ${SRC_FILES_RECURSE}
)



target_include_directories(${EXECUTABLE} PRIVATE 
  src  
  src/ap
  src/bsp
  src/bsp/device
  src/common 
  src/common/core
  src/common/hw/include 
  src/hw  
  src/lib  

  src/hw/driver/usb
  src/hw/driver/usb/usb_cdc

  src/lib/cmsis/device
  src/lib/cmsis/include
  src/lib/drivers/inc
  src/lib/usbd_drivers/inc
)

target_compile_definitions(${EXECUTABLE} PRIVATE
  -DAT32F403ACGU7
  )

target_compile_options(${EXECUTABLE} PRIVATE
  -mcpu=cortex-m4
  -mthumb
  -mfpu=fpv4-sp-d16 
  -mfloat-abi=hard

  -fdata-sections
  -ffunction-sections

  -Wall
  
  -g3
  -O0
  )

target_link_options(${EXECUTABLE} PRIVATE
  -T../src/bsp/ldscript/AT32F403A_FLASH.ld
  -mcpu=cortex-m4
  -mthumb
  -mfpu=fpv4-sp-d16 
  -mfloat-abi=hard
  -specs=nano.specs
  -lc
  -lm
  -lstdc++
  -lsupc++
  -lnosys
  -Wl,-Map=${PRJ_NAME}.map,--cref
  -Wl,--gc-sections
  -Xlinker -print-memory-usage -Xlinker
  )

#target_link_libraries( ${EXECUTABLE} PRIVATE
#  ${CMAKE_SOURCE_DIR}/lib/CMSIS/libarm_cortexM4lf_math.a 
#  )

add_custom_command(TARGET ${EXECUTABLE} 
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
    COMMENT "Invoking: Make Binary"
    )

#set_target_properties(${EXECUTABLE} PROPERTIES SUFFIX ".elf")
#add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
#        COMMAND arm-none-eabi-size --format=berkeley --totals "$<TARGET_FILE:${EXECUTABLE}>"
#        COMMENT "Invoking: Cross ARM GNU Print Size")