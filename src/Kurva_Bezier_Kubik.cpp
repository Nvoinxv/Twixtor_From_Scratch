#include "Kurva_Bezier_Kubik.hpp"
#include <algorithm>

Persamaan_Bazier::Persamaan_Bazier()
    : t(0.0f), indeks(0), x_target(0.0f), n(10),
      x0(0.0f), x1(0.0f), x2(1.0f), x3(1.0f),
      y0(0.0f), y1(0.0f), y2(1.0f), y3(1.0f)
{
    // Kosong
}

Persamaan_Bazier::Persamaan_Bazier(float p1x, float p1y, float p2x, float p2y)
    : t(0.0f), indeks(0), x_target(0.0f), n(10),
      x0(0.0f), x1(p1x), x2(p2x), x3(1.0f),
      y0(0.0f), y1(p1y), y2(p2y), y3(1.0f)
{
    // Inisialisasi kurva Bezier ala After Effects
}

void Persamaan_Bazier::atur_titik_kontrol(
    float p0x, float p0y,
    float p1x, float p1y,
    float p2x, float p2y,
    float p3x, float p3y
) {
    this->x0 = p0x; this->y0 = p0y;
    this->x1 = p1x; this->y1 = p1y;
    this->x2 = p2x; this->y2 = p2y;
    this->x3 = p3x; this->y3 = p3y;
}

void Persamaan_Bazier::atur_parameter(float t_val, float x_tgt, int max_iter) {
    this->t = t_val;
    this->x_target = x_tgt;
    this->n = max_iter;
}

float Persamaan_Bazier::Qubic_Bazier() {
    float u = 1.0f - t;
    float fungsi_x_t = (u * u * u * x0) +
                       (3.0f * u * u * t * x1) + 
                       (3.0f * u * t * t * x2) +
                       (t * t * t * x3);

    return fungsi_x_t;
}

float Persamaan_Bazier::hitung_y_dari_t(float t_val) {
    float u = 1.0f - t_val;
    float fungsi_y_t = (u * u * u * y0) +
                       (3.0f * u * u * t_val * y1) + 
                       (3.0f * u * t_val * t_val * y2) +
                       (t_val * t_val * t_val * y3);

    return fungsi_y_t;
}

float Persamaan_Bazier::Turunan_xt_dt() {
    float u = 1.0f - t;
    float turunan_xt_dt = 3.0f * (u * u) * (x1 - x0) + 
                          6.0f * u * t * (x2 - x1) + 
                          3.0f * (t * t) * (x3 - x2);

    return turunan_xt_dt;
}

float Persamaan_Bazier::Iterasi_Newton() {
    float t_curr = std::clamp(x_target, 0.0f, 1.0f);

    for (int i = 0; i < n; i++) {
        this->t = t_curr;
        float x_t = Qubic_Bazier();
        float x_turunan = Turunan_xt_dt();

        if (std::abs(x_turunan) < 1e-6f) break;

        float delta = (x_t - x_target) / x_turunan;
        t_curr = std::clamp(t_curr - delta, 0.0f, 1.0f);

        if (std::abs(delta) < 1e-5f) break;
    }

    this->t = t_curr;
    return t_curr;
}

float Persamaan_Bazier::hitung_interpolasi_waktu(float x_progress) {
    this->x_target = std::clamp(x_progress, 0.0f, 1.0f);
    float t_solved = Iterasi_Newton();
    return hitung_y_dari_t(t_solved);
}

Posisi_Turunan_Bazier Persamaan_Bazier::Turunan_Velocity_Kurva_Bazier() {
    float u = 1.0f - t;
    Posisi_Turunan_Bazier velocity;

    velocity.dxdt = 3.0f * (u * u) * (x1 - x0) +
                    6.0f * u * t * (x2 - x1) +
                    3.0f * (t * t) * (x3 - x2);

    velocity.dydt = 3.0f * (u * u) * (y1 - y0) +
                    6.0f * u * t * (y2 - y1) + 
                    3.0f * (t * t) * (y3 - y2);

    return velocity;
}