#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

#include "vdf_parser.hpp"

struct ManifestEntry {
    std::string name;

    std::string file;
    std::string size;
    std::string sha2;
    std::string zipvz;
    std::string sha2vz;
};

/**
 * @brief Manages a Manifest file that comes in the form of a VDF
 * 
 */
class ManifestManager {
    public:
        /**
         * @brief Open a VDF file
         *
         * @param vdfFile The path to the file
         *
         */
        ManifestManager(const std::filesystem::path& vdfFile);

        /**
         * @brief Downloads a VDF Manifest file from `downloadUrl` to `outFile`
         *
         * @param downloadUrl The Url to download from
         * @param outPath The path to write `outFile` to
         * @param outFile The destination file to download to
         * 
         */
        ManifestManager(const std::string& downloadUrl, const std::filesystem::path& outPath, const std::string &outFile);

        /**
         * @brief Gets an entry from the Manifest VDF in the form of a `ManifestEntry`
         *
         * @param key The name or key of the entry
         *
         * @return ManifestEntry A valid `ManifestEntry` if found or a default constructed `ManifestEntry` if not found
         *
         */
        ManifestEntry Get(const std::string &key) const;

        const std::vector<ManifestEntry>& GetEntries() const;

        // Recomended VDF Manifest, can specify different one in constructor though.
        static constexpr std::string_view MANIFEST_URL = "https://raw.githubusercontent.com/SteamDatabase/SteamTracking/master/ClientManifest/steam_client_ubuntu12";
    private:
        std::fstream m_vdfFile;
        tyti::vdf::object m_vdfObject;

        std::string m_version;
        std::vector<ManifestEntry> m_entries;
};
