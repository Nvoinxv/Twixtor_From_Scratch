#ifndef OPTICAL_FLOW_DIFFERENTION_HPP
#define OPTICAL_FLOW_DIFFERENTION_HPP

#include "Pixel.hpp"
#include <vector>

class Optical_Flow_Differention {
private:
    std::vector<float> Ix;
    std::vector<float> Iy;
    std::vector<float> It;

public:
    Optical_Flow_Differention();
    
    void hitung_turunan(
        const std::vector<float>& frame1,
        const std::vector<float>& frame2,
        const Mendapatkan_Pixel& indeks_pixel,
        int x,
        int y,
        int t
    );

    const std::vector<float> mendapatkan_Ix() const { return Ix; }
    const std::vector<float> mendapatkan_Iy() const { return Iy; }
    const std::vector<float> mendapatkan_It() const { return It; }
};

#endif