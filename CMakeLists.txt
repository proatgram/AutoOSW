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
add_executable(OSWUpdate
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/OSWUpdate/main.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/OSWUpdate/ManifestManager.cpp>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/OSWUpdate/ManifestManager.hpp>
)

target_link_libraries(OSWUpdate
    PRIVATE
        ValveFileVDF::ValveFileVDF
        argparse
        ${CURL_LIBRARY_RELEASE}
)

target_include_directories(OSWUpdate
    PRIVATE
        ${CURL_INCLUDE_DIR}
)
