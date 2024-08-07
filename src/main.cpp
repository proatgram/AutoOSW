#include "DownloadManager.hpp"
#include "ExtractionManager.hpp"
#include "ManifestManager.hpp"
#include "ClassMap.hpp"
#include "CallbackMap.hpp"

#include "ProtobufDumper/ProtobufDumper.h"

#include "steamworks_dumper.h"
#include "argparse/argparse.hpp"

int main(int argc, char **argv) {
    argparse::ArgumentParser arguments("OSWUpdater");

    /* Handle Arguments */
    arguments.add_argument("--old-headers") // For comparison
        .help("Old dumped header directory");
    arguments.add_argument("--download") // The directory to download files to
        .help("Directory to download and extract files to")
        .default_value("./download");
    arguments.add_argument("--dump") // The directory to dump files to
        .help("Directory to dump files to")
        .default_value("./dumped");
    arguments.add_argument("--header-format") // The format to write header files
        .help("The file extention to write for header files")
        .default_value("h");

    try {
        arguments.parse_args(argc, argv);
    }
    catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << arguments;
        return 1;
    }

    std::string downloadDirectory = arguments.get<std::string>("--download");
    std::string dumpDirectory = arguments.get<std::string>("--dump");
    
    ManifestManager manifestManager(ManifestManager::MANIFEST_URL.data(), std::filesystem::path(downloadDirectory), "client_manifest.vdf"); // Download and manage Client Manifest VDF

    std::string ubuntu12DownloadURL = "https://client-update.akamai.steamstatic.com/" + manifestManager.Get("bins_ubuntu12").file;
    DownloadManager ubuntu_12Download(ubuntu12DownloadURL, downloadDirectory, manifestManager.Get("bins_ubuntu12").file);
    ubuntu_12Download.DownloadFile(); // Download client libs
    
    std::string ubuntu_12ArchiveLocation = ubuntu_12Download.GetDownloadDirectory().string() + '/' + ubuntu_12Download.GetDownloadFileName();
    ExtractionManager ubuntu_12Extraction(ubuntu_12ArchiveLocation, std::filesystem::path(downloadDirectory + "/extracted/"), manifestManager.Get("bins_ubuntu12").name);
    ubuntu_12Extraction.Extract(); // Extract client libs

    /* Dumps from steamclient.so using steamworks_dumper as a library */
    std::string steamclientlib = ubuntu_12Extraction.GetFullOutputPath().string() + '/' + "ubuntu12_32/steamclient.so";

    if (!std::filesystem::exists(dumpDirectory)) {
        std::filesystem::create_directories(dumpDirectory);
    }

    Dump(steamclientlib, dumpDirectory, true); // Used to be main() of steamworks_dumper
    ProtobufDumper::ProtobufDumper::DumpProtobufs({std::filesystem::path(steamclientlib)}, dumpDirectory + "/protobufs/"); // Dumps protobufs

    ClassMap clientUser(dumpDirectory + '/' + "IClientUserMap.json");
    CallbackMap UserCallbacks(dumpDirectory + '/' + "callbacks.json", 100, "SteamUserCallbacks");
}
