#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include <curl/curl.h>

/**
 * @brief Manages a download for a single file using CURL
 *
 */
class DownloadManager {
    public:

        /**
         * @brief Initialize a `DownloadManager` instance
         *
         * @param downloadUrl The URL to download from
         * @param downloadPath The path to download to
         * @param fileName The name of the file to download to
         *
         */
        DownloadManager(const std::string &downloadUrl, const std::filesystem::path &downloadPath, const std::string &fileName);

        ~DownloadManager();

        /**
         * @brief Attempts to download the file
         *
         * @return CURLcode a `CURLcode` showing the status of the operation
         *
         */
        CURLcode DownloadFile();

        /**
         * @brief Gets the underlying file stream
         *
         * @return std::fstream A reference to the underlying file stream
         *
         */
        std::fstream& GetFile();

        /**
         * @brief Gets the download URL
         *
         * @return std::string The download URL
         *
         */
        const std::string& GetDownloadUrl() const;

        /**
         * @brief Gets the download directory
         *
         * @return std::filesystem::path The download directory
         *
         */
        const std::filesystem::path& GetDownloadDirectory() const;

        /**
         * @brief Gets the download filename
         *
         * @return std::string The download filename
         *
         */
        const std::string& GetDownloadFileName() const;

    private:
        std::filesystem::path m_downloadDirectory;
        std::string m_downloadUrl;
        std::string m_downloadFileName;
        CURL *m_curlHandle;
        std::fstream m_file;
};
