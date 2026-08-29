#include "Iterasi_Jendela.hpp"

Algoritma_Lucas_Kanade::Algoritma_Lucas_Kanade(
    Optical_Flow_Differention& ofd,
    Decoder_Video& decoder
)
    : OFD(ofd),
      Decoder(decoder),
      Ix(),
      Iy(),
      It(),
      vx(),
      vy(),
      x(0),
      y(0)
{
}

float Algoritma_Lucas_Kanade::penjumlahan_Ix_pangkat_2() {
    float hasil = 0.0f;
    Ix = OFD.mendapatkan_Ix();

    for (int i = 0; i < Ix.size(); i++) {
        hasil += Ix[i] * Ix[i];
    };

    return hasil;
}

float Algoritma_Lucas_Kanade::penjumlahan_Ix_Iy() {
    float hasil = 0.0f;
    Ix = OFD.mendapatkan_Ix();
    Iy = OFD.mendapatkan_Iy();

      for (int i = 0; i < Ix.size(); i++) {
        hasil += Ix[i] * Iy[i];
    }

    return hasil;
}

float Algoritma_Lucas_Kanade::penjumlahan_Iy_pangkat_2() {
    float hasil = 0.0f;
    Iy = OFD.mendapatkan_Iy();

    for (int i = 0; i < Iy.size(); i++) {
        hasil += Iy[i] * Iy[i];
    };

    return hasil;
}

float Algoritma_Lucas_Kanade::penjumlahan_It_Ix() {
    float hasil = 0.0f;
    It = OFD.mendapatkan_It();
    Ix = OFD.mendapatkan_Ix();

    for (int i = 0; i < Ix.size(); i++) {
        hasil += Ix[i] * It[i];
    };

    return hasil;
}

float Algoritma_Lucas_Kanade::penjumlahan_It_Iy() {
    float hasil = 0.0f;
    It = OFD.mendapatkan_It();
    Iy = OFD.mendapatkan_Iy();

    for (int i=0; i<Iy.size(); i++) {
        hasil += Iy[i] * It[i];
    };

    return hasil;
}

void Algoritma_Lucas_Kanade::Operasi_Matrix(int x, int y)
{
    float sum_Ix2  = penjumlahan_Ix_pangkat_2();
    float sum_IxIy = penjumlahan_Ix_Iy();
    float sum_Iy2  = penjumlahan_Iy_pangkat_2();

    float sum_IxIt = penjumlahan_It_Ix();
    float sum_IyIt = penjumlahan_It_Iy();

    float a = sum_Ix2;
    float b = sum_IxIy;
    float c = sum_IxIy;
    float d = sum_Iy2;

    float e = -sum_IxIt;
    float f = -sum_IyIt;

    float determinan =
        (a * d) - (b * c);

    std::cout << "Jumlah Ix: "
              << Ix.size()
              << std::endl;

    std::cout << "Jumlah Iy: "
              << Iy.size()
              << std::endl;

    std::cout << "Jumlah It: "
              << It.size()
              << std::endl;

    if (std::abs(determinan) < 0.000001f) {
        return;
    }

    float invers_determinan =
        1.0f / determinan;

    float vx_sekarang =
        invers_determinan *
        ((d * e) - (b * f));

    float vy_sekarang =
        invers_determinan *
        ((a * f) - (c * e));


    int lebar_frame =
        Decoder.mendapatkan_lebar();

    int tinggi_frame =
        Decoder.mendapatkan_tinggi();


    if (x < 0 || x >= lebar_frame ||
        y < 0 || y >= tinggi_frame) {
        return;
    }

    size_t index =
        static_cast<size_t>(y) *
        lebar_frame +
        x;

    size_t ukuran_frame =
        static_cast<size_t>(lebar_frame) *
        tinggi_frame;

    if (vx.size() != ukuran_frame) {
        vx.resize(ukuran_frame, 0.0f);
    }

    if (vy.size() != ukuran_frame) {
        vy.resize(ukuran_frame, 0.0f);
    }

    vx[index] = vx_sekarang;
    vy[index] = vy_sekarang;
}