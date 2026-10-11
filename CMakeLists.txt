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
    -Wall
    -Wextra
    -Wconversion
  )
  target_link_options(${target} PRIVATE
    -m${bits}
    -sENVIRONMENT=web
  )
  if(name MATCHES "^exceptions")
    if(name STREQUAL "exceptions_legacy")
      set(legacy 1)
    else()
      set(legacy 0)
    endif()
    target_compile_options(${target} PRIVATE -fwasm-exceptions -sWASM_LEGACY_EXCEPTIONS=${legacy})
    target_link_options(${target} PRIVATE -fwasm-exceptions -sWASM_LEGACY_EXCEPTIONS=${legacy})
  else()
    target_compile_options(${target} PRIVATE -fno-exceptions)
    target_link_options(${target} PRIVATE -fno-exceptions)
  endif()
  if(name STREQUAL "simd")
    target_compile_options(${target} PRIVATE -msimd128)
  elseif(name STREQUAL "bulk_memory")
    target_compile_options(${target} PRIVATE -mbulk-memory -mbulk-memory-opt)
  elseif(name STREQUAL "stack")
    target_link_options(${target} PRIVATE -sSTACK_SIZE=5mb)
  endif()
  if(name STREQUAL "memory_growth")
    target_link_options(${target} PRIVATE -sALLOW_MEMORY_GROWTH=1 -sINITIAL_MEMORY=16mb -sMAXIMUM_MEMORY=64mb -sABORTING_MALLOC=0)
  else()
    target_link_options(${target} PRIVATE -sALLOW_MEMORY_GROWTH=0)
  endif()
  add_dependencies(client ${target})
endfunction()

add_custom_target(client ALL)
set(feature_tests basic exceptions_legacy exceptions simd bulk_memory memory_growth stack)
foreach(name IN LISTS feature_tests)
  add_feature_test(${name} 32)
  add_feature_test(${name} 64)
endforeach()
# One build-generated list is shared by the checklist, iframe runner and smoke test.
list(JOIN feature_tests "','" feature_names)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/test-list.js" "window.feature_tests = ['${feature_names}'];\n")

foreach(file index.html style.css checklist.js runner.html runner.js smoke.js)
  configure_file(html/${file} ${file} COPYONLY)
endforeach()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/build-info.js"
  "window.build_info = {compiler: '${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}', configuration: '${CMAKE_BUILD_TYPE}', standard: 'C++26'};\n")
