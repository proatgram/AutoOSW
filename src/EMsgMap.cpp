#include "EMsgMap.hpp"

#include <fstream>

#include <iostream>

#include "nlohmann/json.hpp"

EMsgMap::EMsgMap(const std::filesystem::path &jsonPath) {
    std::fstream file(jsonPath, std::fstream::in);
    if (!file) {
        throw std::runtime_error(strerror(errno));
    }

    nlohmann::json json = nlohmann::json::parse(file);

    for (const auto &emsgEntry : json) {
        m_emsgs.emplace_back(emsgEntry.at("name"), emsgEntry.at("emsg"), emsgEntry.at("flags"), emsgEntry.at("server_type"));
    }
}

const std::vector<EMsgMap::EMsg>& EMsgMap::GetEMsgs() const {
    return m_emsgs;
}
