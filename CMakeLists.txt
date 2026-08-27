cmake_minimum_required(VERSION 3.14)
project(Twixtor_From_Scratch_Cpp VERSION 1.0 LANGUAGES CXX)

# Gunakan standar C++17 (bisa disesuaikan ke 11, 14, 20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Cari semua file sumber di folder src
file(GLOB SOURCES "src/*.cpp")

# Daftarkan file eksekusi beserta file main.cpp dan implementasi di src/
add_executable(${PROJECT_NAME}
    main.cpp
    ${SOURCES}
)

# Sertakan direktori header (include/Pixel.hpp)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)