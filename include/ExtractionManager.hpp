#pragma once

#include <filesystem>

#include "zip.h"

class ExtractionManager {
    public:
        ExtractionManager(const std::string &archive, const std::filesystem::path &extractionPath, const std::string &extractedName);

        ~ExtractionManager();

        void Extract();

    private:
        std::string m_archive;
        std::filesystem::path m_extractionPath;
        std::string m_extractedName;
        zip_t *m_zipArchive;
};
