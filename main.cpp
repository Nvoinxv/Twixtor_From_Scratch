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
    std::cout << "=== MEMULAI PROGRAM TWIXTOR DENGAN DECODER & KURVA BEZIER ===" << std::endl;

    // 1. Input dan Upload Video Riil
    std::string nama_video = "sample_video.mp4";
    Upload_Video uploader;

    std::cout << "Memproses video: " << nama_video << std::endl;

    // 2. Inisialisasi dan Pembacaan Video Decoder
    Decoder_Video decoder;
    bool video_terbuka = decoder.buka_video(nama_video);

    int lebar = 1920;
    int tinggi = 1080;

    if (video_terbuka) {
        lebar = decoder.mendapatkan_lebar();
        tinggi = decoder.mendapatkan_tinggi();
        decoder.demuxing();
    } else {
        std::cout << "Menggunakan konfigurasi frame resolusi (" << lebar << "x" << tinggi << ")" << std::endl;
        decoder.atur_dimensi(lebar, tinggi);
    }

    // 3. Menyiapkan Frame Video dari Decoder
    Frame_Video frame1 = decoder.buat_frame(lebar, tinggi, 100);
    Frame_Video frame2 = decoder.buat_frame(lebar, tinggi, 130);

    // Titik sampel koordinat piksel yang dianalisis
    int target_x = 250;
    int target_y = 150;

    // Variasi intensitas piksel tetangga
    size_t idx_kanan = static_cast<size_t>(target_y) * lebar + (target_x + 1);
    size_t idx_kiri  = static_cast<size_t>(target_y) * lebar + (target_x - 1);
    if (idx_kanan < frame1.bidang_Y.size()) frame1.bidang_Y[idx_kanan] = 110;
    if (idx_kiri < frame1.bidang_Y.size())  frame1.bidang_Y[idx_kiri]  = 90;

    // 4. Kustomisasi Kurva Bezier Kubik (Speed Ramping ala After Effects)
    // Titik kontrol (P0=(0,0), P1=(0.42, 0.0), P2=(0.58, 1.0), P3=(1,1)) untuk transisi ease-in-out
    Persamaan_Bazier kurva_kecepatan(0.42f, 0.0f, 0.58f, 1.0f);

    float progress_waktu = 0.5f; // Target waktu interpolasi (50% antara frame 1 dan 2)
    float alpha_bezier = kurva_kecepatan.hitung_interpolasi_waktu(progress_waktu);

    Posisi_Turunan_Bazier vel = kurva_kecepatan.Turunan_Velocity_Kurva_Bazier();
    float kecepatan_instan = (std::abs(vel.dxdt) > 1e-5f) ? (vel.dydt / vel.dxdt) : 1.0f;

    std::cout << "\n--- INFORMASI KURVA SPEED RAMP (AFTER EFFECTS) ---" << std::endl;
    std::cout << "Progress Waktu Linear (X)  : " << progress_waktu << std::endl;
    std::cout << "Bobot Waktu Bezier (Alpha) : " << alpha_bezier << std::endl;
    std::cout << "Kecepatan Instan (dy/dx)   : " << kecepatan_instan << std::endl;

    // 5. Perhitungan Optical Flow Antar Frame
    Optical_Flow_Differention OFD;
    OFD.hitung_turunan(frame1, frame2, target_x, target_y);

    std::cout << "\n--- HASIL OPTICAL FLOW ---" << std::endl;
    std::cout << "Ix (Gradient X) = ";
    print_vector(OFD.mendapatkan_Ix());

    std::cout << "Iy (Gradient Y) = ";
    print_vector(OFD.mendapatkan_Iy());

    std::cout << "It (Gradient T) = ";
    print_vector(OFD.mendapatkan_It());

    // 6. Estimasi Vektor Gerak Lucas Kanade
    Algoritma_Lucas_Kanade ALK(OFD, decoder);
    ALK.Operasi_Matrix(target_x, target_y);

    std::cout << "\n--- VEKTOR GERAK LUCAS-KANADE ---" << std::endl;
    std::cout << "Ukuran VX: " << ALK.mendapatkan_vx().size() << std::endl;
    std::cout << "Ukuran VY: " << ALK.mendapatkan_vy().size() << std::endl;

    // 7. Interpolasi Posisi Menggunakan Bobot Kurva Bezier
    Interpolasi_Posisi interpolasi(OFD, decoder);
    int index_pixel = target_y * lebar + target_x;

    Posisi_2D pos_backward = interpolasi.metode_backward(
        static_cast<float>(target_x),
        static_cast<float>(target_y),
        alpha_bezier,
        ALK.mendapatkan_vx(),
        ALK.mendapatkan_vy(),
        index_pixel
    );

    std::cout << "\n--- INTERPOLASI POSISI DENGAN KURVA BEZIER ---" << std::endl;
    std::cout << "Posisi Sumber X : " << pos_backward.x_source << std::endl;
    std::cout << "Posisi Sumber Y : " << pos_backward.y_source << std::endl;

    // 8. Bilinear Resampling Bebas Dead Frame
    Bilinear_resampling BR(OFD, decoder);
    Posisi_derifatif hasil_resampling = BR.resampling(
        static_cast<float>(target_x),
        static_cast<float>(target_y),
        frame1
    );

    std::cout << "\n--- SINTESIS FRAME (BILINEAR RESAMPLING) ---" << std::endl;
    std::cout << "Nilai Intensitas Rekonstruksi : " << hasil_resampling.turunan_x << std::endl;

    // 9. Frame Buffering (DPB) dan Konversi Warna YUV ke RGB
    decoder.frame_buffering(frame1);
    decoder.frame_buffering(frame2);

    std::vector<RGB> rgb_frame1 = decoder.color_conversion(frame1);
    std::cout << "Frame berhasil dikonversi ke RGB. Total piksel: " << rgb_frame1.size() << std::endl;

    std::cout << "\n=== PROSES TWIXTOR SELESAI DENGAN SUKSES ===" << std::endl;

    return 0;
}