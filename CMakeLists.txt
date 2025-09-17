cmake_minimum_required(VERSION 3.15)

message("Using Vcpkg")

if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
elseif(DEFINED ENV{VCPKG_INSTALLATION_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
else()
    message(FATAL_ERROR "VCPKG_ROOT or VCPKG_INSTALLATION_ROOT environment variable must be set to the vcpkg root directory")
endif()
if(DEFINED ENV{VCPKG_TARGET_TRIPLET})
    set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_TARGET_TRIPLET}" CACHE STRING "Vcpkg target triplet")
else()
    set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "Vcpkg target triplet")
endif()
message("Using Vcpkg target triplet: ${VCPKG_TARGET_TRIPLET}")
message("Using Vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

set(VCPKG_FEATURE_FLAGS "manifests")

project(ChickenKiller VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE HEADERS include/*.h include/*.hpp)

if(WIN32)
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} resources.rc)
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()

message("Project source files: ${SOURCES}")
message("Project header files: ${HEADERS}")

message("Starting to find packages...")

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(cjson CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE cjson)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)

if(WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE
     $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
     $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
     $<$<CONFIG:Debug>:/ENTRY:mainCRTStartup>
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/icon.ico"
        COMMENT "Copying icon.ico to output directory"
    )
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/res" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res")

