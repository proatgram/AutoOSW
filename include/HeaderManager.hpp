#pragma once

#include <any>
#include <filesystem>
#include <list>
#include <optional>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>
#include <iostream>

#include "ClassMap.hpp"
#include "EMsgMap.hpp"

class HeaderManagerBase {
    public:
        inline HeaderManagerBase(const std::filesystem::path &jsonDumpPath, const std::filesystem::path &newHeaderPath, std::optional<std::filesystem::path> oldJsonMapDump = std::nullopt, const std::optional<std::filesystem::path> &oldHeader = std::nullopt) :
            m_jsonDumpPath(jsonDumpPath),
            m_newHeaderPath(newHeaderPath),
            m_generatedHeader(),
            m_oldJsonMapDumpPath(oldJsonMapDump),
            m_oldHeaderPath(oldHeader)
        {
            
        }


        inline void GenerateHeader() {
            if (m_oldHeaderPath.has_value()) {
                std::cout << "Analysing old header: " << m_oldHeaderPath->string() << std::endl;
                AnalyzeOldHeader();
            }

            m_generatedHeader << GENERATED_NOTICE_MSG << "\n" << std::endl;
            m_generatedHeader << "#pragma once" << "\n" << std::endl;

            for (const std::string &include : m_systemIncludes) {
                m_generatedHeader << "#include <" << include << ">" << std::endl;
            }

            if (m_systemIncludes.size() != 0) {
                m_generatedHeader << std::endl;
            }

            for (const std::string &include : m_localIncludes) {
                m_generatedHeader << "#include \"" << include << "\"" << std::endl;
            }

            if (m_localIncludes.size() != 0) {
                m_generatedHeader << std::endl;
            }

            std::cout << "Generating header content..." << std::endl;
            GenerateContent();

            std::cout << "Writing new header to: " << m_newHeaderPath.string() << std::endl;
            
            std::filesystem::create_directories(m_newHeaderPath.parent_path());
            std::fstream file;
            file.exceptions(std::fstream::failbit | std::fstream::badbit);
            file.open(m_newHeaderPath, std::fstream::out);

            m_generatedHeader.flush();
            file << m_generatedHeader.rdbuf();
            file.flush();
        }

    protected:
        inline static const std::regex FUNCTION_REGEX = std::regex(R"rgx((?:^\s*)(virtual\s+)?((?:const|volatile)?\s*[\w:\<\>\[\]&\*]+)\s+([\w]+)\s*\(([^)]*)\)\s*(const)?\s*(?:->\s*((?:const|volatile)?\s*[\w:\<\>\[\]&\*]+))?\s*(=\s*0)?;?)rgx");
        inline static const std::regex PARAM_REGEX = std::regex(R"rgx((const\s+)?((?:[\w:\[\]]+)(?:<[^>]+>)?)(\s+const)?\s*(\*|\&)?\s*([\w\[\]]+)?\s*(?=,|$))rgx");

        virtual void AnalyzeOldHeader() = 0;

        virtual void GenerateContent() = 0;

        inline static bool IsFunctionDecleration(const std::string &line) {
            return std::regex_match(line, FUNCTION_REGEX);
        }

        static inline std::vector<std::string> GetParameters(const std::string &line) {
            std::vector<std::string> parameterNames;
            std::smatch match;

            auto paramsBegin = std::sregex_iterator(line.begin(), line.end(), PARAM_REGEX);
            auto paramsEnd = std::sregex_iterator();

            for (std::sregex_iterator i = paramsBegin; i != paramsEnd; ++i) {
                std::smatch paramMatch = *i;
                std::string param;
                param += (paramMatch[1].matched || paramMatch[3].matched ? "const " : "");
                param += paramMatch[2].str();
                param += (paramMatch[4].matched ? " " + paramMatch[4].str() : "");
                param += (paramMatch[5].matched ? (paramMatch[4].matched ? paramMatch[5].str() : " " + paramMatch[5].str()) : "");
                parameterNames.push_back(param);
            }

            return parameterNames;
        }

        static inline std::string GetFunctionName(const std::string &line) {
            std::smatch match;

            if (std::regex_search(line, match, FUNCTION_REGEX)) {
                return match[3].str();
            }

            return "";
        }

        std::filesystem::path m_jsonDumpPath;

        std::stringstream m_generatedHeader;

        std::set<std::string> m_systemIncludes;
        std::set<std::string> m_localIncludes;
        
