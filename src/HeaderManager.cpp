#include "HeaderManager.hpp"

HeaderManager::HeaderManager(const std::filesystem::path &jsonMapPath, const std::optional<std::filesystem::path> &oldHeader) :
    m_jsonClassMap(jsonMapPath),
    m_generatedHeader(),
    m_mappedArgumentNames(std::nullopt),
    m_oldHeader(oldHeader)
{
    
}

void HeaderManager::GenerateHeader(const std::filesystem::path &out) {
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
}
