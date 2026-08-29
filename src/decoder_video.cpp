#include "decoder_video.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Helper fungsi pembatas nilai (clamping)
template <typename T>
static inline uint8_t batasi_uint8(T nilai) {
    if (nilai < 0) return 0;
    if (nilai > 255) return 255;
    return static_cast<uint8_t>(nilai);
}

Decoder_Video::Decoder_Video() 
    : nama_video(""),
      format_ctx(nullptr),
      video_stream_idx(-1),
      lebar(0),
      tinggi(0),
      qp_default(24),
      dpb_buffer()
{
    // Konstruktor default
}

Decoder_Video::Decoder_Video(const std::string& nama_file_video)
    : nama_video(nama_file_video),
      format_ctx(nullptr),
      video_stream_idx(-1),
      lebar(0),
      tinggi(0),
      qp_default(24),
      dpb_buffer()
{
    buka_video(nama_file_video);
}

Decoder_Video::~Decoder_Video() {
    if (format_ctx != nullptr) {
        avformat_close_input(&format_ctx);
        format_ctx = nullptr;
    }
}

bool Decoder_Video::buka_video(const std::string& nama_file) {
    this->nama_video = nama_file;

    if (format_ctx != nullptr) {
        avformat_close_input(&format_ctx);
        format_ctx = nullptr;
    }

    int ret = avformat_open_input(
        &format_ctx,
        nama_video.c_str(),
        nullptr,
        nullptr
    );

    if (ret < 0) {
        std::cerr << "Gagal membuka video: " << nama_file << std::endl;
        return false;
    }

    ret = avformat_find_stream_info(format_ctx, nullptr);
    if (ret < 0) {
        std::cerr << "Gagal membaca stream info!" << std::endl;
        avformat_close_input(&format_ctx);
        format_ctx = nullptr;
        return false;
    }

    video_stream_idx = -1;
    for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
        AVStream* stream = format_ctx->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = static_cast<int>(i);
            break;
        }
    }

    if (video_stream_idx == -1) {
        std::cerr << "Tidak menemukan video stream!" << std::endl;
        avformat_close_input(&format_ctx);
        format_ctx = nullptr;
        return false;
    }

    AVStream* video_stream = format_ctx->streams[video_stream_idx];
    this->lebar = video_stream->codecpar->width;
    this->tinggi = video_stream->codecpar->height;

    std::cout << "Video berhasil dibuka!\n"
              << "Nama: " << nama_video << "\n"
              << "Lebar: " << lebar << "\n"
              << "Tinggi: " << tinggi << "\n"
              << "Codec ID: " << video_stream->codecpar->codec_id << std::endl;

    return true;
}

void Decoder_Video::demuxing() {
    demuxing_dan_parsing_video();
}

int Decoder_Video::mendapatkan_NALType(
    const uint8_t* data,
    int ukuran
) {
    if (data == nullptr || ukuran <= 0) {
        return -1;
    }
    // H.264 NAL Unit type berada pada 5 bit terbawah byte pertama
    return data[0] & 0x1F;
}

