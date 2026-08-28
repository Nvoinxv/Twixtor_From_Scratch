#include "Optical_Flow_Differention.hpp"

Optical_Flow_Differention::Optical_Flow_Differention() : Ix(), Iy(), It() {
    // Kosong
}

float Optical_Flow_Differention::mendapatkan_luma(
    const Frame_Video& frame,
    int x,
    int y
) const
{
    if (x < 0 || x >= frame.lebar ||
        y < 0 || y >= frame.tinggi) {
        return 0.0f;
    }

    size_t index =
        static_cast<size_t>(y) *
        frame.lebar +
        x;

    if (index >= frame.bidang_Y.size()) {
        return 0.0f;
    }

    return static_cast<float>(
        frame.bidang_Y[index]
    );
}
void Optical_Flow_Differention::hitung_turunan(
    const Frame_Video& frame1,
    const Frame_Video& frame2,
    int x,
    int y
)
{
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