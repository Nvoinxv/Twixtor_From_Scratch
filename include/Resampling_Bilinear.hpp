#ifndef RESAMPLING_BILINEAR_HPP
#define RESAMPLING_BILINEAR_HPP

#include "Interpolasi_posisi_pixel.hpp"
#include "Optical_Flow_Differention.hpp"
#include <cmath>

struct Posisi_derifatif {
    float turunan_x;
    float turunan_y;
};

class Bilinear_resampling {
    private:
    float delta_x;
    float delat_y;
    float x;
    float y;
    int lebar;
    int tinggi;
    Optical_Flow_Differention& OFD;
    Interpolasi_Posisi interpolasi_posisi;
    
    public:
    Bilinear_resampling(Optical_Flow_Differention& OFD,
    int lebar,
    int tinggi);

    Posisi_derifatif resampling(
        float x,
        float y
    );
}

#endif