void Decoder_Video::parseNALUnits(
    const uint8_t* data,
    int ukuran
) {
    if (data == nullptr || ukuran <= 0) {
        return;
    }

    int posisi = 0;

    // Mendukung format length-prefixed (AVCC 4 byte size)
    while (posisi + 4 <= ukuran) {
        uint32_t nal_size =
            (static_cast<uint32_t>(data[posisi]) << 24) |
            (static_cast<uint32_t>(data[posisi + 1]) << 16) |
            (static_cast<uint32_t>(data[posisi + 2]) << 8) |
            static_cast<uint32_t>(data[posisi + 3]);

        // Cek jika menggunakan Annex-B start code (0x00000001 atau 0x000001)
        if (data[posisi] == 0 && data[posisi + 1] == 0) {
            if (data[posisi + 2] == 1) {
                // 3-byte start code
                posisi += 3;
                int tipe = mendapatkan_NALType(data + posisi, ukuran - posisi);
                std::cout << "Annex-B NAL Type (3-byte): " << tipe << "\n";
                continue;
            } else if (posisi + 3 < ukuran && data[posisi + 2] == 0 && data[posisi + 3] == 1) {
                // 4-byte start code
                posisi += 4;
                int tipe = mendapatkan_NALType(data + posisi, ukuran - posisi);
                std::cout << "Annex-B NAL Type (4-byte): " << tipe << "\n";
                continue;
            }
        }

        posisi += 4;

        if (nal_size == 0) {
            continue;
        }

        if (posisi + nal_size > static_cast<uint32_t>(ukuran)) {
            std::cerr << "NAL size tidak valid atau melebihi batas buffer!\n";
            break;
        }

        const uint8_t* nal_data = data + posisi;
        int tipe = mendapatkan_NALType(nal_data, nal_size);

        std::cout << "NAL Type: " << tipe
                  << ", ukuran: " << nal_size
                  << " bytes\n";

        posisi += nal_size;
    }
}

void Decoder_Video::parseSPS(
    const uint8_t* data,
    int ukuran,
    NALUnit& nal
) {
    if (data == nullptr || ukuran <= 0) {
        return;
    }

    int nal_type = data[0] & 0x1F;
    if (nal_type != 7) {
        std::cerr << "Data bukan SPS (Sequence Parameter Set)!\n";
        return;
    }

    nal.tipe_data = nal_type;
    std::cout << "SPS ditemukan!\n";

    if (ukuran >= 4) {
        std::cout << "Profile IDC: " << static_cast<int>(data[1]) << "\n";
        std::cout << "Constraint Flags: " << static_cast<int>(data[2]) << "\n";
        std::cout << "Level IDC: " << static_cast<int>(data[3]) << "\n";
    }
}

void Decoder_Video::parsePPS(
    const uint8_t* data,
    int ukuran,
    NALUnit& nal
) {
    if (data == nullptr || ukuran <= 0) {
        return;
    }

    int nal_type = data[0] & 0x1F;
    if (nal_type != 8) {
        std::cerr << "Data bukan PPS (Picture Parameter Set)!\n";
        return;
    }

    nal.tipe_data = nal_type;
    std::cout << "PPS ditemukan!\n";
}

void Decoder_Video::demuxing_dan_parsing_video() {
    if (format_ctx == nullptr || video_stream_idx == -1) {
        std::cerr << "Video belum dibuka. Silakan panggil buka_video() terlebih dahulu.\n";
        return;
    }

    AVPacket* paket = av_packet_alloc();
    if (!paket) {
        std::cerr << "Gagal mengalokasikan AVPacket!" << std::endl;
        return;
    }

    while (av_read_frame(format_ctx, paket) >= 0) {
        if (paket->stream_index == video_stream_idx) {
            std::cout << "Paket video ditemukan! Ukuran: "
                      << paket->size << " bytes\n";

            parseNALUnits(paket->data, paket->size);
        }
        av_packet_unref(paket);
    }

    av_packet_free(&paket);
}


uint64_t Decoder_Video::membaca_bit(const uint8_t* data, size_t& bit_offset) {
    size_t byte_idx = bit_offset / 8;
    int bit_pos = 7 - static_cast<int>(bit_offset % 8);
    bit_offset++;
    return (data[byte_idx] >> bit_pos) & 0x01;
}

uint32_t Decoder_Video::membaca_bits(const uint8_t* data, size_t& bit_offset, int jumlah_bit) {
    uint32_t nilai = 0;
    for (int i = 0; i < jumlah_bit; i++) {
        nilai = (nilai << 1) | static_cast<uint32_t>(membaca_bit(data, bit_offset));
    }
    return nilai;
}

