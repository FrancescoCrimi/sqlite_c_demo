cmake_minimum_required(VERSION 3.24)
project(Sqlite-C-Demo C)

# --- Global Project Settings ---
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Treat all compiler warnings as errors globally
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# --- Dependencies ---
find_package(SQLite3 REQUIRED)

# --- Target Definition ---
add_executable(sqlite_c_demo
    src/main.c
    src/db_ops.c
)

# --- Target Properties & Include Paths ---
# Set MSVC warning level 4 (overrides default /W3 without conflicts)
set_target_properties(sqlite_c_demo PROPERTIES
    MSVC_WARNING_LEVEL Level4
)

# Include header directory
target_include_directories(sqlite_c_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link SQLite3 library
target_link_libraries(sqlite_c_demo PRIVATE SQLite::SQLite3)

# --- Compiler Specific Options ---
# Apply strict warnings only for GCC and Clang (Linux/macOS)
if(NOT MSVC)
    target_compile_options(sqlite_c_demo PRIVATE -Wall -Wextra -pedantic)
endif()
