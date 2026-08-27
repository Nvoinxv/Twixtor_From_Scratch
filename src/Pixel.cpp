#include "Pixel.hpp"

Mendapatkan_Pixel::Mendapatkan_Pixel(int w, int h) : lebar(w), tinggi(h) {}

int Mendapatkan_Pixel::mendapatkan_pixel_2d(int x, int y) const {
    return (y * lebar) + x;
}

int Mendapatkan_Pixel::mendapatkan_pixel_3d(int x, int y, int t) const {
    return (t * tinggi * lebar) + (y * lebar) + x;
}