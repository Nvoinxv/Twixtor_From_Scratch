#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <vector>

struct RGB {
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

class Mendapatkan_Pixel {
private:
    int lebar;
    int tinggi;
    int indeks;
    std::vector<float> frame;

public:
    // Konstruktor untuk inisialisasi ukuran dimensi
    Mendapatkan_Pixel(int w, int h);

    // Mengembalikan indeks 1D tunggal (int)
    RGB mendapatkan_pixel_2d(int x, int y) const;
    RGB mendapatkan_pixel_3d(int x, int y, int t) const;
};

#endif 