// Unsigned Exponential-Golomb Code (ue)
// Rumus: hitung bit 0 di depan (M), baca M bit info -> hasil = (2^M - 1) + info
uint32_t Decoder_Video::membaca_ue(const uint8_t* data, size_t& bit_offset) {
    int leading_zeros = 0;
    while (membaca_bit(data, bit_offset) == 0) {
        leading_zeros++;
        if (leading_zeros > 31) {
            return 0; // Proteksi overflow
        }
    }

    if (leading_zeros == 0) {
        return 0;
    }

    uint32_t info = membaca_bits(data, bit_offset, leading_zeros);
    return (1u << leading_zeros) - 1u + info;
}

// Signed Exponential-Golomb Code (se)
// Mengubah kode ue menjadi nilai bertanda (positif/negatif)
int32_t Decoder_Video::membaca_se(const uint8_t* data, size_t& bit_offset) {
    uint32_t code_num = membaca_ue(data, bit_offset);
    if (code_num == 0) {
        return 0;
    }
    
    int32_t val = static_cast<int32_t>((code_num + 1) / 2);
    if ((code_num & 1) == 0) {
        return -val; // Jika genap -> negatif
    }
    return val;      // Jika ganjil -> positif
}

void Decoder_Video::Entropy_decoding(
    const std::vector<uint8_t>& bitstream,
    std::vector<Blok_Residual>& hasil_residual,
    std::vector<Vektor_Pergerakan>& hasil_mv,
    std::vector<Mode_Prediksi_Intra>& hasil_mode,
    int jumlah_blok
) {
    hasil_residual.clear();
    hasil_mv.clear();
    hasil_mode.clear();

    if (bitstream.empty() || jumlah_blok <= 0) {
        return;
    }

    size_t bit_offset = 0;
    size_t max_bits = bitstream.size() * 8;

    for (int b = 0; b < jumlah_blok && bit_offset + 8 <= max_bits; b++) {
        // 1. Dekode Mode Prediksi (Intra)
        uint32_t mode_val = membaca_bits(bitstream.data(), bit_offset, 2);
        Mode_Prediksi_Intra mode = static_cast<Mode_Prediksi_Intra>(mode_val % 4);
        hasil_mode.push_back(mode);

        // 2. Dekode Motion Vectors (untuk Inter Prediksi)
        Vektor_Pergerakan mv;
        mv.mv_x = membaca_se(bitstream.data(), bit_offset);
        mv.mv_y = membaca_se(bitstream.data(), bit_offset);
        hasil_mv.push_back(mv);

        // 3. Dekode Koefisien Residual Frekuensi (blok 4x4 = 16 elemen)
        Blok_Residual blok;
        blok.ukuran = 4;
        blok.koefisien_frekuensi.resize(16, 0);

        // Membaca koefisien DC dan AC menggunakan signed Exp-Golomb
        for (int i = 0; i < 16 && bit_offset < max_bits; i++) {
            blok.koefisien_frekuensi[i] = membaca_se(bitstream.data(), bit_offset);
        }

        hasil_residual.push_back(blok);
    }
}

void Decoder_Video::invers_quantization(Blok_Residual& blok, int qp) {
    int n = blok.ukuran;
    int total_elemen = n * n;

    blok.koefisien_dekuantisasi.resize(total_elemen, 0.0f);

    // Matriks kuantisasi standar (skala kuantisasi frekuensi)
    // Frekuensi rendah (kiri atas) dikuantisasi lebih halus daripada frekuensi tinggi
    static const float Q_Matrix_4x4[16] = {
        16.0f, 16.0f, 19.0f, 22.0f,
        16.0f, 16.0f, 22.0f, 24.0f,
        19.0f, 22.0f, 26.0f, 27.0f,
        22.0f, 24.0f, 27.0f, 29.0f
    };

    // Skala pengali berbasis parameter kuantisasi QP (2^(QP/6))
    float faktor_skala_qp = std::pow(2.0f, static_cast<float>(qp) / 6.0f);

    for (int i = 0; i < total_elemen; i++) {
        float q_weight = (i < 16 && n == 4) ? (Q_Matrix_4x4[i] / 16.0f) : 1.0f;
        int coeff = (i < static_cast<int>(blok.koefisien_frekuensi.size())) 
                    ? blok.koefisien_frekuensi[i] : 0;

        // Dekuantisasi: Mengembalikan skala koefisien frekuensi
        blok.koefisien_dekuantisasi[i] = static_cast<float>(coeff) * q_weight * faktor_skala_qp;
    }
}

