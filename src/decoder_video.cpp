#include "decoder_video.hpp"

Decoder_video::Decoder_video () : argc(0), argv[], 
 format_ctx(nullptr),
video_stream_idx(0), nama_video(), Upload_Video()
{
    // kosong
}

av_register_all();

int Decoder_video::mendapatkan_NALType(
    uint8_t* data,
    int ukuran
) {
    if (data == nullptr || ukuran <= 0) {
        return -1;
    }

    return data[0] & 0x1F;
}

void Decoder_video::parseNALUnits(
    const uint8_t* data,
    int ukuran
) {
    int posisi = 0;

    while (posisi + 4 <= ukuran) {

        uint32_t nal_size =
            (static_cast<uint32_t>(data[posisi]) << 24) |
            (static_cast<uint32_t>(data[posisi + 1]) << 16) |
            (static_cast<uint32_t>(data[posisi + 2]) << 8) |
            static_cast<uint32_t>(data[posisi + 3]);

        posisi += 4;

        if (nal_size == 0) {
            continue;
        }

        if (posisi + nal_size > ukuran) {
            std::cerr << "NAL size tidak valid!\n";
            break;
        }

        const uint8_t* nal_data = data + posisi;

        int tipe = mendapatkan_NALType(
            const_cast<uint8_t*>(nal_data),
            nal_size
        );

        std::cout << "NAL Type: " << tipe
                  << ", ukuran: " << nal_size
                  << "\n";

        posisi += nal_size;
    }
}

void Decoder_video::parseSPS(
    const uint8_t* data,
    int ukuran,
    NALUnit& nal
) {
    if (data == nullptr || ukuran <= 0) {
        return;
    }

    int nal_type = data[0] & 0x1F;

    if (nal_type != 7) {
        std::cerr << "Data bukan SPS!\n";
        return;
    }

    nal.tipe_data = nal_type;

    std::cout << "SPS ditemukan!\n";

    if (ukuran >= 4) {
        std::cout << "Profile ID: "
                  << static_cast<int>(data[1])
                  << '\n';

        std::cout << "Level ID: "
                  << static_cast<int>(data[3])
                  << '\n';
    }
}

void Decoder_video::parsePPS(
    const uint8_t* data,
    int ukuran,
    NALUnit& nal
) {
    if (data == nullptr || ukuran <= 0) {
        return;
    }

    int nal_type = data[0] & 0x1F;

    if (nal_type != 8) {
        std::cerr << "Data bukan PPS!\n";
        return;
    }

    nal.tipe_data = nal_type;

    std::cout << "PPS ditemukan!\n";
}

void Decoder_video::demuxing_dan_parsing_video() {
    int ret = avformat_open_input(
        &format_ctx,
        nama_video,
        nullptr,
        nullptr
    );

    if (ret < 0) {
        std::cerr << "Gagal membuka video!" << std::endl;
        return 1;
    }

    ret = avformat_find_stream_info(format_ctx, nullptr);

    if (ret < 0) {
        std::cerr << "Gagal membaca stream info!" << std::endl;
        avformat_close_input(&format_ctx);
        return 1;
    }

    int video_stream_idx = -1;

    for (unsigned int i = 0;
    i < format_ctx->nb_streams;
    i++) {
        AVStream* stream = format_ctx->streams[i];

        if (stream->codecpar->codec_type ==
        AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    if (video_stream_idx == -1) {
        std::cerr << "Tidak menemukan video stream" << std::endl;
        avformat_close_input(%format_ctx);
        return 1;
    }

    // Mencari informasi video
    AVstream* video_stream = format_ctx->streams[video_stream_idx];

    std::cout << "Video berhasil ditemukan!" << std::endl;

    std::cout << "Lebar: " << video_stream->codecpar->width << std::endl;
    std::cout << "Panjang: " << video_stream->codecpar->width << std::endl;
    std::cout << "Codec ID: " << video_stream->codecpar->codec_id << std::endl;
    
    AVPacket* paket = av_packet_alloc();

    if (!packet) {
        std::cerr << "Gagal membuat paket!" << std::endl;

        avformat_close_input(&format_ctx);

        return 1;
    }

    while (av_read_frame(format_ctx, paket) >= 0) {
        if (paket->stream_index == video_stream_idx) {
            std::cout << "Paket video ditemukan!" << std::endl;

            std::cout << "Ukuran: " << paket->size << "bytes" << std::endl;

            parseNALUnits(
                paket->data,
                paket->size
            );
        }

        av_packet_unref(packet);
    }


}