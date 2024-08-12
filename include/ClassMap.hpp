#pragma once

#include <vector>
#include <filesystem>

/**
 * A class that encapsulates a Steam Class
 *
 */
class ClassMap {
    public:
        /**
         * Represents a Function in the JSON dump
         *
         */
        struct JsonFunction {
            JsonFunction(const std::string &name, std::size_t argc, int interfaceId, long int functionId, long int fencepost, bool cannotCallInCrossProcess, int32_t address, const std::vector<std::string> &serializedReturns, const std::vector<std::string> &serializedArgs);

            std::string name;
            // argc is unreliable in steamworks_dumper it seems
            // Don't rely on it
            std::size_t argc;

            int interfaceId;
            long int functionId;
            long int fencepost;

            bool cannotCallInCrossProcess;

            int32_t address;

            std::vector<std::string> serializedReturns;
            std::vector<std::string> serializedArgs;
        };

        /**
         * @brief Constructs a new `ClassMap` instance
         *
         * @param jsonPath The path to the Class Map file
         *
         * The Class Map files usually take the form of
         * "IClient_____Map.json" or usually have an "I"
         * as the prefix to the file.
         *
         */
        ClassMap(std::filesystem::path jsonPath);

        /**
         * @brief Gets the functions in the class
         *
         * @return std::vector<JsonFunction> A `std::vector` of functions encapsulates as a `JsonFunction`
         *
         */
        const std::vector<JsonFunction>& GetFunctions() const;

        /**
         * @brief Gets the name of the Steam Class
         *
         * @return std::string The name of the Steam Class
         *
         */
        const std::string& GetClassName() const;

        /**
         * @brief Gets the address in the steamclient.so where the class was found at
         *
         * @return int32_t The address where the class was found at
         *
         */
        int32_t GetFoundAt() const;

    private:
        std::string m_jsonClassName;
        std::vector<JsonFunction> m_functions;
        int32_t m_foundAt;
};
