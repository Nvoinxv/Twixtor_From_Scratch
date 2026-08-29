#ifndef UPLOAD_VIDEO_HPP
#define UPLOAD_VIDEO_HPP

#include <iostream>
#include <curl/curl.h>
#include <string>

class Upload_Video {
    private:
    CURL* curl;
    CURLcode res;
    std::string nama_video;

    public:
    Upload_Video();
    
    bool pengupload_video(const std::string& nama_video_param);

    void pengupload_video(
        CURL* curl,
        CURLcode res,
        std::string nama_video
    );
};

#endif