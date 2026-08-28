#include "Resampling_Bilinear.hpp"

Bilinear_resampling::Bilinear_resampling(
    Optical_Flow_Differention& ofd,
    Decoder_Video& decoder
) 
    : OFD(ofd), 
      Decoder(decoder),
      interpolasi_posisi(ofd, decoder), 
      x(0.0f), y(0.0f),
      delta_x(0.0f), delta_y(0.0f), 
      lebar(decoder.mendapatkan_lebar()), 
      tinggi(decoder.mendapatkan_tinggi()) 
{
    // Kosong
}

float Bilinear_resampling::ambil_luma_aman(
    const Frame_Video& frame,
    int px,
    int py
) const {
    if (frame.bidang_Y.empty() || frame.lebar <= 0 || frame.tinggi <= 0) {
        return 0.0f;
    }

    // Mencegah dead frame dengan membatasi koordinat piksel
    int cx = std::clamp(px, 0, frame.lebar - 1);
    int cy = std::clamp(py, 0, frame.tinggi - 1);

    size_t idx = static_cast<size_t>(cy) * frame.lebar + cx;
    if (idx < frame.bidang_Y.size()) {
        return static_cast<float>(frame.bidang_Y[idx]);
    }

    return 0.0f;
}

Posisi_derifatif Bilinear_resampling::resampling(
    float x,
    float y,
    const Frame_Video& frame
) {
    float alpha = 0.5f;
    int index = static_cast<int>(y) * lebar + static_cast<int>(x);

    std::vector<float> vx = OFD.mendapatkan_Ix();
    std::vector<float> vy = OFD.mendapatkan_Iy();

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

    // Mengambil nilai intensitas piksel dari decoder_video tanpa dead frame
    float I00 = ambil_luma_aman(frame, static_cast<int>(x0),     static_cast<int>(y0));
    float I10 = ambil_luma_aman(frame, static_cast<int>(x0) + 1, static_cast<int>(y0));
    float I01 = ambil_luma_aman(frame, static_cast<int>(x0),     static_cast<int>(y0) + 1);
    float I11 = ambil_luma_aman(frame, static_cast<int>(x0) + 1, static_cast<int>(y0) + 1);

    // Interpolasi bilinear
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