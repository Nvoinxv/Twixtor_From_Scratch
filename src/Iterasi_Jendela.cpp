#include "Iterasi_Jendela.hpp"

Algoritma_Lucas_Kanade::Algoritma_Lucas_Kanade(
    Optical_Flow_Differention& ofd,
    int lebar, 
    int tinggi) : Ix(), Iy(), It(), 
vx(), vy(), OFD(ofd), Pixel(lebar, tinggi) {
    // Kosong
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

    float determinan = (a * d) - (b * c);
    
    // Print bug pada Ix, Iy, It
    std::cout << "Jumlah Ix: " << Ix.size() << std::endl;
    std::cout << "Jumlah Iy: " << Iy.size() << std::endl;
    std::cout << "Jumlah It: " << It.size() << std::endl;

    if (std::abs(determinan) < 0.000001f) {
        return;
    }

    float invers_determinan = 1.0f / determinan;

    float vx_sekarang = invers_determinan * ((d * e) - (b * f));
    float vy_sekarang = invers_determinan * ((a * f) - (c * e));

    // Ambil nilai lebar dari member Pixel
    int lebar_frame = Pixel.mendapatkan_lebar(); 

    // Kalkulasi index flat 1D (row-major order)
    size_t index = static_cast<size_t>(y) * lebar_frame + x;

    if (index >= vx.size()) {
        vx.resize(index + 1, 0.0f);
        vy.resize(index + 1, 0.0f);
    }

    vx[index] = vx_sekarang;
    vy[index] = vy_sekarang;
}