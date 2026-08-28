#include "Kurva_Bezier_Kubik.hpp"

Persamaan_Bazier::Persamaan_Bazier(): t(0.0f), indeks(0),
 x_target(0.0f), n(0), x0(0.0f), x1(0.0f), x2(0.0f), x3(0.0f),
 y0(0.0f), y1(0.0f), y2(0.0f), y3(0.0f) {
    // Kosong
}

float Persamaan_Bazier::Qubic_Bazier() {
    if (t >= 0.0 && t <= 1.0) {
        std::cout << "Angka masih dirange aman!" << std::endl;
        std::cout << "Variabel (t): " << t << std::endl;
    } else {
        std::cout << "Angka sudah diluar range!" << std::endl;
        std::cout << "Variabel (t): " << t << std::endl;
    };
    
    float fungsi_x_t = [([(1.0 - t) * (1.0 - t) * (1 - t)] * x0) +
    (3.0 * (1.0 - t) * (1.0 - t) * t * x1) + (3 * (1.0 - t) * t * t * x2) +
    [(t * t * t) * x3]];

    float fungsi_y_t = [([(1.0 - t) * (1.0 - t) * (1.0 - t)] * y0) +
    (3.0 * (1.0 - t) * (1.0 - t) * t * y1) + (3 * (1.0 - t) * t * t * y2) +
    [(t * t * t) * y3]];
}

float Persamaan_Bazier::Turunan_xt_dt() {
    if (t >= 0.0 && t <= 1.0) {
        std::cout << "Variabel pada waktu aman!" << std::endl;
        std::cout << "Hasil variabel (t): " << t << std::endl;
    } else {
        std::cout << "Variabel diluar range!" << std::endl;
        std::cout << "Hasil variabel (t): " << t << std::endl;
    };

    float turunan_xt_dt = [3.0 * [(1.0 - t) * (1.0 - t)] * x1 + 
    6.0 * (1-t) * t * (x2 - x1) + 3.0 * (t * t) * (1.0 - x2)];

    return turunan_xt_dt;
}

float Persamaan_Bazier::Iterasi_Newton() {
    if (t >= 0.0 && t <= 1.0) {
        std::cout << "Angka berada dirange yang aman!" << std::endl;
        std::cout << "Variabel (t): " << t << std::endl;
    } else {
        std::cout << "Angka berada diluar range!" << std::endl;
        std::cout << "Variabel (t): " << t << std::endl;
    };

    float hasil;
    float x_turunan = Turunan_xt_dt();
    Qubic_Bazier.fungsi_x_t x_t;

    for (int i = 0; i < n + 1; i++) {
        hasil = t[i] - (x_t[i] - x_target[i]) / x_turunan[i];
    };

    return hasil;
}

Posisi_Turunan_Bazier Persamaan_Bazier::Turunan_Velocity_Kurva_Bazier() {
    if (t >= 0.0 && t <= 1.0) {
        std::cout << "Angka berada dirange yang aman!" << std::endl;
        std::cout << "Variabel (t): " << t << std::endl;
    } else {
        std::cout << "Angka berada diluar range!" << std::endl;
        std::cout << "Variabel (t): " << t << std::endl;
    };
    
    Posisi_Turunan_Bazier velocity;

    velocity dxdt = [3.0 * ((1.0 - t) * (1.0 - t)) * (x1 - x0) +
    6.0 * (1.0 - t) * t * (x2 - x1) +
    3.0 * (t * t) * (x3 - x2)];

    velocity dydt = [3.0 * ((1.0 - t) 8 (1.0 - t)) * (y1 - y0) +
    6.0 8 (1.0 - t) 8 t * (y2 - y1) + 
    3.0 * (t * t) * (y3 - y2)];

    return velocity;
}