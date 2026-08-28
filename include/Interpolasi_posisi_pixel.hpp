#ifndef INTERPOLASI_POSISI_PIXEL_HPP
#define INTERPOLASI_POSISI_PIXEL_HPP

#include "Iterasi_Jendela.hpp"
#include "Optical_Flow_Differention.hpp"

struct Posisi_2D {
    float x_source;
    float y_source;
};

class Interpolasi_Posisi {
    private:
    Algoritma_Lucas_Kanade ALK;
    std::vector<float> vx;
    std::vector<float> vy;
    float alpha;
    float x;
    float y;
    int index;
    int lebar;
    int tinggi;

    public:
    Interpolasi_Posisi(
        Optical_Flow_Differention& OFD, 
        Decoder_Video& decoder
    );
    Posisi_2D metode_backward(float x,
    float y,
    float alpha,
    const std::vector<float>& vx,
    const std::vector<float>& vy,
    int index);

    Posisi_2D metode_forward(float x, float y,
    float alpha, const std::vector<float>& vx, const std::vector<float>& vy,
    int index);

};

#endif
