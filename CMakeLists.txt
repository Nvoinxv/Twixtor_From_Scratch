cmake_minimum_required(VERSION 3.14)
project(Twixtor_From_Scratch_Cpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Cari libCURL
find_package(CURL REQUIRED)

# 2. Cari FFmpeg menggunakan PkgConfig (cocok untuk Arch Linux)
find_package(PkgConfig REQUIRED)
pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(SWSCALE REQUIRED IMPORTED_TARGET libswscale)

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
    src/decoder_video.cpp
)

# Masukkan folder include DAN src agar header (.hpp) terdeteksi
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${AVCODEC_INCLUDE_DIRS}
    ${AVFORMAT_INCLUDE_DIRS}
    ${AVUTIL_INCLUDE_DIRS}
    ${SWSCALE_INCLUDE_DIRS}
)

# 3. Link libCURL DAN FFmpeg ke executable
target_link_libraries(${PROJECT_NAME} PRIVATE 
    CURL::libcurl
    PkgConfig::AVCODEC
    PkgConfig::AVFORMAT
    PkgConfig::AVUTIL
    PkgConfig::SWSCALE
)