#include "ExtractionManager.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

ExtractionManager::ExtractionManager(const std::string &archive, const std::filesystem::path &extractionPath, const std::string &extractedName) :
    m_archive(archive),
    m_extractionPath(extractionPath),
    m_extractedName(extractedName)
{
    int errCode;
    zip_error zipError;
    if ((m_zipArchive = zip_open(archive.c_str(), ZIP_RDONLY, &errCode)) == nullptr) {
        zip_error_init_with_code(&zipError, errCode);
        throw std::runtime_error(zip_error_strerror(&zipError));
    }
}

ExtractionManager::~ExtractionManager() {
    zip_close(m_zipArchive);
}

void ExtractionManager::Extract() {
    std::cout << "Inflating " << m_archive << std::endl;
    for (std::size_t i = 0; i < zip_get_num_entries(m_zipArchive, 0); i++) {
        zip_stat_t stat;
        if(zip_stat_index(m_zipArchive, i, 0, &stat) != 0) {
            zip_error zipError;
            int errCode;
            zip_error_init_with_code(&zipError, errCode);
            throw std::runtime_error(zip_error_strerror(&zipError));
        }

        std::string name = stat.name;
        std::string fileLocation(m_extractionPath.string() + '/' + m_extractedName + '/' + name);

        // Handle directories
        if (name.at(name.length() - 1) == '/') {
            std::cout << "Creating directory " << fileLocation;
            std::filesystem::create_directories(fileLocation);
        }
        // Handle files
        else {
            std::cout << "Extracting file " << name << " as " << fileLocation << std::endl;
            zip_file_t *zipFile = zip_fopen_index(m_zipArchive, i, 0);
            if (!zipFile) {
                zip_error zipError;
                int errCode;
                zip_error_init_with_code(&zipError, errCode);
                throw std::runtime_error(zip_error_strerror(&zipError));
            }

            if (std::filesystem::exists(fileLocation)) {
                std::cout << "File " << fileLocation << " exists. Skipping." << std::endl;
                continue;
            }

            std::fstream file(fileLocation, std::fstream::binary | std::fstream::out);
            if (!file) {
                throw std::runtime_error("Unable to create file " + fileLocation + ": " + std::strerror(errno));
            }

            long long int sum = 0;
            while (sum != stat.size) {
                char buf[100];

                int len = zip_fread(zipFile, buf, 100);
                if (len < 0) {
                    continue; // again... uhhh...
                }

                file.write(buf, len);
                sum += len;
            }
            
            zip_fclose(zipFile);
            file.flush();
        }
    }
    std::cout << "Extracted " << m_archive << std::endl;
}
