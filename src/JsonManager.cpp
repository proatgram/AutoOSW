#include "JsonManager.hpp"

#include <fstream>

#include "nlohmann/json.hpp"

JsonManager::JsonFunction::JsonFunction(const std::string &name, std::size_t argc, int32_t interfaceId, int32_t functionId, int32_t fencepost, bool cannotCallInCrossProcess, int32_t address, std::type_index serializedReturn, const std::vector<std::type_index> &serializedArgs) :
    name(name),
    argc(argc),
    interfaceId(interfaceId),
    functionId(functionId),
    fencepost(fencepost),
    cannotCallInCrossProcess(cannotCallInCrossProcess),
    address(address),
    serializedReturn(serializedReturn),
    serializedArgs(serializedArgs)
{

}

JsonManager::JsonManager(const std::filesystem::path jsonPath) :
    m_functions()
{
    std::fstream file(jsonPath, std::fstream::in);
    if (!file) {
        throw std::runtime_error(strerror(errno));
    }

    nlohmann::json json = nlohmann::json::parse(file);

    m_jsonClassName = json.at("name").get<std::string>();
    m_jsonClassName.resize(m_jsonClassName.find_last_of("Map"));
    m_foundAt = std::stoi(json.at("found_at").get<std::string>());
    
    for (auto function : json.at("functions")) {
        auto args = function.at("serializedargs");
        auto returns = function.at("serializedreturns");
        std::vector<std::type_index> typeArgs;

        for (std::string argType : args) {
            typeArgs.push_back(JsonManager::GetTypeFromString(argType));
        }
        
        m_functions.emplace_back(
            function.at("name").get<std::string>(),
            function.at("argc").get<std::size_t>(),
            function.at("interface_id").get<int32_t>(),
            function.at("function_id").get<int32_t>(),
            function.at("fencepost").get<int32_t>(),
            function.at("cannotcallincrossproccess").get<bool>(),
            std::stoi(function.at("addr").get<std::string>()),
            (returns.empty() ? typeid(void) : JsonManager::GetTypeFromString(returns.at(0).get<std::string>())),
            typeArgs
        );
    }
}

std::type_index JsonManager::GetTypeFromString(const std::string &typeString) {

}
