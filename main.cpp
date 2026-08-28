#include "Pixel.hpp"
#include "Optical_Flow_Differention.hpp"
#include "Iterasi_Jendela.hpp"
#include "upload_video.hpp"
#include <iostream>
#include <string>
#include <vector>

void print_vector(const std::vector<float>& data) {
    for (int i = 0; i < data.size(); i++) {
        std::cout << data[i] << " ";
    };
    std::cout << std::endl;
}

int main() {
    std::cout << "MEMULAI PROGRAM" << std::endl;
    
    // Ini hanya berbasis simulasi saja
    // Belum real dipakai! 
    int x = 250;
    int y = 150;
    int t = 130;
    
    int tinggi = 2160;
    int lebar = 3840;

    Mendapatkan_Pixel pixel(lebar, tinggi);
    
    // Dummy frame 
    int total_frame = 150;
    std::vector<float> frame1(lebar * tinggi * total_frame, 100.0f);
    std::vector<float> frame2(lebar * tinggi * total_frame, 130.0f);
    
    auto hasil_pixel = pixel.mendapatkan_pixel_2d(x, y);
    auto hasil_pixel_3d = pixel.mendapatkan_pixel_3d(x, y, t);

    RGB rgb_2d = hasil_pixel.value_or(RGB{0, 0, 0});
    RGB rgb_3d = hasil_pixel_3d.value_or(RGB{0, 0, 0});

    std::cout << "HASIL PIXEL 2D (R,G,B): " 
              << (int)rgb_2d.R << ", " << (int)rgb_2d.G << ", " << (int)rgb_2d.B << std::endl;
    std::cout << "HASIL PIXEL 3D (R,G,B): " 
              << (int)rgb_3d.R << ", " << (int)rgb_3d.G << ", " << (int)rgb_3d.B << std::endl;
    
    // Kita ingin lihat perbedaan pada turunan Ix dan Iy
    // Karena kita menggunakan data dummy tetapi, jika menggunakan gambar/video bisa skip
    int target_x = 250;
    int target_y = 200;

    size_t idx1 = (static_cast<size_t>(target_y) * lebar + (target_x + 1)) * 3;
    size_t idx2 = (static_cast<size_t>(target_y) * lebar + (target_x - 1)) * 3;
    
    float perubahan_pixel_1 = frame1[idx1] = 110.0f;
    float perubahan_pixel_2 = frame1[idx2] = 110.0f;

    float selisih = (perubahan_pixel_1 - perubahan_pixel_2) / 2.0f;

    // Hitung Optical Flow
    Optical_Flow_Differention OFD;
    OFD.hitung_turunan(frame1, frame2, pixel, x, y, t);

    std::cout << "Ix (Gradient X) = ";
    print_vector(OFD.mendapatkan_It());
    std::cout << std::endl;

    std::cout << "Iy (Gradient Y) = ";
    print_vector(OFD.mendapatkan_Iy());
    std::cout << std::endl;

    std::cout << "It (Gradient T) = ";
    print_vector(OFD.mendapatkan_It());
    std::cout << std::endl;

    // Perhitungan Algortima Lucas Kanade
    Algoritma_Lucas_Kanade ALK(OFD, lebar, tinggi);
    ALK.Operasi_Matrix(x, y);

    std::cout << "Ukuran VX: "
        << ALK.mendapatkan_vx().size()
        << std::endl;

    std::cout << "Ukuran VY: "
        << ALK.mendapatkan_vy().size()
        << std::endl;
    
    std::cout << "Hasil dari VX: ";
    print_vector(ALK.mendapatkan_vx());
    std::cout << std::endl;

    std::cout << "Hasil dari VY: ";
    print_vector(ALK.mendapatkan_vy());
    std::cout << std::endl;

    // Melakukan upload pada video 
    Upload_Video upload;
    CURL* curl = nullptr;
    CURLcode res;
    std::string nama_video;

    int lebar_video_twixtor = 1920;
    int tinggi_video_twixtor = 1080;

    Mendapatkan_Pixel video_pixel_twixtor(lebar_video_twixtor, tinggi_video_twixtor);

    return 0;
}