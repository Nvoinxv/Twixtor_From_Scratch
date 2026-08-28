#include "decoder_video.hpp"
#include "Optical_Flow_Differention.hpp"
#include "Iterasi_Jendela.hpp"
#include "Interpolasi_posisi_pixel.hpp"
#include "Resampling_Bilinear.hpp"
#include "Kurva_Bezier_Kubik.hpp"
#include "upload_video.hpp"
#include <iostream>
#include <string>
#include <vector>

void print_vector(const std::vector<float>& data) {
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "MEMULAI PROGRAM" << std::endl;

    // Inisialisasi decoder video
    Decoder_Video decoder;

    int lebar = 3840;
    int tinggi = 2160;
    decoder.atur_dimensi(lebar, tinggi);

    // Simulasi frame dari decoder
    int x = 250;
    int y = 150;

    Frame_Video frame1 = decoder.buat_frame(lebar, tinggi, 100);
    Frame_Video frame2 = decoder.buat_frame(lebar, tinggi, 130);

    // Simulasi perubahan intensitas piksel tetangga
    size_t idx_kanan = static_cast<size_t>(y) * lebar + (x + 1);
    size_t idx_kiri  = static_cast<size_t>(y) * lebar + (x - 1);
    if (idx_kanan < frame1.bidang_Y.size()) frame1.bidang_Y[idx_kanan] = 110;
    if (idx_kiri < frame1.bidang_Y.size())  frame1.bidang_Y[idx_kiri]  = 90;

    // 1. Hitung Optical Flow
    Optical_Flow_Differention OFD;
    OFD.hitung_turunan(frame1, frame2, x, y);

    std::cout << "Ix (Gradient X) = ";
    print_vector(OFD.mendapatkan_Ix());

    std::cout << "Iy (Gradient Y) = ";
    print_vector(OFD.mendapatkan_Iy());

    std::cout << "It (Gradient T) = ";
    print_vector(OFD.mendapatkan_It());

    // 2. Perhitungan Algoritma Lucas Kanade
    Algoritma_Lucas_Kanade ALK(OFD, decoder);
    ALK.Operasi_Matrix(x, y);

    std::cout << "Ukuran VX: " << ALK.mendapatkan_vx().size() << std::endl;
    std::cout << "Ukuran VY: " << ALK.mendapatkan_vy().size() << std::endl;

    std::cout << "Hasil dari VX: ";
    print_vector(ALK.mendapatkan_vx());

    std::cout << "Hasil dari VY: ";
    print_vector(ALK.mendapatkan_vy());

    // 3. Interpolasi Posisi Piksel
    Interpolasi_Posisi interpolasi(OFD, decoder);
    float alpha = 0.5f;
    int index_pixel = y * lebar + x;
    Posisi_2D pos_backward = interpolasi.metode_backward(
        static_cast<float>(x),
        static_cast<float>(y),
        alpha,
        ALK.mendapatkan_vx(),
        ALK.mendapatkan_vy(),
        index_pixel
    );

    std::cout << "Posisi Backward (X, Y): " 
              << pos_backward.x_source << ", " 
              << pos_backward.y_source << std::endl;

    // 4. Bilinear Resampling menggunakan frame decoder
    Bilinear_resampling BR(OFD, decoder);
    Posisi_derifatif hasil_resampling = BR.resampling(
        static_cast<float>(x),
        static_cast<float>(y),
        frame1
    );

    std::cout << "Hasil Resampling Bilinear: " 
              << hasil_resampling.turunan_x << std::endl;

    // 5. Simulasi Upload Video
    Upload_Video upload;
    std::string nama_video;

    return 0;
}