        std::optional<std::filesystem::path> m_oldHeaderPath;
        std::optional<std::filesystem::path> m_oldJsonMapDumpPath;

        std::filesystem::path m_newHeaderPath;

    private:
        static constexpr std::string_view GENERATED_NOTICE_MSG =
R"(            /*****************************  Open SteamWorks  *********************************/
            /*                                                                               */
            /*  This file is part of the Open SteamWorks project. All individuals associated */
            /*  with this project do not claim ownership of the contents                     */
            /*                                                                               */
            /*  The code, comments, and all related files, projects, resources,              */
            /*  redistributables included with this project are Copyright Valve Corporation. */
            /*  Additionally, Valve, the Valve logo, Half-Life, the Half-Life logo, the      */
            /*  Lambda logo, Steam, the Steam logo, Team Fortress, the Team Fortress logo,   */
            /*  Opposing Force, Day of Defeat, the Day of Defeat logo, Counter-Strike, the   */
            /*  Counter-Strike logo, Source, the Source logo, and Counter-Strike Condition   */
            /*  Zero are trademarks and or registered trademarks of Valve Corporation.       */
            /*  All other trademarks are property of their respective owners.                */
            /*                                                                               */
            /*********************************  NOTICE!  *************************************/
            /*                                                                               */
            /*  This is auto-generated code! There may be problems with it or it may be      */
            /*  wrong. Use at your own risk!                                                 */
            /*                                                                               */
            /*********************************************************************************/)";
};

template <typename T>
class HeaderManager final : public HeaderManagerBase {
    public:
        HeaderManager(const std::filesystem::path &jsonDumpPath, const std::filesystem::path &newHeaderPath, std::optional<std::filesystem::path> oldJsonMapDump = std::nullopt, const std::optional<std::filesystem::path> &oldHeader = std::nullopt) = delete;
};

template <>
class HeaderManager<ClassMap> final : public HeaderManagerBase {
    public:
        inline HeaderManager(const std::filesystem::path &jsonDumpPath, const std::filesystem::path &newHeaderPath, std::optional<std::filesystem::path> oldJsonMapDump = std::nullopt, const std::optional<std::filesystem::path> &oldHeader = std::nullopt) :
            HeaderManagerBase(jsonDumpPath, newHeaderPath, oldJsonMapDump, oldHeader),
            m_classMap(jsonDumpPath)
        {
            m_foundTypes.emplace();
        }

    private:

        /* Structures and mappings */

        struct HeaderFunction {
            std::string name;
            std::size_t argc;
            std::string indefiniteReturn;

            // Un-reliable to use
            // (And not implemented yet, might not ever be)
            std::vector<std::string> inferredSerializedReturns;
            std::vector<std::string> arguments;

            bool virtualFunction{};
            bool pureVirtualFunction{};
            bool constFunction{};
        };

        // TODO: Implement diff tracker between old JSON dump and new JSON dump
        struct FunctionDifferences {
            std::list<std::pair<std::size_t, std::size_t>> differences;
        };

        /* Functions */

        static inline HeaderFunction GenerateHeaderFunction(const std::string &line) {
            std::smatch match;
            std::regex_match(line.cbegin(), line.cend(), match, FUNCTION_REGEX);

            HeaderFunction function;

            // Name
            function.name = match[3].str();
            
            // Indefinite Return
            if (match[2].matched && match[2].str().find("auto") != std::string::npos) {
                function.indefiniteReturn = match[6].str();
            }
            else if (match[2].matched) {
                function.indefiniteReturn = match[2].str();
            }
            else {
                function.indefiniteReturn = "osw_unknown_return";
            }

            function.virtualFunction = match[1].matched;
            function.pureVirtualFunction = match[7].matched;
            function.constFunction = match[5].matched;

            function.arguments = GetParameters(match[4].str());

            function.argc = function.arguments.size() + 1;

            // TODO: Calculate inferred serialized returns

            return function;
        } 

