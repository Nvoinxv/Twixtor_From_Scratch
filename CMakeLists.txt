cmake_minimum_required(VERSION 3.14)
project(Twixtor_From_Scratch_Cpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Cari libCURL untuk keperluan upload_video
find_package(CURL REQUIRED)

# Daftarkan seluruh file .cpp secara eksplisit
add_executable(${PROJECT_NAME}
    main.cpp
    src/Pixel.cpp
    src/Optical_Flow_Differention.cpp
    src/Iterasi_Jendela.cpp
    src/upload_video.cpp
    src/Resampling_Bilinear.cpp
    src/Kurva_Bezier_Kubik.cpp
    src/Interpolasi_posisi_pixel.cpp
)

# Masukkan folder include DAN src agar header (.hpp) terdeteksi
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libCURL ke executable
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)