#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>

struct Theme {
    std::string name;
    uint32_t windowBg;
    uint32_t cardBg;
    uint32_t editBg;
    uint32_t logBg;
    uint32_t textTitle;
    uint32_t textSub;
    uint32_t textNormal;
    uint32_t accent;
    uint32_t accentHover;
    uint32_t accentPressed;
};

extern const std::vector<Theme> g_themes;
extern int g_currentThemeIdx;

struct MovedFileInfo {
    std::string fileName;
    std::string category;
    uint64_t fileSize;
    std::string status;
};

std::map<std::string, std::vector<std::string>> ParseConfig(const std::string& jsonStr);
std::string ReadFileToString(const std::string& path);
std::string FormatSize(uint64_t bytes);
std::string EscapePDFText(const std::string& text);
std::string TruncateText(const std::string& text, size_t maxLen);
void GeneratePDFReport(const std::string& outputPath, const std::string& targetFolder,
                       const std::vector<MovedFileInfo>& movedFiles, bool dryRun);
