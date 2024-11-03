#pragma once

#include <filesystem>
#include <vector>

/**
 * A class that encapsulates Steam EMsg's
 *
 */
class EMsgMap {
    public:
        /**
         * Represents an EMsg in the JSON Map
         *
         */
        struct EMsg {
            inline EMsg(const std::string &name, int emsg, int flags, int serverType) :
                name(name),
                emsg(emsg),
                flags(flags),
                serverType(serverType) {}

            std::string name;

            int emsg;
            int flags;
            int serverType;
        };

        /**
         * @brief Constructs a new `EMsgMap` instance
         *
         * @param jsonPath The path the the new EMsg Map file
         *
         * The EMsg Map file will usually take the form of
         * "emsg_list.json"
         *
         */
        EMsgMap(const std::filesystem::path &jsonPath);

        /**
         * @brief Default constructs a new `EMsgMap` instance
         *
         */
        EMsgMap() = default;

        /**
         * @brief Gets all the EMsgs
         *
         * @return std::vector<EMsg> A `std::vector` of EMsgs encapsulated as a `EMsg`
         *
         */
        const std::vector<EMsg>& GetEMsgs() const;

    private:
        std::vector<EMsg> m_emsgs;
};
