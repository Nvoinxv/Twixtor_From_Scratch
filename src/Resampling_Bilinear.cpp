#include "Resampling_Bilinear.hpp"

Bilinear_resampling::Bilinear_resampling(Optical_Flow_Differention& OFD,
int lebar,
int tinggi) : interpolasi_posisi(OFD, lebar, tinggi), x(0.0f), y(0.0f),
delta_x(0.0f), delta_y(0.0f), pixel(lebar, tinggi) {
    // kosong
}

Posisi_derifatif Bilinear_resampling::resampling(
    float x,
    float y,
) {
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

    float y0 = std::floor(y_source);
    float x0 = std::floor(x_source);

    // Perhitungan delta
    delta_X = x_source - x0;
    delta_y = y_akir - y0;

    float I00 = pixel.mendapatkan_pixel_2d(x0,     y0);
    float I10 = pixel.mendapatkan_pixel_2d(x0 + 1, y0);
    float I01 = pixel.mendapatkan_pixel_2d(x0,     y0 + 1);
    float I11 = pixel.mendapatkan_pixel_2d(x0 + 1, y0 + 1);

    float hasil =
    (1.0f - delta_x) * (1.0f - delta_y) * I00
    + delta_x * (1.0f - delta_y) * I10
    + (1.0f - delta_x) * delta_y * I01
    + delta_x * delta_y * I11;
}