#include "ManifestManager.hpp"

#include "DownloadManager.hpp"

#include <iostream>

void downloadManifest(const std::string &downloadUrl, const std::filesystem::path &outPath, const std::string &outFile) {
    DownloadManager downloadManager(downloadUrl, outPath, outFile);
    downloadManager.DownloadFile();
}

ManifestManager::ManifestManager(const std::filesystem::path& vdfPath)
    :
        m_vdfFile(vdfPath),
        m_vdfObject(tyti::vdf::read(m_vdfFile)),
        m_entries()
{
    m_version = m_vdfObject.childs["ubuntu12"]->attribs["version"];
    for (std::pair<const std::basic_string<char>, std::shared_ptr<tyti::vdf::basic_object<char>>> object : m_vdfObject.childs["ubuntu12"]->childs) {
        ManifestEntry entry;
        entry.name = object.first;
        entry.file = object.second->attribs["file"];
        entry.size = object.second->attribs["size"];
        entry.sha2 = object.second->attribs["sha2"];
        entry.zipvz = object.second->attribs["zipvz"];
        entry.sha2vz = object.second->attribs["sha2vz"];

        m_entries.push_back(entry);
    }
}

ManifestManager::ManifestManager(const std::string &downloadUrl, const std::filesystem::path &outPath, const std::string &outFile) :
    m_entries()
{
    if(std::filesystem::exists(outPath.string() + '/' + outFile)) {
        std::cout << "A VDF Manifest file already exists in \"" << outPath.string() + '/' + outFile << "\"" << std::endl;
        char answer;
        while (answer != 'y' && answer != 'n') {
            std::cout << "Would you like to redownload the VDF Manifest? y/n" << std::endl;
            std::cin >> answer;
            answer = std::tolower(answer);

            switch (answer) {
                case 'y': {
                    downloadManifest(downloadUrl, outPath, outFile);
                    break;
                }
                case 'n': {
                    std::cout << "Using VDF Manifest already at " << outPath.string() + '/' + outFile << std::endl;
                    break;
                }
                default: {
                    std::cout << "Unrecognized option '" << answer << "'" << std::endl;  
                    break;
                }
            }
        }
    }
    else {
        downloadManifest(downloadUrl, outPath, outFile);
    }
    
    m_vdfFile.open(outPath.string() + "/" + outFile);
    m_vdfObject = tyti::vdf::read(m_vdfFile);

    m_version = m_vdfObject.childs["ubuntu12"]->attribs["version"];
    for (std::pair<const std::basic_string<char>, std::shared_ptr<tyti::vdf::basic_object<char>>> object : m_vdfObject.childs["ubuntu12"]->childs) {
        ManifestEntry entry;
        entry.name = object.first;
        entry.file = object.second->attribs["file"];
        entry.size = object.second->attribs["size"];
        entry.sha2 = object.second->attribs["sha2"];
        entry.zipvz = object.second->attribs["zipvz"];
        entry.sha2vz = object.second->attribs["sha2vz"];

        m_entries.push_back(entry);
    }
}

ManifestEntry ManifestManager::Get(const std::string &key) const {
    auto iterator = std::find_if(m_entries.cbegin(), m_entries.cend(), [key](ManifestEntry entry) -> bool {return entry.name == key;});
    
    if (iterator == std::end(m_entries)) {
        return ManifestEntry{};
    }

    return *iterator;
}
