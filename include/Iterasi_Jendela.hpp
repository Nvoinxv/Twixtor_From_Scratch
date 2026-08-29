#ifndef ITERASI_JENDELA_HPP
#define ITERASI_JENDELA_HPP

#include "Optical_Flow_Differention.hpp"
#include "decoder_video.hpp"

#include <cmath>
#include <iostream>
#include <vector>

class Algoritma_Lucas_Kanade {
private:
    Optical_Flow_Differention OFD;
    Decoder_Video& Decoder;

    std::vector<float> Ix;
    std::vector<float> Iy;
    std::vector<float> It;

    std::vector<float> vx;
    std::vector<float> vy;

    int x;
    int y;

public:
    Algoritma_Lucas_Kanade(
        Optical_Flow_Differention& ofd,
        Decoder_Video& decoder
    );

    float penjumlahan_Ix_pangkat_2();

    float penjumlahan_Ix_Iy();

    float penjumlahan_Iy_pangkat_2();

    float penjumlahan_It_Ix();

    float penjumlahan_It_Iy();

    void Operasi_Matrix(int x, int y);

    const std::vector<float>& mendapatkan_vx() const {
        return vx;
    }

    const std::vector<float>& mendapatkan_vy() const {
        return vy;
    }
};

#endif