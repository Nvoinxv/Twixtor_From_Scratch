#include "decoder_video.hpp"

Decoder_video::Decoder_video () : argc(0), argv[], 
 format_ctx(nullptr),
video_stream_idx(0), nama_video(), Upload_Video()
{
    // kosong
}

av_register_all();

void Decoder_video::demuxing_dan_parsing_video() {
    if (argc < 2) {
        std::cerr << "Penggunaan: " << argv[0] << "<input_file>" << std::endl;
        return 1;
    };

    if (avformat_open_input(&format_ctx, nama_video, 
        nullptr, nullptr) < 0) {
            std::cerr << "Tidak bisa membuka file video: " << input_file << std::endl;
            return 2;
        };

    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        std::cerr << "Tidak bisa mencari informasi stream!" << std::endl;

        return 3;
    }

    video_stream_idx = -1;
    for (unsigned int i = 0; i < format_ctx->nb_stream; ++i) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    } 

    if (vide_stream_idx == -1) {
        std::cerr << "Tidak bisa mencari stream video!" << std::endl;
        return 4;
    }




}
