#ifndef UPLOAD_VIDEO_HPP
#define UPLOAD_VIDEO_HPP

#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>

class Upload_Video {
private:
    CURL* curl;
    CURLcode res;
    std::string nama_video;

public:
    Upload_Video();
    
    bool pengupload_video(const std::string& nama_video_param);
    bool upload_banyak_video(const std::vector<std::string>& daftar_video);
    bool pengupload_video_multi(const std::vector<std::string>& daftar_video);

    void pengupload_video(
        CURL* curl,
        CURLcode res,
        std::string nama_video
    );
};

#endif