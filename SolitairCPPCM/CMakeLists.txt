# CMakeList.txt : CMake project for SolitairCPPCM, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


set(CUDAToolkit_ROOT "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v13.0")
project ("SolitairCPPCM" LANGUAGES CXX CUDA)
#set(CMAKE_INSTALL_RPATH "$ORIGIN;${TORCH_INSTALL_PREFIX}/lib")
set(CMAKE_PREFIX_PATH "C:/Users/jonat/source/repos/SolitairCPPCM/External/libtorch")
find_package(Torch REQUIRED)
add_executable(SolitairCPPCM "${MY_SOURCES}")
# Add source to this project's executable.
target_link_libraries(${PROJECT_NAME} ${TORCH_LIBRARIES})
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/")
target_include_directories(SolitairCPPCM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SolitairCPPCM PROPERTY CXX_STANDARD 20)
endif()
set(TORCH_DLL_DIR "C:/Users/jonat/source/repos/SolitairCPPCM/External/libtorch/lib")
file(GLOB TORCH_DLLS "${TORCH_DLL_DIR}/*.dll")
message(STATUS "Found DLLs: ${TORCH_DLLS}")
foreach(DLL ${TORCH_DLLS})
    add_custom_command(TARGET SolitairCPPCM POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DLL}"
        $<TARGET_FILE_DIR:SolitairCPPCM>
    )
endforeach()
# TODO: Add tests and install targets if needed.
