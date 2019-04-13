cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME gameboy-emulator)

project(${PROJECT_NAME})

set(SDL2_LIBDIR "C:/SDL2-2.0.9/i686-w64-mingw32/lib")
set(SDL2_INCLUDE_DIRS "C:/SDL2-2.0.9/i686-w64-mingw32/include")
set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}  -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -mwindows -mconsole")
string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

include_directories("./gb")
include_directories("./gb/cpu")
include_directories("./gb/mmu")
include_directories("./gb/ppu")
include_directories("./gb/apu")
include_directories("./gb/joypad")
include_directories("./gb/serial")
include_directories("./gui")

set(SOURCES main.c)
set(SOURCES ${SOURCES} gb/gb.c gb/screen.c)
set(SOURCES ${SOURCES} gb/cpu/cpu.c gb/cpu/cpu_opcode.c gb/cpu/cpu_opcode8.c gb/cpu/cpu_opcode16.c)
set(SOURCES ${SOURCES} gb/mmu/mmu.c)
set(SOURCES ${SOURCES} gb/ppu/ppu.c)
set(SOURCES ${SOURCES} gui/display.c)

set(HEADERS gb/gb.h log.h gb/screen.h)
set(HEADERS ${HEADERS} gb/apu/apu.h)
set(HEADERS ${HEADERS} gb/cpu/cpu.h gb/cpu/cpu_alu.h gb/cpu/cpu_def.h gb/cpu/cpu_irq.h)
set(HEADERS ${HEADERS} gb/cpu/cpu_opcode.h gb/cpu/cpu_opcode8 gb/cpu/cpu_opcode16 gb/cpu/cpu_registers.h gb/cpu/cpu_utils.h gb/cpu/timer.h)
set(HEADERS ${HEADERS} gb/joypad/joypad.h)
set(HEADERS ${HEADERS} gb/mmu/mmu.h gb/mmu/rom.h)
set(HEADERS ${HEADERS} gb/ppu/ppu.h gb/ppu/ppu_regs.h gb/ppu/ppu_def.h gb/ppu/ppu_fetcher.h gb/ppu/ppu_fifo.h)
set(HEADERS ${HEADERS} gb/serial/serial.h)
set(HEADERS ${HEADERS} gui/display.h)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads)
#find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
