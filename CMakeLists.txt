cmake_minimum_required(VERSION 3.20)

project(
    OdisseuORetorno
    VERSION 0.1.0
    DESCRIPTION "Jogo 3D em primeira pessoa inspirado em A Odisseia"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

option(
    ODISSEU_FETCH_RAYLIB
    "Baixar a Raylib automaticamente quando ela nao estiver instalada"
    ON
)

find_package(raylib 6.2 QUIET)

if(NOT raylib_FOUND)
    if(NOT ODISSEU_FETCH_RAYLIB)
        message(FATAL_ERROR
            "Raylib 6.2 nao encontrada. Instale-a ou configure "
            "ODISSEU_FETCH_RAYLIB=ON."
        )
    endif()

    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 6.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(raylib)
endif()

add_executable(odisseu_o_retorno src/main.c)

target_include_directories(
    odisseu_o_retorno
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(odisseu_o_retorno PRIVATE raylib)

if(MSVC)
    target_compile_options(odisseu_o_retorno PRIVATE /W4)
else()
    target_compile_options(
        odisseu_o_retorno
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

add_custom_command(
    TARGET odisseu_o_retorno
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:odisseu_o_retorno>/assets"
    COMMENT "Copiando assets para a pasta do executavel"
)


add_library(
    odisseu_core
    STATIC
        src/core/personagem.c
)

target_include_directories(
    odisseu_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(MSVC)
    target_compile_options(odisseu_core PRIVATE /W4)
else()
    target_compile_options(odisseu_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(odisseu_o_retorno PRIVATE odisseu_core)