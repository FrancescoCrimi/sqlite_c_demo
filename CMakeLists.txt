cmake_minimum_required(VERSION 3.14)
project(Sqlite-C-Demo C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Trova la libreria SQLite3 di sistema
find_package(SQLite3 REQUIRED)

# Inclusione directory header
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Definizione eseguibile demo
add_executable(sqlite_c_demo
    src/main.c
    src/db_ops.c
)

# Collegamento della libreria SQLite3
target_link_libraries(sqlite_c_demo PRIVATE SQLite::SQLite3)

# Opzioni di compilazione aggiuntive per warning e robustezza
if(MSVC)
    target_compile_options(sqlite_c_demo PRIVATE /W4 /wx)
else()
    target_compile_options(sqlite_c_demo PRIVATE -Wall -Wextra -pedantic)
endif()
