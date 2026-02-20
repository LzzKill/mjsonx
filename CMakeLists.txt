cmake_minimum_required (VERSION 3.28) # 降低版本兼容VS2022，3.30可能过新

project("mjsonx" 
    VERSION 1.0.0
    DESCRIPTION "JSON lib in C++ using module and template"
    LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET modules
        TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/module
        FILES
        "module/types.ixx"
        "module/utils.ixx"
        "module/mjsonx.ixx"
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
  
  set(CMAKE_CXX_SCAN_FOR_MODULES ON)
  set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
  set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
  
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

  set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_MODULE_DIR "${CMAKE_BINARY_DIR}/module_interface"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
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

# 测试程序配置
add_executable(test_demo test/demo1.cpp)
target_link_libraries(test_demo PRIVATE ${PROJECT_NAME})
# 确保测试程序也能扫描模块
if(MSVC)
  target_compile_options(test_demo PRIVATE /experimental:module /EHsc)
  set(CMAKE_CXX_SCAN_FOR_MODULES ON)
endif()