void Decoder_Video::Invers_Transform(Blok_Residual& blok) {
    int n = blok.ukuran;
    int total_elemen = n * n;

    blok.data_residual.resize(total_elemen, 0);

    // 2D IDCT (Inverse Discrete Cosine Transform)
    // Mengubah domain frekuensi kembali ke domain spasial (pixel differences)
    // f(x, y) = sum_u sum_v C(u) C(v) F(u, v) cos((2x+1)u pi / 2N) cos((2y+1)v pi / 2N)
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            float sum = 0.0f;

            for (int v = 0; v < n; v++) {
                for (int u = 0; u < n; u++) {
                    float cu = (u == 0) ? (1.0f / std::sqrt(static_cast<float>(n))) 
                                        : (std::sqrt(2.0f / static_cast<float>(n)));
                    float cv = (v == 0) ? (1.0f / std::sqrt(static_cast<float>(n))) 
                                        : (std::sqrt(2.0f / static_cast<float>(n)));

                    float coeff = blok.koefisien_dekuantisasi[v * n + u];

                    float cos_x = std::cos(static_cast<float>((2 * x + 1) * u) * static_cast<float>(M_PI) / (2.0f * n));
                    float cos_y = std::cos(static_cast<float>((2 * y + 1) * v) * static_cast<float>(M_PI) / (2.0f * n));

                    sum += cu * cv * coeff * cos_x * cos_y;
                }
            }

            // Simpan data residual spasial hasil IDCT
            blok.data_residual[y * n + x] = static_cast<int>(std::round(sum));
        }
    }
}

void Decoder_Video::rekonstruksi_intra(
    std::vector<uint8_t>& bidang_y,
    int blok_x,
    int blok_y,
    int lebar_bidang,
    Mode_Prediksi_Intra mode,
    const Blok_Residual& residual
) {
    int n = residual.ukuran;

    // Ambil sampel batas tetangga atas dan kiri jika tersedia
    std::vector<uint8_t> tetangga_atas(n, 128);
    std::vector<uint8_t> tetangga_kiri(n, 128);

    bool ada_atas = (blok_y > 0);
    bool ada_kiri = (blok_x > 0);

    if (ada_atas) {
        for (int i = 0; i < n; i++) {
            tetangga_atas[i] = bidang_y[(blok_y - 1) * lebar_bidang + (blok_x + i)];
        }
    }
    if (ada_kiri) {
        for (int i = 0; i < n; i++) {
            tetangga_kiri[i] = bidang_y[(blok_y + i) * lebar_bidang + (blok_x - 1)];
        }
    }

    // Hitung prediksi berdasarkan mode intra spatial
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            int prediksi = 128;

            switch (mode) {
                case Mode_Prediksi_Intra::DC: {
                    int total = 0;
                    int count = 0;
                    if (ada_atas) {
                        for (int i = 0; i < n; i++) total += tetangga_atas[i];
                        count += n;
                    }
                    if (ada_kiri) {
                        for (int i = 0; i < n; i++) total += tetangga_kiri[i];
                        count += n;
                    }
                    prediksi = (count > 0) ? (total / count) : 128;
                    break;
                }
                case Mode_Prediksi_Intra::VERTIKAL:
                    prediksi = tetangga_atas[x];
                    break;

                case Mode_Prediksi_Intra::HORIZONTAL:
                    prediksi = tetangga_kiri[y];
                    break;

                case Mode_Prediksi_Intra::PLANAR: {
                    // Interpolasi planar gradasi lembut dari piksel atas dan kiri
                    int t_val = tetangga_atas[x];
                    int l_val = tetangga_kiri[y];
                    prediksi = (t_val + l_val + 1) / 2;
                    break;
                }
            }

            // Ambil residual spasial dari hasil IDCT
            int res = (y * n + x < static_cast<int>(residual.data_residual.size())) 
                      ? residual.data_residual[y * n + x] : 0;

            // Piksel Rekonstruksi = Data Prediksi + Data Residual
            int nilai_rekonstruksi = prediksi + res;

            size_t idx = static_cast<size_t>(blok_y + y) * lebar_bidang + (blok_x + x);
            if (idx < bidang_y.size()) {
                bidang_y[idx] = batasi_uint8(nilai_rekonstruksi);
            }
        }
    }
}

