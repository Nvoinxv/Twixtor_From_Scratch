#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <vector>
#include <optional>

struct RGB {
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

class Mendapatkan_Pixel {
private:
    int lebar;
    int tinggi;
    std::vector<float> frame;

public:
    // Konstruktor untuk inisialisasi ukuran dimensi
    Mendapatkan_Pixel(int w, int h);

    // Getter untuk dimensi frame
    int mendapatkan_lebar() const { return lebar; }
    int mendapatkan_tinggi() const { return tinggi; }

    // Mengembalikan indeks 1D tunggal (int)
    std::optional<RGB> mendapatkan_pixel_2d(int x, int y) const;
    std::optional<RGB> mendapatkan_pixel_3d(int x, int y, int t) const;

    float ke_grayscale(const RGB& rgb) const;
};

#endif 