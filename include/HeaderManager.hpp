#pragma once

#include <filesystem>
#include <optional>
#include <map>
#include <list>
#include <string>
#include <sstream>

#include "ClassMap.hpp"

class HeaderManager {
    public:
        HeaderManager(const std::filesystem::path &jsonMapPath, const std::optional<std::filesystem::path> &oldHeader = std::nullopt);

        void GenerateHeader(const std::filesystem::path &out);
    
    private:
        void AnalyzeOldHeader();

        static std::string_view GetCppType(const std::string &dumpedTypeName);

        ClassMap m_jsonClassMap;

        std::stringstream m_generatedHeader;

        std::list<std::string> m_systemIncludes;
        std::list<std::string> m_localIncludes;
        
        std::optional<std::map<ClassMap::JsonFunction, std::map<std::string, std::string>>> m_mappedArgumentNames;
        std::optional<std::filesystem::path> m_oldHeader;

        static constexpr std::string_view GENERATED_NOTICE_MSG = R"(
            /*****************************  Open SteamWorks  *********************************\
            /*                                                                               *\
            /*  This file is part of the Open SteamWorks project. All individuals associated *\
            /*  with this project do not claim ownership of the contents                     *\
            /*                                                                               *\
            /*  The code, comments, and all related files, projects, resources,              *\
            /*  redistributables included with this project are Copyright Valve Corporation. *\
            /*  Additionally, Valve, the Valve logo, Half-Life, the Half-Life logo, the      *\
            /*  Lambda logo, Steam, the Steam logo, Team Fortress, the Team Fortress logo,   *\
            /*  Opposing Force, Day of Defeat, the Day of Defeat logo, Counter-Strike, the   *\
            /*  Counter-Strike logo, Source, the Source logo, and Counter-Strike Condition   *\
            /*  Zero are trademarks and or registered trademarks of Valve Corporation.       *\
            /*  All other trademarks are property of their respective owners.                *\
            /*                                                                               *\
            /*********************************  !NOTICE!  ************************************\
            /*                                                                               *\
            /*  This is auto-generated code! There may be problems with it or it may be      *\
            /*  wrong. Use at your own risk!                                                 *\
            /*                                                                               *\
            /*********************************************************************************\)";

};
