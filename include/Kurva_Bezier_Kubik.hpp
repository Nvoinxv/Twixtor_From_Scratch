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

    float y1;
    float y2;
    float y3;
    float y0;

    float x0;
    float x1;
    float x2;
    float x3;
    
    public:
    Persamaan_Bazier();

    float Qubic_Bazier();

    float Turunan_xt_dt();

    float Iterasi_Newton();

    Posisi_Turunan_Bazier Turunan_Velocity_Kurva_Bazier();
};

#endif