cmake_minimum_required(VERSION 3.24)
project(webgpu_featuretest LANGUAGES CXX)

if(NOT EMSCRIPTEN)
  message(FATAL_ERROR "Configure with emcmake cmake to use Emscripten")
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug or Release" FORCE)
endif()
string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type)
if(NOT build_type STREQUAL "debug" AND NOT build_type STREQUAL "release")
  message(FATAL_ERROR "Use Debug or Release")
endif()

# Every feature gets separate binaries; optional features must remain target-local.
function(add_feature_test name bits)
  set(target "${name}_wasm${bits}")
  add_executable(${target} main_${name}.cpp)
  set_target_properties(${target} PROPERTIES SUFFIX ".js")
  target_compile_definitions(${target} PRIVATE TEST_POINTER_BITS=${bits})
  target_compile_options(${target} PRIVATE
    -m${bits}
    -fno-exceptions
    -Wall
    -Wextra
    -Wconversion
  )
  target_link_options(${target} PRIVATE
    -m${bits}
    -fno-exceptions
    -sENVIRONMENT=web
    -sALLOW_MEMORY_GROWTH=0
  )
  add_dependencies(client ${target})
endfunction()

add_custom_target(client ALL)
add_feature_test(basic 32)
add_feature_test(basic 64)

foreach(file index.html style.css checklist.js runner.html runner.js smoke.js)
  configure_file(html/${file} ${file} COPYONLY)
endforeach()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/build-info.js"
  "window.build_info = {compiler: '${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}', configuration: '${CMAKE_BUILD_TYPE}', standard: 'C++26'};\n")
