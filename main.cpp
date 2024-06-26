#include "argparse/argparse.hpp"

#include "ManifestManager.hpp"

int main(int argc, char **argv) {
    argparse::ArgumentParser arguments("OSWUpdater");

    // Handle arguments
    arguments.add_argument("--headers")
        .help("Dumped header directory");
    arguments.add_argument("--download-dir")
        .help("Directory to download and extract files to")
        .default_value("./download");
    arguments.add_argument("--dump-dir")
        .help("Directory to dump files to")
        .default_value("./extracted");

    try {
        arguments.parse_args(argc, argv);
    }
    catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << arguments;
        return 1;
    }
    
    ManifestManager manifestManager(ManifestManager::MANIFEST_URL.data(), std::filesystem::path(arguments.get<std::string>("--download-dir")), "client_manifest.vdf");
}
