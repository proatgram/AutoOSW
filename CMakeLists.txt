cmake_minimum_required(VERSION 3.12.0)

project(AutoOSW
    VERSION 0.1
    DESCRIPTION "An updater and header generation tool for OpenSteamworks"
    HOMEPAGE_URL "https://github.com/proatgram/AutoOSW"
    LANGUAGES CXX
)

include(FetchContent)

Set(FETCHCONTENT_QUIET FALSE)

fetchcontent_declare(
    ValveFileVDF
    GIT_REPOSITORY "https://github.com/TinyTinni/ValveFileVDF.git"
    GIT_TAG "v1.0.0"
    GIT_PROGRESS TRUE
)

fetchcontent_declare(
    argparse
    GIT_REPOSITORY "https://github.com/p-ranav/argparse.git"
    GIT_TAG "v3.0"
    GIT_PROGRESS TRUE
)

fetchcontent_declare(
    nlohmann_json
    GIT_REPOSITORY "https://github.com/nlohmann/json.git"
    GIT_TAG "v3.11.3"
    GIT_PROGRESS TRUE
)

fetchcontent_declare(
    steamworks_dumper
    GIT_REPOSITORY "https://github.com/proatgram/steamworks_dumper.git"
    GIT_PROGRESS TRUE
)

fetchcontent_makeavailable(ValveFileVDF argparse nlohmann_json steamworks_dumper)

find_package(CURL REQUIRED)
find_package(LIBZIP REQUIRED)

# Target OSWUpdate
add_executable(AutoOSW
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/main.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/ManifestManager.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/ManifestManager.hpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/DownloadManager.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/DownloadManager.hpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/ExtractionManager.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/EMsgMap.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/ExtractionManager.hpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/ClassMap.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/ClassMap.hpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/CallbackMap.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/CallbackMap.hpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/HeaderManager.hpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/EMsgMap.hpp>
)

target_link_libraries(AutoOSW
    PRIVATE
        ValveFileVDF::ValveFileVDF
        argparse
        libzip::zip
        nlohmann_json::nlohmann_json
        steamworks_dumper
        protobuf_dumper
        ${CURL_LIBRARY_RELEASE}
)

target_include_directories(AutoOSW
    PRIVATE
        ${CURL_INCLUDE_DIR}
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)
