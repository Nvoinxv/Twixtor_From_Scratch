#include "Interpolasi_posisi_pixel.hpp"

Interpolasi_Posisi::Interpolasi_Posisi(
    Optical_Flow_Differention& OFD,
    int lebar,
    int tinggi
)
    : ALK(OFD, lebar, tinggi),
      vx(),
      vy(),
      alpha(0.0f),
      x(0.0f),
      y(0.0f),
      index(0),
      lebar(lebar),
      tinggi(tinggi)
{
}

Posisi_2D Interpolasi_Posisi::metode_backward(
    float x, float y, float alpha, 
    const std::vector<float>& vx, const std::vector<float>& vy,
    int index
) {
    Posisi_2D posisi;

    if (index < 0 || index >= vx.size() || index >= vy.size()) {

        posisi.x_source = x;
        posisi.y_source = y;

        return posisi;
    }

    posisi.x_source = x - alpha * vx[index];
    posisi.y_source = y - alpha * vy[index];
    
    return posisi;
}

Posisi_2D Interpolasi_Posisi::metode_forward(
    float x, float y, float alpha, 
    const std::vector<float>& vx, const std::vector<float>& vy,
    int index
) {
    Posisi_2D posisi;

    if (index < 0 || index >= vx.size() || index >= vy.size()) {
        
        posisi.x_source = x;
        posisi.y_source = y;

        return posisi;
    }
    
    posisi.x_source = x + alpha * vx[index];
    posisi.y_source = y + alpha * vy[index];

    return posisi;
}
