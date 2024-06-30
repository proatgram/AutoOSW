#pragma once

#include <typeindex>
#include <typeinfo>
#include <vector>
#include <filesystem>

class JsonManager {
    public:
        struct JsonFunction {
            JsonFunction(const std::string &name, std::size_t argc, int32_t interfaceId, int32_t functionId, int32_t fencepost, bool cannotCallInCrossProcess, int32_t address, std::type_index serializedReturn, const std::vector<std::type_index> &serializedArgs);

            std::string name;
            // argc is unreliable in steamworks_dumper it seems
            // Don't rely on it
            std::size_t argc;

            int32_t interfaceId;
            int32_t functionId;
            int32_t fencepost;

            bool cannotCallInCrossProcess;

            int32_t address;

            std::type_index serializedReturn;
            std::vector<std::type_index> serializedArgs;
        };

        JsonManager(std::filesystem::path jsonPath);

        const std::vector<JsonFunction>& GetFunctions() const;

        const std::string& GetClassName() const;

        int32_t GetFoundAt() const;

    private:
        std::string m_jsonClassName;
        std::vector<JsonFunction> m_functions;
        int32_t m_foundAt;

        static std::type_index GetTypeFromString(const std::string &typeString);
};