void Decoder_Video::rekonstruksi_inter(
    std::vector<uint8_t>& bidang_y_tujuan,
    const std::vector<uint8_t>& bidang_y_referensi,
    int blok_x,
    int blok_y,
    int lebar_bidang,
    int tinggi_bidang,
    const Vektor_Pergerakan& mv,
    const Blok_Residual& residual
) {
    int n = residual.ukuran;

    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            // Kompensasi Pergerakan (Motion Compensation):
            // Posisi sumber pada frame referensi digeser dengan Motion Vector (mv_x, mv_y)
            int ref_x = std::clamp(blok_x + x + mv.mv_x, 0, lebar_bidang - 1);
            int ref_y = std::clamp(blok_y + y + mv.mv_y, 0, tinggi_bidang - 1);

            size_t ref_idx = static_cast<size_t>(ref_y) * lebar_bidang + ref_x;
            uint8_t prediksi = (ref_idx < bidang_y_referensi.size()) 
                               ? bidang_y_referensi[ref_idx] : 128;

            int res = (y * n + x < static_cast<int>(residual.data_residual.size())) 
                      ? residual.data_residual[y * n + x] : 0;

            // Piksel Rekonstruksi = Data Prediksi Temporal + Data Residual
            int nilai_rekonstruksi = static_cast<int>(prediksi) + res;

            size_t dst_idx = static_cast<size_t>(blok_y + y) * lebar_bidang + (blok_x + x);
            if (dst_idx < bidang_y_tujuan.size()) {
                bidang_y_tujuan[dst_idx] = batasi_uint8(nilai_rekonstruksi);
            }
        }
    }
}

void Decoder_Video::Rekonstruksi_piksel(
    Frame_Video& frame_hasil,
    const std::vector<Blok_Residual>& daftar_residual,
    const std::vector<Vektor_Pergerakan>& daftar_mv,
    const std::vector<Mode_Prediksi_Intra>& daftar_mode,
    const Frame_Video* frame_ref_sebelumnya
) {
    int w = frame_hasil.lebar;
    int h = frame_hasil.tinggi;

    if (w <= 0 || h <= 0) return;

    frame_hasil.bidang_Y.resize(w * h, 128);
    frame_hasil.bidang_U.resize((w / 2) * (h / 2), 128);
    frame_hasil.bidang_V.resize((w / 2) * (h / 2), 128);

    int blok_idx = 0;
    int n = 4; // Ukuran blok 4x4

    for (int y = 0; y < h; y += n) {
        for (int x = 0; x < w; x += n) {
            Blok_Residual residual;
            if (blok_idx < static_cast<int>(daftar_residual.size())) {
                residual = daftar_residual[blok_idx];
            } else {
                residual.ukuran = n;
                residual.data_residual.resize(n * n, 0);
            }

            if (frame_hasil.tipe == Tipe_Frame::I_FRAME || frame_ref_sebelumnya == nullptr) {
                // I-Frame: Gunakan Intra Prediction (Spatial)
                Mode_Prediksi_Intra mode = (blok_idx < static_cast<int>(daftar_mode.size())) 
                                           ? daftar_mode[blok_idx] : Mode_Prediksi_Intra::DC;
                rekonstruksi_intra(frame_hasil.bidang_Y, x, y, w, mode, residual);
            } else {
                // P-Frame / B-Frame: Gunakan Inter Prediction (Motion Compensation)
                Vektor_Pergerakan mv;
                if (blok_idx < static_cast<int>(daftar_mv.size())) {
                    mv = daftar_mv[blok_idx];
                }
                rekonstruksi_inter(
                    frame_hasil.bidang_Y,
                    frame_ref_sebelumnya->bidang_Y,
                    x, y, w, h,
                    mv,
                    residual
                );
            }

            blok_idx++;
        }
    }
}