        inline const std::string GenerateStringFunction(const HeaderFunction &function, const std::optional<ClassMap::JsonFunction> &jsonFunction, const std::optional<ClassMap::JsonFunction> &oldJsonFunction = std::nullopt, bool argcMismatch = false) {
            std::stringstream ss;

            ss << "/*\n\t";

            if (!jsonFunction.has_value()) {
                ss << " * FUNCTION NOT FOUND WARNING:\n\t"
                   << " * The function is not found in the new dump.\n\t"
                   << " * Use this function at your own risk.\n\t";
            }
            else if (argcMismatch) {
                ss << " * UNSAFE FUNCTION WARNING:\n\t"
                   << " * The functions argc does not match the previous dump\n\t"
                   << " * Use this function at your own risk.\n\t"
                   << " * Old serialized arguments and returns:\n\t"
                   << " * \t" << [oldJsonFunction]() -> std::string {
                        std::string ret;

                        if (!oldJsonFunction.has_value()) {
                            return "[NO DUMPED INFO]\n\t";
                        }

                        std::vector<std::string> args = oldJsonFunction->serializedArgs;
                        std::vector<std::string> rets = oldJsonFunction->serializedReturns;

                        ret += "[";
                        for (int i = 0; i < args.size(); i++) {
                            ret += args.at(i);
                            if (i + 1 < args.size()) {
                                ret += ", ";
                            }
                        }
                        ret += "]\n\t *\t";

                        ret += "[";
                        for (int i = 0; i < rets.size(); i++) {
                            ret += rets.at(i);
                            if (i + 1 < rets.size()) {
                                ret += ", ";
                            }
                        }
                        ret += "]\n\t *\n\t";

                        return ret;
                   }();
            }
            if (jsonFunction.has_value()) {
                ss << " * Serialization information:\n\t"
                   << " * Serialized arguments and returns:\n\t"
                   << " * \t" << [args = jsonFunction->serializedArgs, rets = jsonFunction->serializedReturns]() -> std::string {
                        std::string ret;

                        ret += "[";
                        for (int i = 0; i < args.size(); i++) {
                            ret += args.at(i);
                            if (i + 1 < args.size()) {
                                ret += ", ";
                            }
                        }
                        ret += "]\n\t *\t";

                        ret += "[";
                        for (int i = 0; i < rets.size(); i++) {
                            ret += rets.at(i);
                            if (i + 1 < rets.size()) {
                                ret += ", ";
                            }
                        }
                        ret += "]\n\t";

                        return ret;
                   }()
                   << " * Function address:\n\t"
                   << " * \t" << jsonFunction->address << "\n\t"
                   << " * Function fencepost:\n\t * \t" << jsonFunction->fencepost << "\n\t";
            }

            ss << "*/\n\t";

            if (!jsonFunction.has_value()) {
                ss << "OSW_NOTFOUND_WARNING\n\t";
            }
            if (argcMismatch) {
                ss << "OSW_UNSAFE_WARNING\n\t";
            }

            ss << (function.virtualFunction ? "virtual " : "")
               << "auto "
               << function.name
               << "(" << [args = function.arguments]() -> std::string {
                   std::string output;

                   for (int i = 0; i < args.size(); i++) {
                       output += args.at(i);
                       if (i + 1 < args.size()) {
                           output += ", ";
                       }
                   }

                   return output;
               }()
               << ")" << (function.constFunction ? " const " : " ")
               << "-> " << function.indefiniteReturn
               << (function.pureVirtualFunction ? " = 0;" : ";");
            
            return ss.str();
        }

