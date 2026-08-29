#ifndef KURVA_BEZIER_KUBIK_HPP
#define KURVA_BEZIER_KUBIK_HPP

#include <cmath>
#include <iostream>

struct Posisi_Turunan_Bazier {
    float dydt;
    float dxdt;
};

class Persamaan_Bazier {
private:
    float t;
    int indeks;
    float x_target;
    int n;

    float y0;
    float y1;
    float y2;
    float y3;

    float x0;
    float x1;
    float x2;
    float x3;
    
public:
    Persamaan_Bazier();
    Persamaan_Bazier(float p1x, float p1y, float p2x, float p2y);

    void atur_titik_kontrol(
        float p0x, float p0y,
        float p1x, float p1y,
        float p2x, float p2y,
        float p3x, float p3y
    );

    void atur_parameter(float t_val, float x_tgt, int max_iter);

    float Qubic_Bazier();
    float hitung_y_dari_t(float t_val);
    float Turunan_xt_dt();
    float Iterasi_Newton();
    float hitung_interpolasi_waktu(float x_progress);

    Posisi_Turunan_Bazier Turunan_Velocity_Kurva_Bazier();
};

#endif