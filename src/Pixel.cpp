#include "Pixel.hpp"

Mendapatkan_Pixel::Mendapatkan_Pixel(int w, int h) : lebar(w), tinggi(h), frame() {}

std::optional<RGB> Mendapatkan_Pixel::mendapatkan_pixel_2d(int x, int y) const
{
    if (x < 0 || x >= lebar || y < 0 || y >= tinggi) {
        return std::nullopt;
    }

    RGB rgb;
    
    size_t indeks = (static_cast<size_t>(y) * lebar + x) * 3;
    
    if (indeks + 2 >= frame.size()) {
        return std::nullopt;
    }

    rgb.R = static_cast<unsigned char>(frame[indeks]);
    rgb.G = static_cast<unsigned char>(frame[indeks + 1]);
    rgb.B = static_cast<unsigned char>(frame[indeks + 2]);

    return rgb;
}

std::optional<RGB> Mendapatkan_Pixel::mendapatkan_pixel_3d(int x, int y, int t) const {
    if (x < 0 || x >= lebar || y < 0 || y >= tinggi) {
        return std::nullopt;
    }

    RGB rgb;

    size_t indeks = ((static_cast<size_t>(t) * tinggi * lebar) + (static_cast<size_t>(y) * lebar) + x) * 3;

    if (indeks + 2 >= frame.size()) {
        return std::nullopt;
    }

    rgb.R = static_cast<unsigned char>(frame[indeks]);
    rgb.G = static_cast<unsigned char>(frame[indeks + 1]);
    rgb.B = static_cast<unsigned char>(frame[indeks + 2]);

    return rgb;
}

float Mendapatkan_Pixel::ke_grayscale(const RGB& rgb) const {
    // Rumus standar luminance: 0.299R + 0.587G + 0.114B
    return 0.299f * rgb.R + 0.587f * rgb.G + 0.114f * rgb.B;
}