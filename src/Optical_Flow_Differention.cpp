#include "Optical_Flow_Differention.hpp"

Optical_Flow_Differention::Optical_Flow_Differention() : Ix(0.0f), Iy(0.0f), It(0.0f) {
    // Kosong karena hanya konstruktor saja 
}

void Optical_Flow_Differention::hitung_turunan(
    const std::vector<float>& frame1,
    const std::vector<float>& frame2,
    const Mendapatkan_Pixel& indeks_pixel,
    int x,
    int y,
    int t
) {
    // Ini mengambil pendekatan central difference
    // Yang mana rumusnya seperti ini: f'(x) = (f(x+1) - f(x-1)) / 2
    
    // Mengambil nilai optional RGB
    auto indeks_sekarang = indeks_pixel.mendapatkan_pixel_3d(x, y, t);
    auto indeks_masa_depan = indeks_pixel.mendapatkan_pixel_3d(x, y, t + 1);
    
    auto indeks_kanan = indeks_pixel.mendapatkan_pixel_2d(x + 1, y);
    auto indeks_kiri  = indeks_pixel.mendapatkan_pixel_2d(x - 1, y);
    auto indeks_bawah = indeks_pixel.mendapatkan_pixel_2d(x, y + 1);
    auto indeks_atas  = indeks_pixel.mendapatkan_pixel_2d(x, y - 1);
    
    // Ambil nilainya jika ada, atau gunakan nilai default (misal RGB{0,0,0})
    RGB rgb_sekarang = pixel_sekarang.value_or(RGB{0, 0, 0});
    RGB rgb_kanan    = pixel_kanan.value_or(RGB{0, 0, 0});

    Ix.push_back((frame1[indeks_kanan] - frame1[indeks_kiri]) / 2.0f);
    Iy.push_back((frame1[indeks_bawah] - frame1[indeks_atas]) / 2.0f);
    It.push_back((frame2[indeks_masa_depan] - frame1[indeks_sekarang]) / 2.0f);
}