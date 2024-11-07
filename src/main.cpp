#include "DownloadManager.hpp"
#include "ExtractionManager.hpp"
#include "ManifestManager.hpp"
#include "HeaderManager.hpp"
#include "ClassMap.hpp"
#include "CallbackMap.hpp" // Callbacks seem really hit or miss for steamworks_dumper, not sure how I'd properly dump and update them

#include "ProtobufDumper/ProtobufDumper.h"

#include "steamworks_dumper.h"
#include "argparse/argparse.hpp"

int main(int argc, char **argv) {
    argparse::ArgumentParser arguments("OSWUpdater");

    /* Handle Arguments */
    arguments.add_argument("--old-headers") // For comparison
        .help("Old dumped header directory")
        .required();
    arguments.add_argument("--old-dump")
        .help("Old dumped JSON Maps directory"); // Also for comparison
    arguments.add_argument("--download") // The directory to download files to
        .help("Directory to download and extract files to")
        .default_value("./download");
    arguments.add_argument("--dump") // The directory to dump files to
        .help("Directory to dump files to")
        .default_value("./dumped");
    arguments.add_argument("--header-format") // The format to write header files
        .help("The file extention to write for header files")
        .default_value("hpp");
    arguments.add_argument("--generated") // The directory to write generated files to
        .help("The directory to write generated files to")
        .default_value("./generated");
    arguments.add_argument("--protobufs-in-generated")
        .help("Puts the dumped Protobuf files in the generation directory")
        .flag();

    try {
        arguments.parse_args(argc, argv);
    }
    catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << arguments;
        return 1;
    }

    /* Arguments */
    // Defaulted or required arguments
    std::string downloadDirectory = arguments.get<std::string>("--download");
    std::string dumpDirectory = arguments.get<std::string>("--dump");
    std::string generatedDirectory = arguments.get<std::string>("--generated");
    std::string headerFormat = arguments.get<std::string>("--header-format");
    std::string oldHeaderDirectory = arguments.get<std::string>("--old-headers");

    // Optional arguments
    std::optional<std::string> oldDumpDirectory = arguments.present<std::string>("--old-dump");

    // Constructor downloads, class manages Client Manifest VDF
    ManifestManager manifestManager(ManifestManager::MANIFEST_URL.data(), std::filesystem::path(downloadDirectory), "client_manifest.vdf");

    std::string ubuntu12DownloadURL = "https://client-update.akamai.steamstatic.com/" + manifestManager.Get("bins_ubuntu12").file;
    DownloadManager ubuntu_12Download(ubuntu12DownloadURL, downloadDirectory, manifestManager.Get("bins_ubuntu12").file);
    ubuntu_12Download.DownloadFile(); // Download client libs
    
    std::string ubuntu_12ArchiveLocation = ubuntu_12Download.GetDownloadDirectory().string() + '/' + ubuntu_12Download.GetDownloadFileName();
    ExtractionManager ubuntu_12Extraction(ubuntu_12ArchiveLocation, std::filesystem::path(downloadDirectory + "/extracted/"), manifestManager.Get("bins_ubuntu12").name);
    ubuntu_12Extraction.Extract(); // Extract client libs

    std::string steamclientlib = ubuntu_12Extraction.GetFullOutputPath().string() + "/ubuntu12_32/steamclient.so";

    if (!std::filesystem::exists(dumpDirectory)) {
        std::filesystem::create_directories(dumpDirectory);
    }

    /* Dumps Protobufs and function information from steamclient.so using steamworks_dumper and ProtobufDumper */
    Dump(steamclientlib, dumpDirectory, true); // Dump actual function information
    ProtobufDumper::ProtobufDumper::DumpProtobufs({std::filesystem::path(steamclientlib)}, (arguments.get<bool>("--protobufs-in-generated") ? generatedDirectory : dumpDirectory) + "/Protobuf/"); // Dumps protobufs

    /* Handle generating header file information */
    // TODO: If a map dump and a header have a nape mismatch (but still have the same functions) DEAL WITH IT (somehow)
    
    // Generates the Client Interface headers
    for (const std::filesystem::path &currentNewDumpFile : std::filesystem::directory_iterator(dumpDirectory)) {
        if (!currentNewDumpFile.has_filename() || currentNewDumpFile.filename().string().substr(0, 7) != "IClient") {
            continue;
        }

        // Find a match for the JSON Dump in the old headers
        std::optional<std::filesystem::path> matchedOldHeader;
        for (const std::filesystem::path &match : std::filesystem::directory_iterator(oldHeaderDirectory)) {
            if (currentNewDumpFile.filename().string().substr(0, currentNewDumpFile.filename().string().find("Map.json")).append(".h") == match.filename().string()) {
                matchedOldHeader = match;
            }
        }

        // Finds a match for the JSON Dump in the old JSON Dumps
        std::optional<std::filesystem::path> matchedOldDump;
        if (oldDumpDirectory.has_value()) {
            for (const std::filesystem::path &match : std::filesystem::directory_iterator(oldDumpDirectory.value())) {
                if (currentNewDumpFile.filename() == match.filename()) {
                    matchedOldDump = match;
                }
            }
        }

        HeaderManager<ClassMap> header(currentNewDumpFile, generatedDirectory + "/Interfaces/Client/" + currentNewDumpFile.filename().string().substr(0, currentNewDumpFile.filename().string().find("Map.json")).append(".").append(headerFormat), matchedOldDump, matchedOldHeader);
        header.GenerateHeader();
    }

    HeaderManager<EMsgMap> header(std::filesystem::path(dumpDirectory).append("emsg_list.json"), std::filesystem::path(generatedDirectory).append("EMsg").append(std::string("EMsgs.").append(headerFormat)), std::nullopt);
    header.GenerateHeader();
}
