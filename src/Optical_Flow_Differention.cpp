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
    RGB rgb_sekarang = indeks_sekarang.value_or(RGB{0, 0, 0});
    RGB rgb_masa_depan = indeks_masa_depan.value_or(RGB{0, 0, 0});

    RGB rgb_kanan = indeks_kanan.value_or(RGB{0, 0, 0});
    RGB rgb_kiri = indeks_kiri.value_or(RGB{0,0,0});
    RGB rgb_atas = indeks_atas.value_or(RGB{0, 0, 0});
    RGB rgb_bawah = indeks_bawah.value_or(RGB{0,0,0});

    float I_kanan = indeks_pixel.ke_grayscale(rgb_kanan);
    float I_kiri = indeks_pixel.ke_grayscale(rgb_kiri);
    float I_atas = indeks_pixel.ke_grayscale(rgb_atas);
    float I_bawah = indeks_pixel.ke_grayscale(rgb_bawah);

    float I_sekarang = indeks_pixel.ke_grayscale(rgb_sekarang);
    float I_masa_depan = indeks_pixel.ke_grayscale(rgb_masa_depan);

    Ix.push_back((I_kanan - I_kiri) / 2.0f);
    Iy.push_back((I_bawah - I_atas) / 2.0f);
    It.push_back(I_masa_depan - I_sekarang);
}