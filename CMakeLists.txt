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

# Only diagnostics are shared. Feature and architecture flags are explicit below.
set(warning_options
  -Wall
  -Wextra
  -Wconversion
)

# Basic WebAssembly execution

add_executable(basic_wasm32
  main_basic.cpp
)
set_target_properties(basic_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(basic_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(basic_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(basic_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(basic_wasm64
  main_basic.cpp
)
set_target_properties(basic_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(basic_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(basic_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(basic_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

# Browser storage

add_executable(storage_wasm32
  main_storage.cpp
)
set_target_properties(storage_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(storage_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(storage_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(storage_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(storage_wasm64
  main_storage.cpp
)
set_target_properties(storage_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(storage_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(storage_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(storage_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

# Embind and DOM

add_executable(dom_wasm32
  main_dom.cpp
)
set_target_properties(dom_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(dom_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(dom_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(dom_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)
target_link_libraries(dom_wasm32
  PRIVATE embind
)

add_executable(dom_wasm64
  main_dom.cpp
)
set_target_properties(dom_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(dom_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(dom_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(dom_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)
target_link_libraries(dom_wasm64
  PRIVATE embind
)

# Fetch

add_executable(fetch_wasm32
  main_fetch.cpp
)
set_target_properties(fetch_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(fetch_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(fetch_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(fetch_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sFETCH=1
  -sFETCH_SUPPORT_INDEXEDDB=0
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(fetch_wasm64
  main_fetch.cpp
)
set_target_properties(fetch_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(fetch_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(fetch_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(fetch_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sFETCH=1
  -sFETCH_SUPPORT_INDEXEDDB=0
  -sALLOW_MEMORY_GROWTH=0
)

# Preloaded filesystem

add_executable(filesystem_wasm32
  main_filesystem.cpp
)
set_target_properties(filesystem_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(filesystem_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(filesystem_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(filesystem_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/assets/fixture.txt@/fixture.txt"
  -sALLOW_MEMORY_GROWTH=0
)
set_property(TARGET filesystem_wasm32 APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/fixture.txt")

add_executable(filesystem_wasm64
  main_filesystem.cpp
)
set_target_properties(filesystem_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(filesystem_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(filesystem_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(filesystem_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/assets/fixture.txt@/fixture.txt"
  -sALLOW_MEMORY_GROWTH=0
)
set_property(TARGET filesystem_wasm64 APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/fixture.txt")

# WebSocket capability

add_executable(websocket_wasm32
  main_websocket.cpp
)
set_target_properties(websocket_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(websocket_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(websocket_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(websocket_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)
target_link_libraries(websocket_wasm32
  PRIVATE websocket.js
)

add_executable(websocket_wasm64
  main_websocket.cpp
)
set_target_properties(websocket_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(websocket_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(websocket_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(websocket_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)
target_link_libraries(websocket_wasm64
  PRIVATE websocket.js
)

# Legacy Wasm exceptions

add_executable(exceptions_legacy_wasm32
  main_exceptions_legacy.cpp
)
set_target_properties(exceptions_legacy_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(exceptions_legacy_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(exceptions_legacy_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fwasm-exceptions
  -sWASM_LEGACY_EXCEPTIONS=1
)
target_link_options(exceptions_legacy_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fwasm-exceptions
  -sWASM_LEGACY_EXCEPTIONS=1
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(exceptions_legacy_wasm64
  main_exceptions_legacy.cpp
)
set_target_properties(exceptions_legacy_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(exceptions_legacy_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(exceptions_legacy_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fwasm-exceptions
  -sWASM_LEGACY_EXCEPTIONS=1
)
target_link_options(exceptions_legacy_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fwasm-exceptions
  -sWASM_LEGACY_EXCEPTIONS=1
  -sALLOW_MEMORY_GROWTH=0
)

# Standardized Wasm exceptions

add_executable(exceptions_wasm32
  main_exceptions.cpp
)
set_target_properties(exceptions_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(exceptions_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(exceptions_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fwasm-exceptions
  -sWASM_LEGACY_EXCEPTIONS=0
)
target_link_options(exceptions_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fwasm-exceptions
  -sWASM_LEGACY_EXCEPTIONS=0
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(exceptions_wasm64
  main_exceptions.cpp
)
set_target_properties(exceptions_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(exceptions_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(exceptions_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fwasm-exceptions
  -sWASM_LEGACY_EXCEPTIONS=0
)
target_link_options(exceptions_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fwasm-exceptions
  -sWASM_LEGACY_EXCEPTIONS=0
  -sALLOW_MEMORY_GROWTH=0
)

# SIMD128

add_executable(simd_wasm32
  main_simd.cpp
)
set_target_properties(simd_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(simd_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(simd_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  -msimd128
)
target_link_options(simd_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(simd_wasm64
  main_simd.cpp
)
set_target_properties(simd_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(simd_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(simd_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  -msimd128
)
target_link_options(simd_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

# Relaxed SIMD

add_executable(relaxed_simd_wasm32
  main_relaxed_simd.cpp
)
set_target_properties(relaxed_simd_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(relaxed_simd_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(relaxed_simd_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  -mrelaxed-simd
)
target_link_options(relaxed_simd_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(relaxed_simd_wasm64
  main_relaxed_simd.cpp
)
set_target_properties(relaxed_simd_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(relaxed_simd_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(relaxed_simd_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  -mrelaxed-simd
)
target_link_options(relaxed_simd_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

# Bulk memory

add_executable(bulk_memory_wasm32
  main_bulk_memory.cpp
)
set_target_properties(bulk_memory_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(bulk_memory_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(bulk_memory_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  -mbulk-memory
  -mbulk-memory-opt
)
target_link_options(bulk_memory_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(bulk_memory_wasm64
  main_bulk_memory.cpp
)
set_target_properties(bulk_memory_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(bulk_memory_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(bulk_memory_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  -mbulk-memory
  -mbulk-memory-opt
)
target_link_options(bulk_memory_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

# Memory growth

add_executable(memory_growth_wasm32
  main_memory_growth.cpp
)
set_target_properties(memory_growth_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(memory_growth_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(memory_growth_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(memory_growth_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=1
  -sINITIAL_MEMORY=16mb
  -sMAXIMUM_MEMORY=64mb
  -sABORTING_MALLOC=0
)

add_executable(memory_growth_wasm64
  main_memory_growth.cpp
)
set_target_properties(memory_growth_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(memory_growth_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(memory_growth_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(memory_growth_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=1
  -sINITIAL_MEMORY=16mb
  -sMAXIMUM_MEMORY=64mb
  -sABORTING_MALLOC=0
)

# 2 GiB maximum memory; only 16 MiB is allocated at startup.

add_executable(maximum_memory_2gb_wasm32
  main_maximum_memory.cpp
)
set_target_properties(maximum_memory_2gb_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(maximum_memory_2gb_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(maximum_memory_2gb_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(maximum_memory_2gb_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=1
  -sINITIAL_MEMORY=16mb
  -sMAXIMUM_MEMORY=2GB
)

add_executable(maximum_memory_2gb_wasm64
  main_maximum_memory.cpp
)
set_target_properties(maximum_memory_2gb_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(maximum_memory_2gb_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(maximum_memory_2gb_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(maximum_memory_2gb_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=1
  -sINITIAL_MEMORY=16mb
  -sMAXIMUM_MEMORY=2GB
)

# 4 GiB maximum memory; only 16 MiB is allocated at startup.

add_executable(maximum_memory_4gb_wasm32
  main_maximum_memory.cpp
)
set_target_properties(maximum_memory_4gb_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(maximum_memory_4gb_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(maximum_memory_4gb_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(maximum_memory_4gb_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=1
  -sINITIAL_MEMORY=16mb
  -sMAXIMUM_MEMORY=4GB
)

add_executable(maximum_memory_4gb_wasm64
  main_maximum_memory.cpp
)
set_target_properties(maximum_memory_4gb_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(maximum_memory_4gb_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(maximum_memory_4gb_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(maximum_memory_4gb_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=1
  -sINITIAL_MEMORY=16mb
  -sMAXIMUM_MEMORY=4GB
)

# 8 GiB maximum memory; only 16 MiB is allocated at startup.

# No wasm32 target: wasm-ld rejects maximum memory above 4 GiB.
add_executable(maximum_memory_8gb_wasm64
  main_maximum_memory.cpp
)
set_target_properties(maximum_memory_8gb_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(maximum_memory_8gb_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(maximum_memory_8gb_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(maximum_memory_8gb_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=1
  -sINITIAL_MEMORY=16mb
  -sMAXIMUM_MEMORY=8GB
)

# Custom main stack

add_executable(stack_wasm32
  main_stack.cpp
)
set_target_properties(stack_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(stack_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(stack_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(stack_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sSTACK_SIZE=5mb
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(stack_wasm64
  main_stack.cpp
)
set_target_properties(stack_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(stack_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(stack_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(stack_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sSTACK_SIZE=5mb
  -sALLOW_MEMORY_GROWTH=0
)

# Shared memory and atomics

add_executable(atomics_wasm32
  main_atomics.cpp
)
set_target_properties(atomics_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(atomics_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(atomics_wasm32 PRIVATE
  -m32
  ${warning_options}
  -sSHARED_MEMORY=1
  -fno-exceptions
)
target_link_options(atomics_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -sSHARED_MEMORY=1
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(atomics_wasm64
  main_atomics.cpp
)
set_target_properties(atomics_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(atomics_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(atomics_wasm64 PRIVATE
  -m64
  ${warning_options}
  -sSHARED_MEMORY=1
  -fno-exceptions
)
target_link_options(atomics_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -sSHARED_MEMORY=1
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

# Wasm Workers

add_executable(workers_wasm32
  main_workers.cpp
)
set_target_properties(workers_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(workers_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(workers_wasm32 PRIVATE
  -m32
  ${warning_options}
  -sWASM_WORKERS=1
  -fno-exceptions
)
target_link_options(workers_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web,worker
  -sWASM_WORKERS=1
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(workers_wasm64
  main_workers.cpp
)
set_target_properties(workers_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(workers_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(workers_wasm64 PRIVATE
  -m64
  ${warning_options}
  -sWASM_WORKERS=1
  -fno-exceptions
)
target_link_options(workers_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web,worker
  -sWASM_WORKERS=1
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

# Wasm Worker synchronization with pthread support

add_executable(synchronisation_wasm32
  main_synchronisation.cpp
)
set_target_properties(synchronisation_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(synchronisation_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(synchronisation_wasm32 PRIVATE
  -m32
  ${warning_options}
  -sWASM_WORKERS=1
  -pthread
  -fno-exceptions
)
target_link_options(synchronisation_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web,worker
  -sWASM_WORKERS=1
  -pthread
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(synchronisation_wasm64
  main_synchronisation.cpp
)
set_target_properties(synchronisation_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(synchronisation_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(synchronisation_wasm64 PRIVATE
  -m64
  ${warning_options}
  -sWASM_WORKERS=1
  -pthread
  -fno-exceptions
)
target_link_options(synchronisation_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web,worker
  -sWASM_WORKERS=1
  -pthread
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

# Pthread creation

add_executable(pthreads_wasm32
  main_pthreads.cpp
)
set_target_properties(pthreads_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(pthreads_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(pthreads_wasm32 PRIVATE
  -m32
  ${warning_options}
  -pthread
  -fno-exceptions
)
target_link_options(pthreads_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web,worker
  -pthread
  -sEXIT_RUNTIME=1
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(pthreads_wasm64
  main_pthreads.cpp
)
set_target_properties(pthreads_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(pthreads_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(pthreads_wasm64 PRIVATE
  -m64
  ${warning_options}
  -pthread
  -fno-exceptions
)
target_link_options(pthreads_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web,worker
  -pthread
  -sEXIT_RUNTIME=1
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

# Canvas 2D

add_executable(canvas_wasm32
  main_canvas.cpp
)
set_target_properties(canvas_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(canvas_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(canvas_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(canvas_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(canvas_wasm64
  main_canvas.cpp
)
set_target_properties(canvas_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(canvas_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(canvas_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(canvas_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

# WebGL 1

add_executable(webgl_wasm32
  main_webgl.cpp
  webgl_test.cpp
)
set_target_properties(webgl_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgl_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgl_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(webgl_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sMIN_WEBGL_VERSION=1
  -sMAX_WEBGL_VERSION=1
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(webgl_wasm64
  main_webgl.cpp
  webgl_test.cpp
)
set_target_properties(webgl_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgl_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgl_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(webgl_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sMIN_WEBGL_VERSION=1
  -sMAX_WEBGL_VERSION=1
  -sALLOW_MEMORY_GROWTH=0
)

# WebGL 2

add_executable(webgl2_wasm32
  main_webgl2.cpp
  webgl_test.cpp
)
set_target_properties(webgl2_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgl2_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgl2_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(webgl2_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sMIN_WEBGL_VERSION=2
  -sMAX_WEBGL_VERSION=2
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(webgl2_wasm64
  main_webgl2.cpp
  webgl_test.cpp
)
set_target_properties(webgl2_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgl2_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgl2_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(webgl2_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sMIN_WEBGL_VERSION=2
  -sMAX_WEBGL_VERSION=2
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(audio_buffer_wasm32 main_audio_buffer.cpp)
set_target_properties(audio_buffer_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(audio_buffer_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(audio_buffer_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
)
target_link_options(audio_buffer_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(audio_buffer_wasm64 main_audio_buffer.cpp)
set_target_properties(audio_buffer_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(audio_buffer_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(audio_buffer_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
)
target_link_options(audio_buffer_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
)

add_executable(audio_wasm32 main_audio.cpp)
set_target_properties(audio_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(audio_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(audio_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  -pthread
  -sWASM_WORKERS=1
)
target_link_options(audio_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web,worker
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  -pthread
  -sWASM_WORKERS=1
  -sAUDIO_WORKLET=1
)

add_executable(audio_wasm64 main_audio.cpp)
set_target_properties(audio_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(audio_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(audio_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  -pthread
  -sWASM_WORKERS=1
)
target_link_options(audio_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web,worker
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  -pthread
  -sWASM_WORKERS=1
  -sAUDIO_WORKLET=1
)

add_executable(webgpu_wasm32 main_webgpu.cpp)
set_target_properties(webgpu_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_wasm64 main_webgpu.cpp)
set_target_properties(webgpu_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_depth_stencil_wasm32 main_webgpu_depth_stencil.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_depth_stencil_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_depth_stencil_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_depth_stencil_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_depth_stencil_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_depth_stencil_wasm64 main_webgpu_depth_stencil.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_depth_stencil_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_depth_stencil_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_depth_stencil_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_depth_stencil_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_bc_wasm32 main_webgpu_bc.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_bc_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_bc_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_bc_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_bc_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_bc_wasm64 main_webgpu_bc.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_bc_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_bc_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_bc_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_bc_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_indirect_wasm32 main_webgpu_indirect.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_indirect_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_indirect_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_indirect_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_indirect_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_indirect_wasm64 main_webgpu_indirect.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_indirect_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_indirect_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_indirect_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_indirect_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_timestamp_wasm32 main_webgpu_timestamp.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_timestamp_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_timestamp_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_timestamp_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_timestamp_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_timestamp_wasm64 main_webgpu_timestamp.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_timestamp_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_timestamp_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_timestamp_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_timestamp_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_f16_wasm32 main_webgpu_f16.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_f16_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_f16_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_f16_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_f16_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_f16_wasm64 main_webgpu_f16.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_f16_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_f16_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_f16_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_f16_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_float32_filterable_wasm32 main_webgpu_float32_filterable.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_float32_filterable_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_float32_filterable_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_float32_filterable_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_float32_filterable_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_float32_filterable_wasm64 main_webgpu_float32_filterable.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_float32_filterable_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_float32_filterable_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_float32_filterable_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_float32_filterable_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_compute_wasm32 main_webgpu_compute.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_compute_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_compute_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_compute_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_compute_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_compute_wasm64 main_webgpu_compute.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_compute_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_compute_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_compute_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_compute_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_render_wasm32 main_webgpu_render.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_render_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_render_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_render_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_render_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_render_wasm64 main_webgpu_render.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_render_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_render_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_render_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_render_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_worker_wasm32 main_webgpu_worker.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_worker_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_worker_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_worker_wasm32 PRIVATE
  -pthread # emdawnwebgpu uses mutexes; this test creates a Wasm Worker.
  -m32
  ${warning_options}
  -fno-exceptions
  -sWASM_WORKERS=1
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_worker_wasm32 PRIVATE
  -pthread # emdawnwebgpu uses mutexes; this test creates a Wasm Worker.
  -m32
  -sENVIRONMENT=web,worker
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  -sWASM_WORKERS=1
  --use-port=emdawnwebgpu
)

add_executable(webgpu_worker_wasm64 main_webgpu_worker.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_worker_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_worker_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_worker_wasm64 PRIVATE
  -pthread # emdawnwebgpu uses mutexes; this test creates a Wasm Worker.
  -m64
  ${warning_options}
  -fno-exceptions
  -sWASM_WORKERS=1
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_worker_wasm64 PRIVATE
  -pthread # emdawnwebgpu uses mutexes; this test creates a Wasm Worker.
  -m64
  -sENVIRONMENT=web,worker
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  -sWASM_WORKERS=1
  --use-port=emdawnwebgpu
)

add_executable(webgpu_etc2_wasm32 main_webgpu_etc2.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_etc2_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_etc2_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_etc2_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_etc2_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_etc2_wasm64 main_webgpu_etc2.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_etc2_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_etc2_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_etc2_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_etc2_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_astc_wasm32 main_webgpu_astc.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_astc_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_astc_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_astc_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_astc_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(webgpu_astc_wasm64 main_webgpu_astc.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_astc_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_astc_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_astc_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_astc_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -fno-exceptions
  -sALLOW_MEMORY_GROWTH=0
  --use-port=emdawnwebgpu
)

add_executable(exceptions_js_wasm32 main_exceptions_js.cpp)
set_target_properties(exceptions_js_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(exceptions_js_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(exceptions_js_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fexceptions
  -sDISABLE_EXCEPTION_CATCHING=0
)
target_link_options(exceptions_js_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -sALLOW_MEMORY_GROWTH=0
  -fexceptions
  -sDISABLE_EXCEPTION_CATCHING=0
)

add_executable(exceptions_js_wasm64 main_exceptions_js.cpp)
set_target_properties(exceptions_js_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(exceptions_js_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(exceptions_js_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fexceptions
  -sDISABLE_EXCEPTION_CATCHING=0
)
target_link_options(exceptions_js_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -sALLOW_MEMORY_GROWTH=0
  -fexceptions
  -sDISABLE_EXCEPTION_CATCHING=0
)

add_executable(webgpu_indexed_wasm32 main_webgpu_indexed.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_indexed_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_indexed_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_indexed_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_indexed_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -sALLOW_MEMORY_GROWTH=0
  -fno-exceptions
  --use-port=emdawnwebgpu
)

add_executable(webgpu_indexed_wasm64 main_webgpu_indexed.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_indexed_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_indexed_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_indexed_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_indexed_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -sALLOW_MEMORY_GROWTH=0
  -fno-exceptions
  --use-port=emdawnwebgpu
)

add_executable(webgpu_uniform_wasm32 main_webgpu_uniform.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_uniform_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_uniform_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_uniform_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_uniform_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -sALLOW_MEMORY_GROWTH=0
  -fno-exceptions
  --use-port=emdawnwebgpu
)

add_executable(webgpu_uniform_wasm64 main_webgpu_uniform.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_uniform_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_uniform_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_uniform_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_uniform_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -sALLOW_MEMORY_GROWTH=0
  -fno-exceptions
  --use-port=emdawnwebgpu
)

add_executable(webgpu_bundles_wasm32 main_webgpu_bundles.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_bundles_wasm32 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_bundles_wasm32 PRIVATE TEST_POINTER_BITS=32)
target_compile_options(webgpu_bundles_wasm32 PRIVATE
  -m32
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_bundles_wasm32 PRIVATE
  -m32
  -sENVIRONMENT=web
  -sALLOW_MEMORY_GROWTH=0
  -fno-exceptions
  --use-port=emdawnwebgpu
)

add_executable(webgpu_bundles_wasm64 main_webgpu_bundles.cpp webgpu_feature_test.cpp)
set_target_properties(webgpu_bundles_wasm64 PROPERTIES SUFFIX ".js")
target_compile_definitions(webgpu_bundles_wasm64 PRIVATE TEST_POINTER_BITS=64)
target_compile_options(webgpu_bundles_wasm64 PRIVATE
  -m64
  ${warning_options}
  -fno-exceptions
  --use-port=emdawnwebgpu
)
target_link_options(webgpu_bundles_wasm64 PRIVATE
  -m64
  -sENVIRONMENT=web
  -sALLOW_MEMORY_GROWTH=0
  -fno-exceptions
  --use-port=emdawnwebgpu
)

# Build the complete suite with ./build.sh or the client target.
add_custom_target(client ALL DEPENDS
  basic_wasm32 basic_wasm64
  storage_wasm32 storage_wasm64
  dom_wasm32 dom_wasm64
  fetch_wasm32 fetch_wasm64
  filesystem_wasm32 filesystem_wasm64
  websocket_wasm32 websocket_wasm64
  exceptions_legacy_wasm32 exceptions_legacy_wasm64
  exceptions_wasm32 exceptions_wasm64
  exceptions_js_wasm32 exceptions_js_wasm64
  simd_wasm32 simd_wasm64
  relaxed_simd_wasm32 relaxed_simd_wasm64
  bulk_memory_wasm32 bulk_memory_wasm64
  memory_growth_wasm32 memory_growth_wasm64
  maximum_memory_2gb_wasm32 maximum_memory_2gb_wasm64
  maximum_memory_4gb_wasm32 maximum_memory_4gb_wasm64
  maximum_memory_8gb_wasm64
  stack_wasm32 stack_wasm64
  atomics_wasm32 atomics_wasm64
  workers_wasm32 workers_wasm64
  synchronisation_wasm32 synchronisation_wasm64
  pthreads_wasm32 pthreads_wasm64
  canvas_wasm32 canvas_wasm64
  webgl_wasm32 webgl_wasm64
  webgl2_wasm32 webgl2_wasm64
  audio_buffer_wasm32 audio_buffer_wasm64
  audio_wasm32 audio_wasm64
  webgpu_wasm32 webgpu_wasm64
  webgpu_compute_wasm32 webgpu_compute_wasm64
  webgpu_render_wasm32 webgpu_render_wasm64
  webgpu_indexed_wasm32 webgpu_indexed_wasm64
  webgpu_uniform_wasm32 webgpu_uniform_wasm64
  webgpu_bundles_wasm32 webgpu_bundles_wasm64
  webgpu_worker_wasm32 webgpu_worker_wasm64
  webgpu_etc2_wasm32 webgpu_etc2_wasm64
  webgpu_astc_wasm32 webgpu_astc_wasm64
  webgpu_depth_stencil_wasm32 webgpu_depth_stencil_wasm64
  webgpu_bc_wasm32 webgpu_bc_wasm64
  webgpu_indirect_wasm32 webgpu_indirect_wasm64
  webgpu_timestamp_wasm32 webgpu_timestamp_wasm64
  webgpu_f16_wasm32 webgpu_f16_wasm64
  webgpu_float32_filterable_wasm32 webgpu_float32_filterable_wasm64
)

# Feature names used by the browser checklist; no build options are assigned here.
set(feature_tests
  basic
  storage
  dom
  fetch
  filesystem
  websocket
  exceptions_legacy
  exceptions
  exceptions_js
  simd
  relaxed_simd
  bulk_memory
  memory_growth
  maximum_memory_2gb
  maximum_memory_4gb
  maximum_memory_8gb
  stack
  atomics
  workers
  synchronisation
  pthreads
  canvas
  webgl
  webgl2
  audio_buffer
  audio
  webgpu
  webgpu_compute
  webgpu_render
  webgpu_indexed
  webgpu_uniform
  webgpu_bundles
  webgpu_worker
  webgpu_etc2
  webgpu_astc
  webgpu_depth_stencil
  webgpu_bc
  webgpu_indirect
  webgpu_timestamp
  webgpu_f16
  webgpu_float32_filterable
)

configure_file(assets/fixture.txt fixture.txt COPYONLY)
configure_file(assets/social-preview.jpg social-preview.jpg COPYONLY)
# One build-generated list is shared by the checklist, iframe runner and smoke test.
list(JOIN feature_tests "','" feature_names)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/test-list.js" "window.feature_tests = ['${feature_names}'];\n")

foreach(file index.html style.css checklist.js runner.html runner.js smoke.js)
  configure_file(html/${file} ${file} COPYONLY)
endforeach()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/build-info.js"
  "window.build_info = {emscripten: '${EMSCRIPTEN_VERSION}', compiler: '${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}', configuration: '${CMAKE_BUILD_TYPE}', standard: 'C++26'};\n")