        inline void AnalyzeOldHeader() final {
            std::fstream oldHeader(m_oldHeaderPath.value(), std::ios::in);
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

                // TODO: Check for other things in header file
                std::optional<ClassMap> oldJsonMap;
                if (m_oldJsonMapDumpPath.has_value()) {
                    oldJsonMap.emplace(*m_oldJsonMapDumpPath);
                }

                if (IsFunctionDecleration(curLine)) {
                    HeaderFunction headerFunction = GenerateHeaderFunction(curLine);
                    std::any classMapFound = IsFunctionInClassMap(headerFunction);
                    std::any oldClassMapFound = IsFunctionInClassMap(headerFunction, oldJsonMap.value_or(ClassMap{}));

                    std::optional<ClassMap::JsonFunction> oldClassMapFunction = std::nullopt;
                    if (oldClassMapFound.type() == typeid(ClassMap::JsonFunction)) {
                        oldClassMapFunction = std::any_cast<ClassMap::JsonFunction>(oldClassMapFound);
                    }

                    if (classMapFound.has_value()) {
                        if (classMapFound.type() == typeid(ClassMap::JsonFunction)) {
                            // No overloaded function, proceed as is
                            ClassMap::JsonFunction newClassMapFunction = std::any_cast<ClassMap::JsonFunction>(classMapFound);
                            if (headerFunction.arguments.size() + 1 == newClassMapFunction.serializedArgs.size() + (newClassMapFunction.serializedReturns.size() == 0 ? 1 : newClassMapFunction.serializedReturns.size())) {
                                // Function size (argc) hasn't changed
                                // Would compare both argc, but dumped JSON file argc seemed to be inaccurate? Por que?? é estúpido
                                // TODO: BIG TODO: Get a copy of a newer dump and headers, and incorperate checks
                                // with dumps from steamworks_dumper to check serialization changes
                                m_foundTypes->emplace(newClassMapFunction, std::vector<std::pair<HeaderFunction, std::string>>{{headerFunction, GenerateStringFunction(headerFunction, newClassMapFunction, oldClassMapFunction)}});
                            }
                            else {
                                // Function size (argc) doesn't match and has changed
                                m_foundTypes->emplace(newClassMapFunction, std::vector<std::pair<HeaderFunction, std::string>>{{headerFunction, GenerateStringFunction(headerFunction, newClassMapFunction, oldClassMapFunction, true)}});
                            }

                        }
                        else if (classMapFound.type() == typeid(std::list<ClassMap::JsonFunction>)) {
                            // Means that the function is overloaded.
                            // Determine which overloaded definition we are looking at.
                            // (How so? Good question :3)

                            std::list<ClassMap::JsonFunction> jsonFunctionList = std::any_cast<std::list<ClassMap::JsonFunction>>(classMapFound);

                            std::optional<ClassMap::JsonFunction> matchedJsonFunction{std::nullopt};
                            for (const ClassMap::JsonFunction &jsonFunction : jsonFunctionList) {
                                // First try to determine overloads by serialized return types, etc

                                int jsonSerializedReturns = jsonFunction.serializedReturns.size();
                                int jsonSerializedArgs = jsonFunction.serializedArgs.size();
                                
                                // Determine header "serialized returns"
                                int headerSerializedReturns{1};
                                int headerSerializedArgs{};
                                for (const std::string &argument : headerFunction.arguments) {
                                    std::smatch match;
                                    if (!std::regex_search(argument, match, PARAM_REGEX)) {
                                        std::cerr << "Param regex did not match in overloaded function handler... :(" << std::endl;
                                    }

                                    if (match[4].matched && !(match[1].matched || match[3].matched)) {
                                        headerSerializedReturns++;
                                    }
                                    else {
                                        headerSerializedArgs++;
                                    }
                                }

                                if (jsonSerializedReturns != headerSerializedReturns || jsonSerializedArgs != headerSerializedArgs) {
                                    std::cerr << "Found overloaded function, attempt to match to a JSON function failed..." << std::endl;
                                    std::cerr << "JSON serialized argument count: " << jsonSerializedArgs << std::endl;
                                    std::cerr << "Header serialized argument count: " << headerSerializedArgs << std::endl;
                                    std::cerr << "JSON serialized returns count: " << jsonSerializedReturns << std::endl;
                                    std::cerr << "Header serialized returns count: " << headerSerializedReturns << std::endl;
                                    continue;
                                }

                                std::cout << "Matched overloaded function to header:" << "\n\t";
                                std::cout << curLine << "\n\t";
                                std::cout << "Serialized Arguments:" << [jsonFunction]() -> std::string {
                                    std::string ret;
                                    ret += "[";
                                    for (int i = 0; i < jsonFunction.serializedArgs.size(); i++) {
                                        ret += jsonFunction.serializedArgs.at(i);
                                        if (i + 1 < jsonFunction.serializedArgs.size()) {
                                            ret += ", ";
                                        }
                                    }

                                    ret += "]";

                                    return ret;
                                }() << "\n\t";

                                std::cout << "Serialized returns:" << [jsonFunction]() -> std::string {
                                    std::string ret;
                                    ret += "[";
                                    for (int i = 0; i < jsonFunction.serializedReturns.size(); i++) {
                                        ret += jsonFunction.serializedReturns.at(i);
                                        if (i + 1 < jsonFunction.serializedReturns.size()) {
                                            ret += ", ";
                                        }
                                    }
                                    
                                    ret += "]";

                                    return ret;
                                }() << std::endl;
                                matchedJsonFunction = jsonFunction;
                                break;
                            }

                            if (!matchedJsonFunction.has_value()) {
                                std::cerr << "Unabled to find a match for the function." << std::endl;
                            }

                            auto it = m_foundTypes->find(matchedJsonFunction);

                            std::string stringFunc = GenerateStringFunction(headerFunction, matchedJsonFunction, oldClassMapFunction, (matchedJsonFunction.has_value() ? false : true));

                            if (it == m_foundTypes->end()) {
                                m_foundTypes->emplace(matchedJsonFunction, std::vector<std::pair<HeaderFunction, std::string>>{{headerFunction, stringFunc}});
                            }
                            else {
                                bool found{false};
                                for (const auto &[headerFunc, headerString] : it->second) {
                                    // Determine if overload is already in the overload vector
                                    if (headerString == stringFunc) {
                                        std::cerr << "Overloaded function is already in the overload vector..." << std::endl;
                                        found = true;
                                    }
                                }
                                if (!found) {
                                    it->second.push_back({headerFunction, stringFunc});
                                }
                            }
                        }
                        else {
                            // Unknown function in the class map.
                            // Give warning. (Boo)
                            std::cerr << "Found unknown function " << headerFunction.name << " in file " << m_oldHeaderPath->string() << std::endl;
                            m_foundTypes->emplace(std::nullopt, std::vector<std::pair<HeaderFunction, std::string>>{{headerFunction, GenerateStringFunction(headerFunction, std::nullopt, oldClassMapFunction)}});
                        }
                    }
                }
            }
        }

        inline void GenerateContent() final {
            m_generatedHeader << "osw_abstract_class OSW_UNSAFE_INTERFACE" << m_classMap.GetClassName() << " {" << std::endl;
            for (const auto &[jsonFunction, functionPairVector] : *m_foundTypes) {
                for (const auto &[oldHeaderFunction, stringFunction] : functionPairVector) {
                    m_generatedHeader << "\t" << stringFunction << "\n\n";
                }
            }

            m_generatedHeader << "\n};" << std::endl;
        }

        inline std::any IsFunctionInClassMap(const HeaderFunction &headerFunction) const {
            std::list<ClassMap::JsonFunction> occ;
            for (const ClassMap::JsonFunction &jsonFunction : m_classMap.GetFunctions()) {
                if (jsonFunction.name == headerFunction.name) {
                    occ.push_back(jsonFunction);
                }
            }

            if (occ.size() == 1) {
                return *occ.begin();
            }
            else if (occ.size() > 1) {
                return occ;
            }
            else {
                return {};
            }
        }

        inline std::any IsFunctionInClassMap(const HeaderFunction &headerFunction, const ClassMap &classMap) const {
            std::list<ClassMap::JsonFunction> occ;
            for (const ClassMap::JsonFunction &jsonFunction : classMap.GetFunctions()) {
                if (jsonFunction.name == headerFunction.name) {
                    occ.push_back(jsonFunction);
                }
            }

            if (occ.size() == 1) {
                return *occ.begin();
            }
            else if (occ.size() > 1) {
                return occ;
            }
            else {
                return {};
            }
        }

        ClassMap m_classMap;

        // Custom comparator for std::map
        struct OptionalComparator {
            bool operator()(const std::optional<ClassMap::JsonFunction>& lhs,
                            const std::optional<ClassMap::JsonFunction>& rhs) const {
                // Handle cases where either optional is empty
                if (!lhs && !rhs) return false;
                if (!lhs) return true;
                if (!rhs) return false;
                
                // If both are populated, compare the contained values
                return lhs->name < rhs->name;
            }
        };


        std::optional<std::map<std::optional<ClassMap::JsonFunction>, std::vector<std::pair<HeaderFunction, std::string>>, OptionalComparator>> m_foundTypes;
};

template<>
class HeaderManager<EMsgMap> final : public HeaderManagerBase {
    public:
        HeaderManager(const std::filesystem::path &jsonDumpPath, const std::filesystem::path &newHeaderPath, std::optional<std::filesystem::path> ) :
            HeaderManagerBase(jsonDumpPath, newHeaderPath, std::nullopt, std::nullopt),
                m_eMsgMap(jsonDumpPath) {}

    private:
        // Not used for EMsg (Porque nós não temos um antigo)
        // At least, not yet
        void AnalyzeOldHeader() override {}

        void GenerateContent() override {
            for (const auto &[name, eMsg, flags, serverType] : m_eMsgMap.GetEMsgs()) {
                m_eMsgs.insert_or_assign(eMsg, name);
            }

            m_generatedHeader << "enum class EMessages {\n";
            for (const auto &[eMsg, name] : m_eMsgs) {
                m_generatedHeader << "    " << name << " = " << eMsg << ",\n";
            }
            m_generatedHeader << "};";
        }

        std::map<int, std::string> m_eMsgs;

        EMsgMap m_eMsgMap;
};
