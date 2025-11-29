#pragma once
#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include <windows.h>

struct FVec2
{
    float                                   x, y;
    constexpr FVec2() : x(0.0f), y(0.0f) {}
    constexpr FVec2(float _x, float _y) : x(_x), y(_y) {}
};

//最高支持到INIWeaver 1.0.6版本
const int constexpr INIWeaver_VersionMajor = 1;
const int constexpr INIWeaver_VersionMinor = 0;
const int constexpr INIWeaver_VersionRelease = 6;
const int constexpr INIWeaver_Version = 10000 * INIWeaver_VersionMajor + 100 * INIWeaver_VersionMinor + INIWeaver_VersionRelease;

inline int constexpr GetClipFormatVersion(int AppVersion)
{
    if (AppVersion > INIWeaver_Version)return AppVersion;
    if (AppVersion >= 10006)return 10006;
    if (AppVersion >= 200)return 10000;
    return 0;
}

inline std::string GetVersionStr(int Ver)
{
    if (Ver <= 0)
    {
        return "Unknown Version";
    }
    else if (Ver < 200)
    {
        if (Ver % 100)return std::string("Old V0.1") + " b" + std::to_string(Ver % 100);
        else return std::string("Old V0.1");
    }
    else if (Ver < 10000)
    {
        if (Ver % 100)return std::to_string(Ver / 10000) + "." + std::to_string((Ver / 100) % 100) + "b" + std::to_string(Ver % 100);
        else return std::to_string(Ver / 10000) + "." + std::to_string((Ver / 100) % 100);
    }
    else
    {
        if (Ver % 100)return std::to_string(Ver / 10000) + "." + std::to_string((Ver / 100) % 100) + "." + std::to_string(Ver % 100);
        else return std::to_string(Ver / 10000) + "." + std::to_string((Ver / 100) % 100);
    }
}

struct IBS_Project
{
    std::wstring LastOutputDir;
    std::vector<std::pair<std::string, std::wstring>> LastOutputIniName;

    uint64_t CreateTime;
    int CreateVersionMajor, CreateVersionMinor, CreateVersionRelease;
    float FullView_Ratio;
    FVec2 FullView_EqCenter;
    std::vector<BYTE> Data;
};

struct PairClipString
{
    std::string A;
    std::string B;

};

struct PairClipOnShow
{
    std::string Str;
    bool Show;
};

struct IniToken
{
    bool IsSection{ false }, Empty{ false }, HasDesc{ false };
    std::string Key, Value, Desc;
};

struct ModuleClipData
{
    bool IsLinkGroup;
    bool IsComment;
    bool Ignore;
    bool FromClipBoard;
    bool CollapsedInComposed;
    bool Frozen;
    bool Hidden;
    FVec2 EqSize;
    FVec2 EqDelta;
    PairClipString Desc;
    std::string Comment;
    std::string Inherit;
    std::string Register;
    std::vector<PairClipString> DefaultLinkKey;
    std::string DisplayName;
    std::vector<IniToken> Lines;
    std::vector<PairClipString> LinkGroup_LinkTo;
    std::vector<PairClipString> VarList;
    PairClipString IncludedBySection;
    std::vector<PairClipString> IncludingSections;
};

struct ClipBoardData
{
    uint32_t ProjectRID;
    std::vector<ModuleClipData> Modules;
};