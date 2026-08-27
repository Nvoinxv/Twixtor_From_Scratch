#include "upload_video.hpp"

Upload_Video::Upload_Video() : curl(), res(), nama_video {
    // Kosong
}

void Upload_Video::pengupload_video(
    CURL* curl,
    CURLcode res,
    std::string nama_video
) {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL,
                         "http://localhost:3000/upload");

        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        curl_mime* mime = curl_mime_init(curl);

        curl_mimepart* part = curl_mime_addpart(mime);

        curl_mime_name(part, "video");
        
        std::cout << "Masukkan nama file video: ";
        std::cin >> nama_video;
        
        curl_mime_filedata(part, nama_video);

        std::cout << std::endl;

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Upload gagal: "
                      << curl_easy_strerror(res)
                      << '\n';
        } else {
            std::cout << "Upload berhasil!\n";
        }

        curl_mime_free(mime);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}