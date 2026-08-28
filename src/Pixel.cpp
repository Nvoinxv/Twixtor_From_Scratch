#include "Pixel.hpp"

Mendapatkan_Pixel::Mendapatkan_Pixel(int w, int h) : lebar(w), tinggi(h), frame() {}

std::optional<RGB> Mendapatkan_Pixel::mendapatkan_pixel_2d(int x, int y) const
{
    if (x < 0 || x >= lebar || y < 0 || y >= tinggi) {
        return std::nullopt;
    }

    RGB rgb;
    
    size_t indeks = (y * lebar + x) * 3;
    
    rgb.R = frame[indeks];
    rgb.G = frame[indeks + 1];
    rgb.B = frame[indeks + 2];

    return rgb;
}

std::optional<RGB> Mendapatkan_Pixel::mendapatkan_pixel_3d(int x, int y, int t) const {
    if (x < 0 || x >= lebar || y < 0 || y >= tinggi) {
        return std::nullopt;
    }

    RGB rgb;

    size_t indeks = [t * tinggi * lebar) + (y * lebar) + x] * 3;

    rgb.R = frame[indeks];
    rgb.G = frame[indeks + 1];
    rgb.B = frame[indeks + 2];

    return rgb;
}