#ifndef OPTICAL_FLOW_DIFFERENTION_HPP
#define OPTICAL_FLOW_DIFFERENTION_HPP

#include "decoder_video.hpp"
#include <vector>

class Optical_Flow_Differention {
private:
    std::vector<float> Ix;
    std::vector<float> Iy;
    std::vector<float> It;

    float mendapatkan_luma(
        const AVFrame* frame,
        int x,
        int y
    ) const;

public:
    Optical_Flow_Differention();
    
    void hitung_turunan(
        const AVFrame* frame1,
        const AVFrame* frame2,
        int x,
        int y
    );

    const std::vector<float> mendapatkan_Ix() const { return Ix; }
    const std::vector<float> mendapatkan_Iy() const { return Iy; }
    const std::vector<float> mendapatkan_It() const { return It; }
};

#endif