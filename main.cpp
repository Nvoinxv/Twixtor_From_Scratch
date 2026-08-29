#include "Optical_Flow_Differention.hpp"
#include "decoder_video.hpp"
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

    // Melakukan upload pada video 
    Upload_Video upload;
    CURL* curl = nullptr;
    CURLcode res;
    std::string nama_video;

    Decoder_Video decoder;
    
    // Ini hanya berbasis simulasi saja
    // Belum real dipakai! 
    int x = 250;
    int y = 150;
    int t = 130;
    
    int tinggi = 2160;
    int lebar = 3840;

    Frame_Video frame1;
    Frame_Video frame2;

    // Hitung Optical Flow
    Optical_Flow_Differention OFD;
    OFD.hitung_turunan(frame1, frame2, x, y);

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

    return 0;
}