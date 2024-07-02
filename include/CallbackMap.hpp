#pragma once

#include <cstddef>
#include <string>
#include <filesystem>
#include <vector>

/**
 * A class that encapsulates Steam Callbacks
 *
 */
class CallbackMap {
    public:

        /**
         * The callback
         *
         */
        struct JsonCallback {
            JsonCallback(std::size_t id, const std::string &name, std::size_t size, const std::vector<int32_t> &postedAt);

            std::size_t id;
            std::string name;
            std::size_t size;
            std::vector<int32_t> postedAt;
        };

        /**
         * @brief Constructs a new `CallbackMap` instance
         *
         * @param jsonPath The path to the callback file
         * @param callbackTypeMask A mask for the callback type
         * @param callbackTypeName The name of the callback type
         *
         */
        CallbackMap(const std::filesystem::path &jsonPath, std::size_t callbackTypeMask, const std::string &callbackTypeName);

        /**
         * @brief Gets the callbacks
         *
         * @return std::vector<JsonCallback> The callbacks available
         *
         */
        const std::vector<JsonCallback>& GetCallbacks() const;

        /**
         * @brief Gets the Callback Type Mask
         *
         * @return std::size_t The Callback Type Mask
         *
         */
        std::size_t GetCallbackTypeMask() const;

        /**
         * @brief Gets the Callback Type Name
         *
         * @return std::string The Callback Type Name
         *
         */
        const std::string& GetCallbackTypeName() const;

    private:
        std::size_t m_callbackTypeMask;
        std::string m_callbackTypeName;
        std::vector<JsonCallback> m_callbacks;
};
