#ifndef DECODER_VIDEO_HPP
#define DECODER_VIDEO_HPP

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/error.h>
#include <libavutil/avutil.h>
}

#include <iostream>
#include "upload_video.hpp"
#include <string>
#include <vector>
#include <fstream>

class Decoder_Video {
    private:
    int argc;
    char * argv[];
    Upload_Video();
    std::string nama_video;
    AVFormatContext* format_ctx = nullptr;
    int video_stream_idx;
    
    struct NALUnit {
        uint8_t* data;         
        int ukuran;               
        int tipe_data;               
        bool is_keyframe;      
        
        int lebar;             
        int panjang;             
        int frame_rate;         
    };
    
    public:
    Decoder_video();

    void demuxing();
    void parseSPS(const uint8_t* data, int ukuran, NALUnit& nal);
    void parsePPS(const uint8_t* data, int ukuran, NALUnit& nal);
    void parseNALUnits(const uint8_t* data, int ukuran);
    int mendapatkan_NALType(uint8_t* data, int ukuran);

    void Entropy_decoding();

    void invers_quantization();

    void Invers_Transform();

    void Rekronstruksi_piksel();

    void Pengulangan_filtering();

    void frame_buffering();

    void color_conversion();
};

#endif