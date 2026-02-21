cmake_minimum_required (VERSION 3.30)

project("mjsonx" LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULES_DIR ${CMAKE_BINARY_DIR}/modules)
set(CMAKE_CXX_MODULE_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_MODULE_EXTENSIONS ".ixx")  # 本项目只有ixx，所以就写这个了
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Build compile_commands.json

if (NOT CMAKE_GENERATOR MATCHES "Ninja")
  message(WARNING "C++20 modules require Ninja generator! Forcing Ninja...")
  set(CMAKE_GENERATOR "Ninja" CACHE STRING "Build generator" FORCE)
endif()

target_sources(${PROJECT_NAME}
  PUBLIC
  FILE_SET modules
  TYPE CXX_MODULES
  BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/module
  FILES
  "module/types.ixx"
  "module/utils.ixx"
  "module/mjsonx.ixx"
  "module/lexer.ixx"
  "module/parser.ixx"
)

target_sources(${PROJECT_NAME}
  PRIVATE
  "source/mjsonx.cpp"
)

if (MSVC)
  if (POLICY CMP0141) # 为 MSVC 启用热重载
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT 
      "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
  endif()
  
  target_compile_options(${PROJECT_NAME} PRIVATE
    /W4
    /utf-8
    /D _CRT_SECURE_NO_WARNINGS
    /EHsc
    /experimental:module
  )
  # Debug模式额外参数
  target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:/Zi /Od>
  )

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Werror=return-type
    -stdlib=libc++
    -std=c++20
    -fmodules
    -fbuiltin-module-map
    -fmodules-cache-path=${CMAKE_BINARY_DIR}/clang_module_cache
    -fmodule-map-file=/usr/include/c++/v1/module.modulemap
    -I/usr/include/c++/v1
    $<$<CONFIG:Debug>:-O0 -g -fsanitize=address>
    $<$<CONFIG:Release>:-O3 -march=native>
  )
  target_link_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address>
    -stdlib=libc++
  )

elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Werror=return-type
    -fmodules-ts
    $<$<CONFIG:Debug>:-O0 -g -fsanitize=address>
    $<$<CONFIG:Release>:-O3 -march=native -flto>
  )
  # GCC链接选项
  target_link_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address>
    $<$<CONFIG:Release>:-flto>
  )
endif()

if (UNIX)
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    RESULT_VARIABLE symlink_result
    ERROR_QUIET
  )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_BINARY_DIR}/module_interface
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/module  
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source
)

# 测试程序配置 这里只写了 Clang 部分
add_executable(test_demo test/demo1.cpp)
target_link_libraries(test_demo PRIVATE ${PROJECT_NAME})
target_compile_options(test_demo PRIVATE
    -stdlib=libc++ -std=c++20
    -fmodules -fbuiltin-module-map
    -fmodules-cache-path=${CMAKE_BINARY_DIR}/clang_module_cache
    -fmodule-map-file=/usr/include/c++/v1/module.modulemap
    -I/usr/include/c++/v1
)
target_link_options(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Debug>:-fsanitize=address>
  -stdlib=libc++
  -L/usr/lib/x86_64-linux-gnu
  -lc++ -lc++abi
)
target_link_options(test_demo PRIVATE
  -stdlib=libc++
  -L/usr/lib/x86_64-linux-gnu
  -lc++ -lc++abi
)
# if(MSVC)
#   target_compile_options(test_demo PRIVATE /experimental:module /EHsc)
#   set(CMAKE_CXX_SCAN_FOR_MODULES ON)
# endif()
