#include "CallbackMap.hpp"

#include <fstream>

#include "nlohmann/json.hpp"

CallbackMap::JsonCallback::JsonCallback(std::size_t id, const std::string &name, std::size_t size, const std::vector<int32_t> &postedAt) :
    id(id),
    name(name),
    size(size),
    postedAt(postedAt)
{

}

CallbackMap::CallbackMap(const std::filesystem::path &jsonPath, std::size_t callbackTypeMask, const std::string &callbackTypeName) :
    m_callbackTypeMask(callbackTypeMask),
    m_callbackTypeName(callbackTypeName),
    m_callbacks()
{
    std::fstream file(jsonPath, std::fstream::in);
    if (!file) {
        throw std::runtime_error(strerror(errno));
    }

    nlohmann::json json = nlohmann::json::parse(file);

    for (auto callback : json) {
        std::vector<int32_t> postedAt{};
        for (auto posted : callback.at("posted_at")) {
            postedAt.push_back(std::stoi(posted.get<std::string>()));
        }

        m_callbacks.emplace_back(
            callback.at("id").get<std::size_t>(),
            callback.at("name").get<std::string>(),
            callback.at("size").get<std::size_t>(),
            postedAt
        );
    }
}

const std::vector<CallbackMap::JsonCallback>& CallbackMap::GetCallbacks() const {
    return m_callbacks;
}

std::size_t CallbackMap::GetCallbackTypeMask() const {
    return m_callbackTypeMask;
}

const std::string& CallbackMap::GetCallbackTypeName() const {
    return m_callbackTypeName;
}
