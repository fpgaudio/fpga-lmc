cmake_minimum_required(VERSION 3.16.3)

project(fpga-lmc VERSION "1.0.0" LANGUAGES C)

# LeapSDK
# -------
set(ULTRALEAP_PATH_ROOT "$ENV{ProgramFiles}/Ultraleap")
find_package(LeapSDK 5 REQUIRED PATHS "${ULTRALEAP_PATH_ROOT}")
get_target_property(LEAPC_IMPORTED_CONFIG LeapSDK::LeapC IMPORTED_CONFIGURATIONS)
get_target_property(LEAPC_SHARED_LIB_PATH LeapSDK::LeapC IMPORTED_LOCATION_${LEAPC_IMPORTED_CONFIG})

# libExampleConnection
# --------------------
add_library(
	libExampleConnection
	OBJECT
	"lib/LeapSDK/samples/ExampleConnection.c" "include/serial_device.h")
target_include_directories(libExampleConnection PRIVATE "lib/LeapSDK/samples")
target_link_libraries(libExampleConnection PUBLIC LeapSDK::LeapC)

# Our binary
# ----------
add_executable(fpga-lmc "src/fpga-lmc.c" "src/leap_math.c" "src/cli.c" "src/win_ex.c" "src/serial_device.c" "src/lmc.c")
if(MSVC)
  target_compile_options(fpga-lmc PRIVATE /W4 /WX /wd4201 /D_CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(fpga-lmc PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
target_include_directories(fpga-lmc PRIVATE "include" "lib/LeapSDK/samples")
target_link_libraries(fpga-lmc PRIVATE LeapSDK::LeapC)
add_custom_command(TARGET fpga-lmc POST_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy
	${LEAPC_SHARED_LIB_PATH}
	$<TARGET_FILE_DIR:fpga-lmc>)
target_link_libraries(fpga-lmc PUBLIC libExampleConnection)
