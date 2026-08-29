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
        curl_easy_setopt(this->curl, CURLOPT_URL, "http://localhost:8000/upload");
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

bool Upload_Video::upload_banyak_video(const std::vector<std::string>& daftar_video) {
    if (daftar_video.empty()) {
        std::cerr << "Daftar video kosong!\n";
        return false;
    }

    bool semua_sukses = true;
    std::cout << "Memulai upload " << daftar_video.size() << " video secara otomatis...\n";

    for (size_t i = 0; i < daftar_video.size(); i++) {
        std::cout << "[" << (i + 1) << "/" << daftar_video.size() << "] ";
        bool sukses = pengupload_video(daftar_video[i]);
        if (!sukses) semua_sukses = false;
    }

    return semua_sukses;
}

bool Upload_Video::pengupload_video_multi(const std::vector<std::string>& daftar_video) {
    if (daftar_video.empty()) {
        std::cerr << "Daftar video kosong!\n";
        return false;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    this->curl = curl_easy_init();

    bool status = false;
    if (this->curl) {
        curl_easy_setopt(this->curl, CURLOPT_URL, "http://localhost:8000/upload");
        curl_easy_setopt(this->curl, CURLOPT_POST, 1L);

        curl_mime* mime = curl_mime_init(this->curl);

        for (const auto& vid : daftar_video) {
            curl_mimepart* part = curl_mime_addpart(mime);
            curl_mime_name(part, "videos");
            curl_mime_filedata(part, vid.c_str());
        }

        curl_easy_setopt(this->curl, CURLOPT_MIMEPOST, mime);

        this->res = curl_easy_perform(this->curl);

        if (this->res != CURLE_OK) {
            std::cerr << "Upload multi-video gagal: " << curl_easy_strerror(this->res) << '\n';
        } else {
            std::cout << "Upload " << daftar_video.size() << " video secara batch berhasil!\n";
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