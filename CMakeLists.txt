cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME gameboy-emulator)

project(${PROJECT_NAME})

set(SOURCES main.c cpu.c cpu_opcode16.c cpu_opcode8.c cpu_opcode.c mmu.c ppu.c)
set(HEADERS cpu.h cpu_registers.h cpu_opcode16.h cpu_opcode8.h cpu_opcode.h cpu_base.h cpu_alu.h intc.h joypad.h lcd.h mmu.h ppu_base.h ppu_fetcher.h ppu.h rom.h serial.h timer.h)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(SDL2 REQUIRED)
find_package(Threads)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Threads::Threads)
