#ifndef DECODER_VIDEO_HPP
#define DECODER_VIDEO_HPP

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/error.h>
#include <libavutil/avutil.h>
}

#include "Pixel.hpp"
#include "upload_video.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <memory>

// Tipe frame video
enum class Tipe_Frame {
    I_FRAME, // Intra-frame (Keyframe)
    P_FRAME, // Predicted frame (Forward prediction)
    B_FRAME  // Bi-directional predicted frame
};

// Mode prediksi intra untuk spatial prediction
enum class Mode_Prediksi_Intra {
    DC,          // Rata-rata dari tetangga atas dan kiri
    VERTIKAL,    // Menyalin piksel baris atas ke bawah
    HORIZONTAL,  // Menyalin piksel kolom kiri ke kanan
    PLANAR       // Interpolasi bilinear permukaan lembut
};

// Struktur vektor pergerakan (Motion Vector) untuk Inter Prediction
struct Vektor_Pergerakan {
    int mv_x = 0; // Pergeseran horizontal
    int mv_y = 0; // Pergeseran vertikal
};

// Struktur blok residual untuk proses IQ/IT
struct Blok_Residual {
    int ukuran = 4;                               // Ukuran blok (misal 4x4 atau 8x8)
    std::vector<int> koefisien_frekuensi;         // Hasil entropy decoding (domain frekuensi)
    std::vector<float> koefisien_dekuantisasi;    // Hasil inverse quantization (IQ)
    std::vector<int> data_residual;               // Hasil inverse transform / IDCT (domain spasial)
};

// Struktur frame video untuk Decoded Picture Buffer (DPB)
struct Frame_Video {
    int lebar = 0;
    int tinggi = 0;
    int64_t pts = 0;                              // Presentation Time Stamp
    int64_t poc = 0;                              // Picture Order Count
    Tipe_Frame tipe = Tipe_Frame::I_FRAME;
    bool is_referensi = true;

    std::vector<uint8_t> bidang_Y;                // Komponen Luma (lebar * tinggi)
    std::vector<uint8_t> bidang_U;                // Komponen Chroma Cb ((lebar/2) * (tinggi/2))
    std::vector<uint8_t> bidang_V;                // Komponen Chroma Cr ((lebar/2) * (tinggi/2))
};

// Struktur unit NAL (Network Abstraction Layer)
struct NALUnit {
    const uint8_t* data = nullptr;
    int ukuran = 0;
    int tipe_data = 0;
    bool is_keyframe = false;
    
    int lebar = 0;
    int panjang = 0; // Tinggi
    int frame_rate = 0;
};

class Decoder_Video {
private:
    std::string nama_video;
    AVFormatContext* format_ctx = nullptr;
    int video_stream_idx = -1;
    int lebar = 0;
    int tinggi = 0;
    int qp_default = 24; // Quantization Parameter default

    // Decoded Picture Buffer (DPB) untuk menampung frame sebelum ditampilkan
    std::vector<Frame_Video> dpb_buffer;

public:
    // Konstruktor dan Destruktor
    Decoder_Video();
    Decoder_Video(const std::string& nama_file_video);
    ~Decoder_Video();

    // 1. Demuxing & NAL Parsing
    bool buka_video(const std::string& nama_file);
    void demuxing();
    void demuxing_dan_parsing_video();
    void parseNALUnits(const uint8_t* data, int ukuran);
    int mendapatkan_NALType(const uint8_t* data, int ukuran);
    void parseSPS(const uint8_t* data, int ukuran, NALUnit& nal);
    void parsePPS(const uint8_t* data, int ukuran, NALUnit& nal);

    // 2. Entropy Decoding & Bitstream Reader
    uint64_t membaca_bit(const uint8_t* data, size_t& bit_offset);
    uint32_t membaca_bits(const uint8_t* data, size_t& bit_offset, int jumlah_bit);
    uint32_t membaca_ue(const uint8_t* data, size_t& bit_offset); // Unsigned Exponential-Golomb
    int32_t membaca_se(const uint8_t* data, size_t& bit_offset);  // Signed Exponential-Golomb
    void Entropy_decoding(
        const std::vector<uint8_t>& bitstream,
        std::vector<Blok_Residual>& hasil_residual,
        std::vector<Vektor_Pergerakan>& hasil_mv,
        std::vector<Mode_Prediksi_Intra>& hasil_mode,
        int jumlah_blok
    );

    // 3. Inverse Quantization & Inverse Transform (IQ/IT)
    void invers_quantization(Blok_Residual& blok, int qp);
    void Invers_Transform(Blok_Residual& blok);

    // 4. Rekonstruksi Piksel (Prediksi & Penggabungan)
    void rekonstruksi_intra(
        std::vector<uint8_t>& bidang_y,
        int blok_x,
        int blok_y,
        int lebar_bidang,
        Mode_Prediksi_Intra mode,
        const Blok_Residual& residual
    );

    void rekonstruksi_inter(
        std::vector<uint8_t>& bidang_y_tujuan,
        const std::vector<uint8_t>& bidang_y_referensi,
        int blok_x,
        int blok_y,
        int lebar_bidang,
        int tinggi_bidang,
        const Vektor_Pergerakan& mv,
        const Blok_Residual& residual
    );

    void Rekonstruksi_piksel(
        Frame_Video& frame_hasil,
        const std::vector<Blok_Residual>& daftar_residual,
        const std::vector<Vektor_Pergerakan>& daftar_mv,
        const std::vector<Mode_Prediksi_Intra>& daftar_mode,
        const Frame_Video* frame_ref_sebelumnya = nullptr
    );

    // 5. In-Loop Filtering
    void deblocking_filter(
        std::vector<uint8_t>& bidang,
        int lebar_bidang,
        int tinggi_bidang,
        int ukuran_blok,
        int qp
    );

    void sample_adaptive_offset(
        std::vector<uint8_t>& bidang,
        int lebar_bidang,
        int tinggi_bidang,
        int offset_edge
    );

    void Pengulangan_filtering(Frame_Video& frame, int qp);

    // 6. Frame Buffering (DPB) & Color Conversion
    void frame_buffering(const Frame_Video& frame);
    std::vector<Frame_Video> ambil_frame_urut_pts();
    std::vector<RGB> color_conversion(const Frame_Video& frame);
    Mendapatkan_Pixel konversi_ke_objek_pixel(const Frame_Video& frame);

    // Getter untuk dimensi dan status
    int mendapatkan_lebar() const { return lebar; }
    int mendapatkan_tinggi() const { return tinggi; }
    const std::vector<Frame_Video>& mendapatkan_dpb() const { return dpb_buffer; }
};

#endif