void Decoder_Video::deblocking_filter(
    std::vector<uint8_t>& bidang,
    int lebar_bidang,
    int tinggi_bidang,
    int ukuran_blok,
    int qp
) {
    if (bidang.empty() || lebar_bidang <= 0 || tinggi_bidang <= 0) return;

    // Parameter ambang batas filter deblocking berdasarkan QP
    int alpha = std::min(255, 4 + (qp / 2));
    int beta  = std::min(255, 2 + (qp / 4));
    int c0    = std::max(1, qp / 8);

    // Deblocking pada batas vertikal antar-blok
    for (int x = ukuran_blok; x < lebar_bidang; x += ukuran_blok) {
        for (int y = 0; y < tinggi_bidang; y++) {
            size_t p0_idx = static_cast<size_t>(y) * lebar_bidang + (x - 1);
            size_t q0_idx = static_cast<size_t>(y) * lebar_bidang + x;
            size_t p1_idx = static_cast<size_t>(y) * lebar_bidang + std::max(0, x - 2);
            size_t q1_idx = static_cast<size_t>(y) * lebar_bidang + std::min(lebar_bidang - 1, x + 1);

            int p0 = bidang[p0_idx];
            int q0 = bidang[q0_idx];
            int p1 = bidang[p1_idx];
            int q1 = bidang[q1_idx];

            // Deteksi apakah batas merupakan artefak kompresi atau kontur alami
            if (std::abs(p0 - q0) < alpha && std::abs(p1 - p0) < beta && std::abs(q1 - q0) < beta) {
                int delta = std::clamp((4 * (q0 - p0) + (p1 - q1) + 4) / 8, -c0, c0);
                bidang[p0_idx] = batasi_uint8(p0 + delta);
                bidang[q0_idx] = batasi_uint8(q0 - delta);
            }
        }
    }

    // Deblocking pada batas horizontal antar-blok
    for (int y = ukuran_blok; y < tinggi_bidang; y += ukuran_blok) {
        for (int x = 0; x < lebar_bidang; x++) {
            size_t p0_idx = static_cast<size_t>(y - 1) * lebar_bidang + x;
            size_t q0_idx = static_cast<size_t>(y) * lebar_bidang + x;
            size_t p1_idx = static_cast<size_t>(std::max(0, y - 2)) * lebar_bidang + x;
            size_t q1_idx = static_cast<size_t>(std::min(tinggi_bidang - 1, y + 1)) * lebar_bidang + x;

            int p0 = bidang[p0_idx];
            int q0 = bidang[q0_idx];
            int p1 = bidang[p1_idx];
            int q1 = bidang[q1_idx];

            if (std::abs(p0 - q0) < alpha && std::abs(p1 - p0) < beta && std::abs(q1 - q0) < beta) {
                int delta = std::clamp((4 * (q0 - p0) + (p1 - q1) + 4) / 8, -c0, c0);
                bidang[p0_idx] = batasi_uint8(p0 + delta);
                bidang[q0_idx] = batasi_uint8(q0 - delta);
            }
        }
    }
}

