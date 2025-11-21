cmake_minimum_required (VERSION 3.8)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


project ("Solitaire" LANGUAGES CXX)

add_executable(Solitaire "${MY_SOURCES}" "include/DummyPlayer.h")
# Add source to this project's executable.
target_link_libraries(${PROJECT_NAME} ${TORCH_LIBRARIES})
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./Resources/")
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC TopCard=10)
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC LastCol=3)
target_include_directories(Solitaire PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Solitaire PROPERTY CXX_STANDARD 20)
endif()
