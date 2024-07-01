#pragma once

#include <vector>
#include <filesystem>

class ClassMap {
    public:
        struct JsonFunction {
            JsonFunction(const std::string &name, std::size_t argc, int interfaceId, long int functionId, long int fencepost, bool cannotCallInCrossProcess, int32_t address, std::string serializedReturn, const std::vector<std::string> &serializedArgs);

            std::string name;
            // argc is unreliable in steamworks_dumper it seems
            // Don't rely on it
            std::size_t argc;

            int interfaceId;
            long int functionId;
            long int fencepost;

            bool cannotCallInCrossProcess;

            int32_t address;

            std::string serializedReturn;
            std::vector<std::string> serializedArgs;
        };

        ClassMap(std::filesystem::path jsonPath);

        const std::vector<JsonFunction>& GetFunctions() const;

        const std::string& GetClassName() const;

        int32_t GetFoundAt() const;

    private:
        std::string m_jsonClassName;
        std::vector<JsonFunction> m_functions;
        int32_t m_foundAt;
};
