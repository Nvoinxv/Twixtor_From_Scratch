#include "Optical_Flow_Differention.hpp"

Optical_Flow_Differention::Optical_Flow_Differention() : Ix(0.0f), Iy(0.0f), It(0.0f) {
    // Kosong karena hanya konstruktor saja 
}

float Optical_Flow_Differention::mendapatkan_luma(
    const AVFrame* frame,
    int x,
    int y
) const
{
    if (frame == nullptr) {
        return 0.0f;
    }

    if (x < 0 || x >= frame->width ||
        y < 0 || y >= frame->height) {
        return 0.0f;
    }

    /*
     * Untuk AV_PIX_FMT_YUV420P:
     *
     * data[0] = Y plane
     * linesize[0] = ukuran sebenarnya satu baris
     */

    return static_cast<float>(
        frame->data[0][y * frame->linesize[0] + x]
    );
}

void Optical_Flow_Differention::hitung_turunan(
    const AVFrame* frame1,
    const AVFrame* frame2,
    int x,
    int y
) {
    if (frame1 == nullptr || frame2 == nullptr) {
        return;
    }

    float I_kanan =
        mendapatkan_luma(frame1, x + 1, y);

    float I_kiri =
        mendapatkan_luma(frame1, x - 1, y);


    float I_bawah =
        mendapatkan_luma(frame1, x, y + 1);

    float I_atas =
        mendapatkan_luma(frame1, x, y - 1);


    float I_sekarang =
        mendapatkan_luma(frame1, x, y);

    float I_masa_depan =
        mendapatkan_luma(frame2, x, y);

    float ix =
        (I_kanan - I_kiri) / 2.0f;

    float iy =
        (I_bawah - I_atas) / 2.0f;

    float it =
        I_masa_depan - I_sekarang;

    Ix.push_back(ix);
    Iy.push_back(iy);
    It.push_back(it);
}