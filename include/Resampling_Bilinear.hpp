#ifndef RESAMPLING_BILINEAR_HPP
#define RESAMPLING_BILINEAR_HPP

#include "Interpolasi_posisi_pixel.hpp"
#include "Optical_Flow_Differention.hpp"
#include "decoder_video.hpp"
#include <cmath>
#include <algorithm>

struct Posisi_derifatif {
    float turunan_x;
    float turunan_y;
};

class Bilinear_resampling {
private:
    float delta_x;
    float delta_y;
    float x;
    float y;
    int lebar;
    int tinggi;
    Optical_Flow_Differention& OFD;
    Decoder_Video& Decoder;
    Interpolasi_Posisi interpolasi_posisi;

public:
    Bilinear_resampling(
        Optical_Flow_Differention& OFD,
        Decoder_Video& decoder
    );

    float ambil_luma_aman(
        const Frame_Video& frame,
        int px,
        int py
    ) const;

    Posisi_derifatif resampling(
        float x,
        float y,
        const Frame_Video& frame
    );
};

#endif