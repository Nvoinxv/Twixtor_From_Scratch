#include "Resampling_Bilinear.hpp"

Bilinear_resampling::Bilinear_resampling(Optical_Flow_Differention& OFD,
                                         int lebar,
                                         int tinggi) 
    : OFD(OFD), 
      interpolasi_posisi(OFD, lebar, tinggi), 
      x(0.0f), y(0.0f),
      delta_x(0.0f), delta_y(0.0f), 
      lebar(lebar), tinggi(tinggi),
      pixel(lebar, tinggi) {
    // kosong
}

Posisi_derifatif Bilinear_resampling::resampling(
    float x,
    float y
) {
    float alpha = 0.5f;
    int index = static_cast<int>(y) * lebar + static_cast<int>(x);
    
    // Ini versi realnya //
    std::vector<float> vx = OFD.mendapatkan_Ix();
    std::vector<float> vy = OFD.mendapatkan_Iy();

    /*
    // Ini versi dummy //
    std::vector<float> vx(lebar * tinggi, 0.0f);
    std::vector<float> vy(lebar * tinggi, 0.0f);
    */

    Posisi_2D posisi = interpolasi_posisi.metode_backward(
        x,
        y,
        alpha,
        vx,
        vy,
        index
    );

    float x_source = posisi.x_source;
    float y_source = posisi.y_source;

    float x0 = std::floor(x_source);
    float y0 = std::floor(y_source);

    delta_x = x_source - x0;
    delta_y = y_source - y0;

    // Helper lambda untuk mengambil nilai intensitas pixel (grayscale) secara aman
    auto get_pixel_value = [this](int px, int py) -> float {
        auto opt_rgb = pixel.mendapatkan_pixel_2d(px, py);
        if (opt_rgb.has_value()) {
            return pixel.ke_grayscale(opt_rgb.value());
        }
        return 0.0f;
    };

    float I00 = get_pixel_value(static_cast<int>(x0),     static_cast<int>(y0));
    float I10 = get_pixel_value(static_cast<int>(x0) + 1, static_cast<int>(y0));
    float I01 = get_pixel_value(static_cast<int>(x0),     static_cast<int>(y0) + 1);
    float I11 = get_pixel_value(static_cast<int>(x0) + 1, static_cast<int>(y0) + 1);

    // Hasil perhitungan interpolasi bilinear
    float nilai_interpolasi =
        (1.0f - delta_x) * (1.0f - delta_y) * I00
      + delta_x * (1.0f - delta_y) * I10
      + (1.0f - delta_x) * delta_y * I01
      + delta_x * delta_y * I11;

    Posisi_derifatif hasil;
    hasil.turunan_x = nilai_interpolasi;
    hasil.turunan_y = nilai_interpolasi;

    return hasil;
}