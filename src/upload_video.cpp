#include "upload_video.hpp"

Upload_Video::Upload_Video() : curl(nullptr), res(CURLE_OK), nama_video("") {
    // Kosong
}

void Upload_Video::pengupload_video(
    CURL* curl_param,
    CURLcode res_param,
    std::string nama_video_param
) {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    this->curl = curl_easy_init();

    if (this->curl) {
        curl_easy_setopt(this->curl, CURLOPT_URL, "http://localhost:3000/upload");
        curl_easy_setopt(this->curl, CURLOPT_POST, 1L);

        curl_mime* mime = curl_mime_init(this->curl);
        curl_mimepart* part = curl_mime_addpart(mime);

        curl_mime_name(part, "video");
        
        std::cout << "Masukkan nama file video: ";
        std::cin >> this->nama_video;
        
        curl_mime_filedata(part, this->nama_video.c_str());

        std::cout << std::endl;

        curl_easy_setopt(this->curl, CURLOPT_MIMEPOST, mime);

        this->res = curl_easy_perform(this->curl);

        if (this->res != CURLE_OK) {
            std::cerr << "Upload gagal: "
                      << curl_easy_strerror(this->res)
                      << '\n';
        } else {
            std::cout << "Upload berhasil!\n";
        }

        curl_mime_free(mime);
        curl_easy_cleanup(this->curl);
    }

    curl_global_cleanup();
}