void Decoder_Video::sample_adaptive_offset(
    std::vector<uint8_t>& bidang,
    int lebar_bidang,
    int tinggi_bidang,
    int offset_edge
) {
    if (bidang.empty() || lebar_bidang <= 2 || tinggi_bidang <= 2) return;

    // SAO Edge Offset: Mengoreksi distorsi lembah/puncak (ringing) di sekitar gradien tinggi
    for (int y = 1; y < tinggi_bidang - 1; y++) {
        for (int x = 1; x < lebar_bidang - 1; x++) {
            size_t c_idx = static_cast<size_t>(y) * lebar_bidang + x;
            size_t l_idx = static_cast<size_t>(y) * lebar_bidang + (x - 1);
            size_t r_idx = static_cast<size_t>(y) * lebar_bidang + (x + 1);

            int val_c = bidang[c_idx];
            int val_l = bidang[l_idx];
            int val_r = bidang[r_idx];

            // Kategori Lembah Lokal (Local Minima): Tambahkan offset positif
            if (val_c < val_l && val_c < val_r) {
                bidang[c_idx] = batasi_uint8(val_c + offset_edge);
            }
            // Kategori Puncak Lokal (Local Maxima): Kurangi dengan offset
            else if (val_c > val_l && val_c > val_r) {
                bidang[c_idx] = batasi_uint8(val_c - offset_edge);
            }
        }
    }
}

void Decoder_Video::Pengulangan_filtering(Frame_Video& frame, int qp) {
    deblocking_filter(frame.bidang_Y, frame.lebar, frame.tinggi, 4, qp);
    deblocking_filter(frame.bidang_U, frame.lebar / 2, frame.tinggi / 2, 4, qp);
    deblocking_filter(frame.bidang_V, frame.lebar / 2, frame.tinggi / 2, 4, qp);
    sample_adaptive_offset(frame.bidang_Y, frame.lebar, frame.tinggi, 1);
}


void Decoder_Video::frame_buffering(const Frame_Video& frame) {
    dpb_buffer.push_back(frame);
}

std::vector<Frame_Video> Decoder_Video::ambil_frame_urut_pts() {
    std::vector<Frame_Video> hasil = dpb_buffer;

    // Mengurutkan buffer frame berdasarkan Presentation Time Stamp (PTS)
    std::sort(hasil.begin(), hasil.end(), [](const Frame_Video& a, const Frame_Video& b) {
        return a.pts < b.pts;
    });

    return hasil;
}

std::vector<RGB> Decoder_Video::color_conversion(const Frame_Video& frame) {
    int w = frame.lebar;
    int h = frame.tinggi;

    std::vector<RGB> hasil_rgb(w * h);

    // Konversi ruang warna YUV420p ke RGB (Standar ITU-R BT.601 / BT.709)
    // Formula matematis:
    // R = Y + 1.402 * (V - 128)
    // G = Y - 0.344136 * (U - 128) - 0.714136 * (V - 128)
    // B = Y + 1.772 * (U - 128)
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            size_t y_idx = static_cast<size_t>(y) * w + x;
            size_t uv_idx = static_cast<size_t>(y / 2) * (w / 2) + (x / 2);

            uint8_t val_y = (y_idx < frame.bidang_Y.size()) ? frame.bidang_Y[y_idx] : 0;
            uint8_t val_u = (uv_idx < frame.bidang_U.size()) ? frame.bidang_U[uv_idx] : 128;
            uint8_t val_v = (uv_idx < frame.bidang_V.size()) ? frame.bidang_V[uv_idx] : 128;

            float y_f = static_cast<float>(val_y);
            float u_f = static_cast<float>(val_u) - 128.0f;
            float v_f = static_cast<float>(val_v) - 128.0f;

            float r = y_f + 1.402f * v_f;
            float g = y_f - 0.344136f * u_f - 0.714136f * v_f;
            float b = y_f + 1.772f * u_f;

            RGB rgb;
            rgb.R = batasi_uint8(std::round(r));
            rgb.G = batasi_uint8(std::round(g));
            rgb.B = batasi_uint8(std::round(b));

            hasil_rgb[y_idx] = rgb;
        }
    }

    return hasil_rgb;
}

Mendapatkan_Pixel Decoder_Video::konversi_ke_objek_pixel(const Frame_Video& frame) {
    Mendapatkan_Pixel obj_pixel(frame.lebar, frame.tinggi);

    std::vector<RGB> buffer_rgb = color_conversion(frame);

    return obj_pixel;
}