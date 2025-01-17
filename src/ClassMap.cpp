#include "ClassMap.hpp"

#include <fstream>

#include "nlohmann/json.hpp"

ClassMap::JsonFunction::JsonFunction(const std::string &name, std::size_t argc, int interfaceId, long int functionId, long int fencepost, bool cannotCallInCrossProcess, int32_t address, const std::vector<std::string> &serializedReturns, const std::vector<std::string> &serializedArgs) :
    name(name),
    argc(argc),
    interfaceId(interfaceId),
    functionId(functionId),
    fencepost(fencepost),
    cannotCallInCrossProcess(cannotCallInCrossProcess),
    address(address),
    serializedReturns(serializedReturns),
    serializedArgs(serializedArgs)
{

}

ClassMap::ClassMap(const std::filesystem::path jsonPath) :
    m_functions()
{
    std::fstream file(jsonPath, std::fstream::in);
    if (!file) {
        throw std::runtime_error(strerror(errno));
    }

    nlohmann::json json = nlohmann::json::parse(file);

    m_jsonClassName = json.at("name").get<std::string>();
    m_jsonClassName.resize(m_jsonClassName.find("Map"));
    m_foundAt = std::stoi(json.at("found_at").get<std::string>());
    
    for (auto function : json.at("functions")) {
        auto args = function.at("serializedargs");
        auto returns = function.at("serializedreturns");
        std::vector<std::string> typeArgs;
        std::vector<std::string> typeReturns;

        for (const std::string &argType : args) {
            typeArgs.push_back(argType);
        }
        for (const std::string &returnType : returns) {
            typeReturns.push_back(returnType);
        }
        
        m_functions.emplace_back(
            function.at("name").get<std::string>(),
            std::stoi(function.at("argc").get<std::string>()),
            std::stoi(function.at("interfaceid").get<std::string>()),
            std::stol(function.at("functionid").get<std::string>()),
            std::stol(function.at("fencepost").get<std::string>()),
            (function.at("cannotcallincrossprocess").get<std::string>() == "0" ? false : true),
            std::stoi(function.at("addr").get<std::string>()),
            typeReturns,
            typeArgs
        );
    }
}

const std::vector<ClassMap::JsonFunction>& ClassMap::GetFunctions() const {
    return m_functions;
}

const std::string& ClassMap::GetClassName() const {
    return m_jsonClassName;
}

int32_t ClassMap::GetFoundAt() const {
    return m_foundAt;
}
