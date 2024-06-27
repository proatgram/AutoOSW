#include "DownloadManager.hpp"
#include <iostream>

static std::size_t curlDownloadCallback(char *data, std::size_t dataSize, std::size_t dataCount, void *instance) {
    DownloadManager* downloadManager = static_cast<DownloadManager*>(instance);
    if (!downloadManager) {
        return -1;
    }

    std::fstream &file = downloadManager->GetFile();
    if (!file.is_open()) {
        // Make sure the directory exists before we open a file to it.
        if (!std::filesystem::exists(downloadManager->GetDownloadDirectory())) {
            std::filesystem::create_directory(downloadManager->GetDownloadDirectory());
        }

        file.open(downloadManager->GetDownloadDirectory().string() + '/' + downloadManager->GetDownloadFileName(), std::fstream::trunc | std::fstream::out);
        
        // If there is a failure opening the file, then we return -1 to signal to libcurl an issue.
        if (!file.is_open()) {
            return -1;
        }
    }
    std::char_traits<char>::pos_type positionBefore = file.tellp();

    file.write(data, dataSize * dataCount);

    std::char_traits<char>::pos_type numberOfBytesWritten = file.tellp() - positionBefore;
    return static_cast<std::size_t>(numberOfBytesWritten);
}

DownloadManager::DownloadManager(const std::string &downloadUrl, const std::filesystem::path &downloadPath, const std::string &fileName) :
    m_downloadDirectory(downloadPath),
    m_downloadUrl(downloadUrl),
    m_downloadFileName(fileName)
{
    m_curlHandle = curl_easy_init();
    if (!m_curlHandle) {
        throw std::runtime_error("Failed to init CURL handle.");
    }

    curl_easy_setopt(m_curlHandle, CURLOPT_URL, downloadUrl.c_str());
    curl_easy_setopt(m_curlHandle, CURLOPT_WRITEFUNCTION, &curlDownloadCallback);
    curl_easy_setopt(m_curlHandle, CURLOPT_WRITEDATA, this);
}

DownloadManager::~DownloadManager() {
    curl_easy_cleanup(m_curlHandle);
}

CURLcode DownloadManager::DownloadFile() {
    return curl_easy_perform(m_curlHandle);
}

std::fstream& DownloadManager::GetFile() {
    return m_file;
}

const std::string& DownloadManager::GetDownloadUrl() const {
    return m_downloadUrl;
}

const std::filesystem::path& DownloadManager::GetDownloadDirectory() const {
    return m_downloadDirectory;
}

const std::string& DownloadManager::GetDownloadFileName() const {
    return m_downloadFileName;
}
