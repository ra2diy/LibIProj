#pragma once
#include "WeaverObj.h"
#include <unordered_map>
#include <ctime>
#include <coroutine>
#include <generator>

const int constexpr IPROJ_HeaderSign = 0x00114514;

const char* const IPROJ_Extension = ".iproj";

class INIWeaverModule
{
	const ModuleClipData* ModData;

	std::unordered_map<std::string, std::string> SearchTable_Lines;
	std::unordered_map<std::string, std::string> SearchTable_Vars;
	std::unordered_map<std::string, std::string> SearchTable_DefaultLinkKeys;
public:
	INIWeaverModule() = default;
	INIWeaverModule(const ModuleClipData* modData) : ModData(modData) {}
	//don't construct by yourself, use INIWeaverProject to get instances

	bool Available() const;
	operator bool() const;
	bool BuildSearchTable();

	//All accessors below assume Available() is true

	bool IsLinkGroup() const;
	bool IsComment() const;
	bool Ignore() const;
	bool CollapsedInComposed() const;
	bool Frozen() const;
	bool Hidden() const;

	FVec2 GetEqSize() const;
	FVec2 GetEqDelta() const;
	const std::string& GetININame() const;
	const std::string& GetSectionName() const;


	//only if IsComment() is true
	const std::string& GetComment() const;

	//only if IsLinkGroup() is true
	const std::vector<PairClipString>& GetLinkGroup_LinkTo() const;

	const std::string& GetInherit() const;
	const std::string& GetRegister() const;
	const std::string& GetDisplayName() const;

	const std::vector<IniToken>& GetLines() const;
	const std::vector<PairClipString>& GetDefaultLinkKey() const;
	const std::vector<PairClipString>& GetVarList() const;

	const std::string* GetLineValueByKey(const std::string& key) const;
	const std::string* GetVarValueByName(const std::string& name) const;
	const std::string* GetDefaultLinkKeyByName(const std::string& name) const;

	bool KeyExistsInLines(const std::string& key) const;
	bool VarExistsByName(const std::string& name) const;
	bool DefaultLinkKeyExistsByName(const std::string& name) const;

	const PairClipString& GetIncludedBySection() const;
	const std::vector<PairClipString>& GetIncludingSections() const;
};

class INIWeaverProject
{
	IBS_Project SProj;
	ClipBoardData Clip;

	std::unordered_map<std::string, std::wstring> LastOutputIniName_Table;
	std::unordered_map<std::string, std::unordered_map<std::string, INIWeaverModule*>> ModuleClipData_ByDesc;
	std::unordered_map<std::string, INIWeaverModule*> ModuleClipData_ByDisplayName;
	std::vector<INIWeaverModule> ModuleClipData_List;

	bool BuildSearchTable();
	bool LoadClip();
public :

	bool Load(LPCVOID* data, size_t size);

	bool Load(const std::vector<BYTE>& data);

	bool Load(std::vector<BYTE>&& data);

	int GetCreateVersion() const;
	int GetCreateVersionMajor() const;
	int GetCreateVersionMinor() const;
	int GetCreateVersionRelease() const;
	std::string GetCreateVersionStr() const;

	const std::wstring& GetLastOutputDir() const;
	std::vector<std::pair<std::string, std::wstring>> GetLastOutputIniName() const;
	const std::wstring* GetLastOutputIniName(const std::string& key) const;

	uint64_t GetCreateTime() const;
	time_t GetCreateTimeAsTimeT() const;

	float GetViewRatio() const;
	FVec2 GetViewCenter() const;

	uint32_t GetProjectID() const;

	const std::vector<INIWeaverModule>& GetModules() const;
	const INIWeaverModule* GetModuleByIndex(size_t index) const;
	const INIWeaverModule* GetModuleByDesc(const std::string& INI, const std::string& Section) const;
	const INIWeaverModule* GetModuleByDisplayName(const std::string& displayName) const;

};

class INIWeaverProjectStreamer
{
	IBS_Project SProj;
	uint32_t ProjectRID;
	ModuleClipData Mod;
public:
	bool Load(LPCVOID* data, size_t size);
	bool Load(const std::vector<BYTE>& data);
	bool Load(std::vector<BYTE>&& data);

	std::generator<ModuleClipData&> StreamModules();
	
	/*
	Note : Different from Export INI in INIWeaver.
	Since there is no RegisterTypes Config File.
	*/
	std::generator<std::string> StreamLines();
	std::generator<std::string> StreamLines_CollectRegister();
};