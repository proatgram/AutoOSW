#pragma once

#include <filesystem>

#include "zip.h"

/**
 * @brief Manages a ZIP archive
 *
 */
class ExtractionManager {
    public:

        /**
         * @brief Initializes a `ExtractionManager` instance
         *
         * @param archive The source archive
         * @param extractionPath The directory to extract the archive to
         * @param extractedname A directory to extract the content to
         *
         * Output path will take the form of extractionPath/extractedName/ZIP_DATA
         *
         */
        ExtractionManager(const std::string &archive, const std::filesystem::path &extractionPath, const std::string &extractedName);

        ~ExtractionManager();

        /**
         * @brief Extracts the content managed by the `ExtractionManager` instance
         *
         * @return void
         *
         */
        void Extract();

        /**
         * @brief Gets the archive managed by the `ExtractionManager`
         *
         * @return std::string The path to the file managed
         *
         */
        const std::string& GetSourceArchive() const;

        /**
         * @brief Gets the full output path
         *
         * Will take the form of extractionPath/extractedName
         *
         * @return std::filesystem::path The path to the full output
         *
         */
        std::filesystem::path GetFullOutputPath() const;

        /**
         * @brief Gets the output path
         *
         * Takes the form of extractionPath
         *
         * @return std::filesystem::path The path to the output
         *
         */
        std::filesystem::path GetOutputPath() const;

        /**
         * @brief Gets the extracted name
         *
         * Takes the form of extractedName
         *
         * @return std::string The name of the extracted content
         *
         */
        const std::string& GetExtractionName() const;

    private:
        std::string m_archive;
        std::filesystem::path m_extractionPath;
        std::string m_extractedName;
        zip_t *m_zipArchive;
};
