#include "upload_video.hpp"

Upload_Video::Upload_Video() : curl(nullptr), res(CURLE_OK), nama_video("") {
    // Kosong
}

bool Upload_Video::pengupload_video(const std::string& nama_video_param) {
    if (nama_video_param.empty()) {
        std::cerr << "Nama file video kosong!\n";
        return false;
    }

    this->nama_video = nama_video_param;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    this->curl = curl_easy_init();

    bool status = false;
    if (this->curl) {
        curl_easy_setopt(this->curl, CURLOPT_URL, "http://localhost:3000/upload");
        curl_easy_setopt(this->curl, CURLOPT_POST, 1L);

        curl_mime* mime = curl_mime_init(this->curl);
        curl_mimepart* part = curl_mime_addpart(mime);

        curl_mime_name(part, "video");
        curl_mime_filedata(part, this->nama_video.c_str());

        curl_easy_setopt(this->curl, CURLOPT_MIMEPOST, mime);

        this->res = curl_easy_perform(this->curl);

        if (this->res != CURLE_OK) {
            std::cerr << "Upload gagal: " << curl_easy_strerror(this->res) << '\n';
        } else {
            std::cout << "Upload video " << this->nama_video << " berhasil!\n";
            status = true;
        }

        curl_mime_free(mime);
        curl_easy_cleanup(this->curl);
        this->curl = nullptr;
    }

    curl_global_cleanup();
    return status;
}

void Upload_Video::pengupload_video(
    CURL* curl_param,
    CURLcode res_param,
    std::string nama_video_param
) {
    if (!nama_video_param.empty()) {
        pengupload_video(nama_video_param);
        return;
    }

    std::cout << "Masukkan nama file video: ";
    std::string input_nama;
    std::cin >> input_nama;
    pengupload_video(input_nama);
}