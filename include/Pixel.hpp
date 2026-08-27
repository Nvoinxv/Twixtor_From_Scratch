#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <vector>

class Mendapatkan_Pixel {
private:
    int lebar;
    int tinggi;

public:
    // Konstruktor untuk inisialisasi ukuran dimensi
    Mendapatkan_Pixel(int w, int h);

    // Mengembalikan indeks 1D tunggal (int)
    int mendapatkan_pixel_2d(int x, int y) const;
    int mendapatkan_pixel_3d(int x, int y, int t) const;
};

#endif 