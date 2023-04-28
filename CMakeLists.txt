cmake_minimum_required(VERSION 3.16.3)

project(fpga-lmc VERSION "0.1.0" LANGUAGES C)

# Constants
if (WIN32)
    set(ULTRALEAP_PATH_ROOT
        "$ENV{ProgramFiles}/Ultraleap")
elseif (UNIX)
    set(ULTRALEAP_PATH_ROOT
        "/usr/share/doc/ultraleap-hand-tracking-service/lib/cmake/LeapSDK")
endif()

message("${ULTRALEAP_PATH_ROOT}")

# Finding Packages
find_package(LeapSDK 5 REQUIRED PATHS "${ULTRALEAP_PATH_ROOT}")
if (UNIX)
    find_package(Threads REQUIRED)
endif(UNIX)

# Main Executable
add_executable(fpga-lmc "src/fpga-lmc.c")
target_link_libraries(fpga-lmc PRIVATE LeapSDK::LeapC)
