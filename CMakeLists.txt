cmake_minimum_required(VERSION 3.12.0)

project(AutoOSW
    VERSION 0.1
    DESCRIPTION "An updater and header generation tool for OpenSteamworks"
    HOMEPAGE_URL "https://github.com/proatgram/AutoOSW"
    LANGUAGES CXX
)

include(FetchContent)

fetchcontent_declare(
    ValveFileVDF
    GIT_REPOSITORY "https://github.com/TinyTinni/ValveFileVDF.git"
    GIT_TAG "v1.0.0"
)

fetchcontent_declare(
    argparse
    GIT_REPOSITORY "https://github.com/p-ranav/argparse.git"
    GIT_TAG "v3.0"
)

fetchcontent_makeavailable(ValveFileVDF argparse)

find_package(CURL REQUIRED)

# Target OSWUpdate
add_executable(AutoOSW
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/main.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/ManifestManager.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/ManifestManager.hpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/DownloadManager.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/DownloadManager.hpp>
)

target_link_libraries(AutoOSW
    PRIVATE
        ValveFileVDF::ValveFileVDF
        argparse
        ${CURL_LIBRARY_RELEASE}
)

target_include_directories(AutoOSW
    PRIVATE
        ${CURL_INCLUDE_DIR}
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)
