#pragma once

#include <filesystem>
#include <optional>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>

#include "ClassMap.hpp"

class HeaderManagerBase {
    public:
        inline HeaderManagerBase(const std::filesystem::path &jsonMapPath, const std::optional<std::filesystem::path> &oldHeader) :
            m_jsonMap(jsonMapPath),
            m_generatedHeader(),
            m_oldHeader(oldHeader)
        {
            
        }


        inline void GenerateHeader(const std::filesystem::path &out) {
            if (m_oldHeader.has_value()) {
                AnalyzeOldHeader();
            }

            m_generatedHeader << GENERATED_NOTICE_MSG << "\n" << std::endl;
            m_generatedHeader << "#pragma once" << "\n" << std::endl;

            for (const std::string &include : m_systemIncludes) {
                m_generatedHeader << "#include <" << include << ">" << std::endl;
            }

            m_generatedHeader << std::endl;

            for (const std::string &include : m_localIncludes) {
                m_generatedHeader << "#include \"" << include << "\"" << std::endl;
            }

            GenerateContent();
        }

    
    protected:
        inline static const std::regex FUNCTION_REGEX = std::regex(R"((?:[\w\[\]]+\s+)+[\w\[\]]*\s*\([^)]*\)\s*(?:const)?\s*(?:->\s*[\w\[\]]+)?\s*(?:final)?\s*;?)");
        virtual void AnalyzeOldHeader() = 0;

        virtual void GenerateContent() = 0;

        inline static std::string GetCppType(const std::string &dumpedTypeName) {
            return {};
        }

        inline static bool IsFunctionDecleration(const std::string &line) {
            return std::regex_match(line, FUNCTION_REGEX);
        }

        static inline std::vector<std::string> GetParameterNames(const std::string &line) {
            std::vector<std::string> parameterNames;
            static const std::regex PARAMETER_PATTERN(R"(\(([^)]*)\))");
            std::smatch match;

            if (std::regex_search(line, match, PARAMETER_PATTERN)) {
                std::string parameters = match[1];
                static const std::regex PARAM_REGEX(R"(([\w\[\]&\*]+)\s+([\w\[\]&\*]+)?\s*(,|$))");
                auto paramsBegin = std::sregex_iterator(parameters.begin(), parameters.end(), PARAM_REGEX);
                auto paramsEnd = std::sregex_iterator();

                for (std::sregex_iterator i = paramsBegin; i != paramsEnd; ++i) {
                    std::smatch paramMatch = *i;
                    if (paramMatch[2].matched) {
                        parameterNames.push_back(paramMatch[2].str());
                    } else {
                        parameterNames.push_back(""); // Unnamed parameter
                    }
                }
            }

            return parameterNames;
        }
        static inline std::string GetFunctionName(const std::string &line) {
            std::smatch match;

            if (std::regex_search(line, match, FUNCTION_REGEX)) {
                return match[1].str();
            }

            return "";
        }

        std::filesystem::path m_jsonMap;

        std::stringstream m_generatedHeader;

        std::set<std::string> m_systemIncludes;
        std::set<std::string> m_localIncludes;
        
        std::optional<std::filesystem::path> m_oldHeader;

    private:
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

template <typename T>
class HeaderManager final : public HeaderManagerBase {
    public:
        HeaderManager(const std::filesystem::path &jsonMapPath, const std::optional<std::filesystem::path> &oldHeader = std::nullopt) = delete;
};

template <>
class HeaderManager<ClassMap> final : public HeaderManagerBase {
    public:
        inline HeaderManager(const std::filesystem::path &jsonMapPath, const std::optional<std::filesystem::path> &oldHeader = std::nullopt) :
            HeaderManagerBase(jsonMapPath, oldHeader),
            m_classMap(jsonMapPath)
        {
            if (m_oldHeader.has_value()) {
                m_foundTypes.emplace();
            }
        }

    private:
        inline const std::string GenerateFunction(const ClassMap::JsonFunction &function, const std::optional<std::vector<std::string>> parameterNames = std::nullopt) {
            std::stringstream ss;
            
            return {};
        }

        inline void AnalyzeOldHeader() final {
            std::fstream oldHeader(m_oldHeader.value(), std::ios::in);
            std::string curLine;
            while (!std::getline(oldHeader, curLine).eof()) {
                std::size_t pos;
                if ((pos = curLine.find("#include")) != std::string::npos) {
                    if ((pos = curLine.find_first_of("\"")) != std::string::npos) {
                        m_localIncludes.insert(curLine.substr(pos + 1, curLine.find_last_of("\"") - pos - 1));
                    }
                    else if ((pos = curLine.find_first_of("<")) != std::string::npos) {
                        m_systemIncludes.insert(curLine.substr(pos + 1, curLine.find_last_of(">") - pos - 1));
                    }
                }

                if (IsFunctionDecleration(curLine)) {
                    auto functionName = std::find_if(std::begin(m_classMap.GetFunctions()), std::end(m_classMap.GetFunctions()), [functionName = GetFunctionName(curLine)](const ClassMap::JsonFunction &function) -> bool {
                        return (function.name == functionName); // Deal with overloading
                    });

                    std::vector<std::string> argumentNames{};
                    for (const std::string &argumentName : GetParameterNames(curLine)) {
                        argumentNames.push_back(argumentName);
                    }

                    auto found = m_foundTypes->find(*functionName);
                    if (found == std::end(m_foundTypes.value())) {
                        m_foundTypes->emplace(*functionName, std::vector<std::vector<std::string>>{argumentNames});
                    }
                    else {
                        // Should only happen when overloading?
                        found->second.push_back(argumentNames);
                    }
                }
            }
        }

        inline void GenerateContent() final {
            m_generatedHeader << "abstract_class " << m_classMap.GetClassName() << " {" << std::endl;
            for (const ClassMap::JsonFunction jsonFunction : m_classMap.GetFunctions()) {
                auto it = m_foundTypes->find(jsonFunction);
                m_generatedHeader << GenerateFunction(jsonFunction, (it != std::end(m_foundTypes.value()) ? std::make_optional(it->second.at(0)) : std::nullopt));
            }
            m_generatedHeader << "\n}" << std::endl;
        }

        ClassMap m_classMap;

        std::optional<std::map<ClassMap::JsonFunction, std::vector<std::vector<std::string>>>> m_foundTypes;
};
