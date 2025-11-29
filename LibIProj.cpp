#include "LibIProj.h"
#include "ByteStream.h"
#include <ranges>

bool INIWeaverProject::LoadClip()
{
	ByteInputStream& stm = SProj.Data;
	stm >> Clip;
	return stm.Success();
}

bool INIWeaverProject::Load(LPCVOID* data, size_t size)
{
	ByteInputStream stm;
	stm.Set(data, size);
	stm >> SProj;
	return stm.Success() && LoadClip() && BuildSearchTable();
}

bool INIWeaverProject::Load(const std::vector<BYTE>& data)
{
	ByteInputStream stm;
	stm.Set(data);
	stm >> SProj;
	return stm.Success() && LoadClip() && BuildSearchTable();
}

bool INIWeaverProject::Load(std::vector<BYTE>&& data)
{
	ByteInputStream stm;
	stm.Set(std::move(data));
	stm >> SProj;
	return stm.Success() && LoadClip() && BuildSearchTable();
}

bool INIWeaverProject::BuildSearchTable()
{
	bool Ret = true;

	LastOutputIniName_Table.clear();
	for (const auto& pair : SProj.LastOutputIniName)
	{
		LastOutputIniName_Table.insert(pair);
	}

	ModuleClipData_ByDesc.clear();
	ModuleClipData_ByDisplayName.clear();
	ModuleClipData_List.reserve(Clip.Modules.size());
	for (auto& mod : Clip.Modules)
	{
		if (!mod.IsLinkGroup)
		{
			ModuleClipData_List.emplace_back(&mod);
			auto& Module = ModuleClipData_List.back();
			ModuleClipData_ByDisplayName[mod.DisplayName] = &Module;
			ModuleClipData_ByDesc[mod.Desc.A][mod.Desc.B] = &Module;
			Ret &= Module.BuildSearchTable();
		}
	}
	return Ret;
}

int INIWeaverProject::GetCreateVersion() const
{
	return SProj.CreateVersionMajor * 10000 + SProj.CreateVersionMinor * 100 + SProj.CreateVersionRelease;
}
int INIWeaverProject::GetCreateVersionMajor() const
{
	return SProj.CreateVersionMajor;
}
int INIWeaverProject::GetCreateVersionMinor() const
{
	return SProj.CreateVersionMinor;
}
int INIWeaverProject::GetCreateVersionRelease() const
{
	return SProj.CreateVersionRelease;
}
std::string INIWeaverProject::GetCreateVersionStr() const
{
	return GetVersionStr(GetCreateVersion());
}

const std::wstring& INIWeaverProject::GetLastOutputDir() const
{
	return SProj.LastOutputDir;
}
std::vector<std::pair<std::string, std::wstring>> INIWeaverProject::GetLastOutputIniName() const
{
	return SProj.LastOutputIniName;
}
const std::wstring* INIWeaverProject::GetLastOutputIniName(const std::string& key) const
{
	auto it = LastOutputIniName_Table.find(key);
	if (it != LastOutputIniName_Table.end())
	{
		return &it->second;
	}
	return nullptr;
}

uint64_t INIWeaverProject::GetCreateTime() const
{
	return SProj.CreateTime;
}
time_t INIWeaverProject::GetCreateTimeAsTimeT() const
{
	return static_cast<time_t>(SProj.CreateTime / 1000000);
}

float INIWeaverProject::GetViewRatio() const
{
	return SProj.FullView_Ratio;
}
FVec2 INIWeaverProject::GetViewCenter() const
{
	return SProj.FullView_EqCenter;
}

uint32_t INIWeaverProject::GetProjectID() const
{
	return Clip.ProjectRID;
}

const std::vector<INIWeaverModule>& INIWeaverProject::GetModules() const
{
	return ModuleClipData_List;
}

const INIWeaverModule* INIWeaverProject::GetModuleByIndex(size_t index) const
{
	if (index < Clip.Modules.size())
	{
		return &ModuleClipData_List[index];
	}
	return nullptr;
}

const INIWeaverModule* INIWeaverProject::GetModuleByDesc(const std::string& INI, const std::string& Section)  const
{
	auto it1 = ModuleClipData_ByDesc.find(INI);
	if (it1 != ModuleClipData_ByDesc.end())
	{
		auto it2 = it1->second.find(Section);
		if (it2 != it1->second.end())
		{
			return it2->second;
		}
	}
	return nullptr;
}

const INIWeaverModule* INIWeaverProject::GetModuleByDisplayName(const std::string& displayName) const
{
	auto it = ModuleClipData_ByDisplayName.find(displayName);
	if (it != ModuleClipData_ByDisplayName.end())
	{
		return it->second;
	}
	return nullptr;
}











bool INIWeaverModule::Available() const
{
	return ModData != nullptr;
}
INIWeaverModule::operator bool() const
{
	return Available();
}

bool INIWeaverModule::BuildSearchTable()
{
	SearchTable_Lines.clear();
	for (const auto& line : ModData->Lines)
	{
		if (!line.Empty)
		{
			SearchTable_Lines[line.Key] = line.Value;
		}
	}

	SearchTable_Vars.clear();
	for (const auto& var : ModData->VarList)
	{
		SearchTable_Vars[var.A] = var.B;
	}

	SearchTable_DefaultLinkKeys.clear();
	for (const auto& linkKey : ModData->DefaultLinkKey)
	{
		SearchTable_DefaultLinkKeys[linkKey.A] = linkKey.B;
	}

	return true;
}

//All accessors below assume Available() is true

bool INIWeaverModule::IsLinkGroup() const
{
	return ModData->IsLinkGroup;
}
bool INIWeaverModule::IsComment() const
{
	return ModData->IsComment;
}
bool INIWeaverModule::Ignore() const
{
	return ModData->Ignore;
}
bool INIWeaverModule::CollapsedInComposed() const
{
	return ModData->CollapsedInComposed;
}
bool INIWeaverModule::Frozen() const
{
	return ModData->Frozen;
}
bool INIWeaverModule::Hidden() const
{
	return ModData->Hidden;
}

FVec2 INIWeaverModule::GetEqSize() const
{
	return ModData->EqSize;
}
FVec2 INIWeaverModule::GetEqDelta() const
{
	return ModData->EqDelta;
}
const std::string& INIWeaverModule::GetININame() const
{
	return ModData->Desc.A;
}
const std::string& INIWeaverModule::GetSectionName() const
{
	return ModData->Desc.B;
}


//only if IsComment() is true
const std::string& INIWeaverModule::GetComment() const
{
	return ModData->Comment;
}

//only if IsLinkGroup() is true
const std::vector<PairClipString>& INIWeaverModule::GetLinkGroup_LinkTo() const
{
	return ModData->LinkGroup_LinkTo;
}

const std::string& INIWeaverModule::GetInherit() const
{
	return ModData->Inherit;
}
const std::string& INIWeaverModule::GetRegister() const
{
	return ModData->Register;
}
const std::string& INIWeaverModule::GetDisplayName() const
{
	return ModData->DisplayName;
}

const std::vector<IniToken>& INIWeaverModule::GetLines() const
{
	return ModData->Lines;
}
const std::vector<PairClipString>& INIWeaverModule::GetDefaultLinkKey() const
{
	return ModData->DefaultLinkKey;
}
const std::vector<PairClipString>& INIWeaverModule::GetVarList() const
{
	return ModData->VarList;
}

const std::string* INIWeaverModule::GetLineValueByKey(const std::string& key) const
{
	auto it = SearchTable_Lines.find(key);
	if (it != SearchTable_Lines.end())
	{
		return &it->second;
	}
	return nullptr;
}
const std::string* INIWeaverModule::GetVarValueByName(const std::string& name) const
{
	auto it = SearchTable_Vars.find(name);
	if (it != SearchTable_Vars.end())
	{
		return &it->second;
	}
	return nullptr;
}
const std::string* INIWeaverModule::GetDefaultLinkKeyByName(const std::string& name) const
{
	auto it = SearchTable_Lines.find(name);
	if (it != SearchTable_Lines.end())
	{
		return &it->second;
	}
	return nullptr;
}

bool INIWeaverModule::KeyExistsInLines(const std::string& key) const
{
	return SearchTable_Lines.find(key) != SearchTable_Lines.end();
}
bool INIWeaverModule::VarExistsByName(const std::string& name) const
{
	return SearchTable_Vars.find(name) != SearchTable_Vars.end();
}
bool INIWeaverModule::DefaultLinkKeyExistsByName(const std::string& name) const
{
	return SearchTable_DefaultLinkKeys.find(name) != SearchTable_DefaultLinkKeys.end();
}

const PairClipString& INIWeaverModule::GetIncludedBySection() const
{
	return ModData->IncludedBySection;
}
const std::vector<PairClipString>& INIWeaverModule::GetIncludingSections() const
{
	return ModData->IncludingSections;
}



bool INIWeaverProjectStreamer::Load(LPCVOID* data, size_t size)
{
	ByteInputStream stm;
	stm.Set(data, size);
	stm >> SProj;
	return stm.Success();
}

bool INIWeaverProjectStreamer::Load(const std::vector<BYTE>& data)
{
	ByteInputStream stm;
	stm.Set(data);
	stm >> SProj;
	return stm.Success();
}

bool INIWeaverProjectStreamer::Load(std::vector<BYTE>&& data)
{
	ByteInputStream stm;
	stm.Set(std::move(data));
	stm >> SProj;
	return stm.Success();
}

std::generator<ModuleClipData&> INIWeaverProjectStreamer::StreamModules()
{
	auto& stm = SProj.Data;
	stm.Rewind();
	stm >> ProjectRID;

	uint32_t x;
	stm >> x;
	stm.Seek(int(stm.GetSizeTypeSize() - sizeof(uint32_t)), SEEK_CUR); // align if needed
	for (uint32_t i = 0; i < x; i++)
	{
		stm >> Mod;
		if (!stm.Success()) co_return;
		co_yield Mod;
	}
}

std::unordered_map<std::string, std::string> IniWeaverTypetoWICType = {
	{ "Rules", "rule" },
	{ "Art", "art" },
	{ "AI", "ai" },
	{ "Sound", "sound" },
	{ "Eva", "eva" },
	{ "UI", "ui" },
};

std::generator<std::string> INIWeaverProjectStreamer::StreamLines()
{
	for (const auto& mod : StreamModules())
	{
		// output if !IsLinkGroup && !IsComment && !Ignore
		if (mod.IsComment || mod.IsLinkGroup || mod.Ignore) continue;
		// process Desc Inherit Register Lines
		/*
		Format :
		[Desc.Sec]:[Inherit.0]
		...
		[Desc.Sec]:[Inherit.N]
		@Type= rule/art/sound etc.
		Lines[0].Key = Lines[0].Value
		...
		Lines[N].Key = Lines[N].Value

		[Register]
		Desc.Sec=Desc.Sec
		*/
		//Inherit is comma separated
		if (mod.Inherit.empty())
		{
			co_yield "[" + mod.Desc.B + "]";
		}
		else
		{
			auto InheritNames = mod.Inherit | std::views::split(',') | std::views::transform([](auto&& rng)
				{
					return std::string(&*rng.begin(), std::ranges::distance(rng));
				});
			for (const auto& inheritName : InheritNames)
			{
				co_yield "[" + mod.Desc.B + "]:" + inheritName;
			}
		}
		if (IniWeaverTypetoWICType.contains(mod.Desc.A))
		{
			co_yield "@Type=" + IniWeaverTypetoWICType[mod.Desc.A];
		}
		for (const auto& line : mod.Lines)
		{
			co_yield line.Key + " = " + line.Value;
		}
		co_yield "";
		co_yield "[" + mod.Register + "]";
		co_yield mod.Desc.B + "=" + mod.Desc.B;
		co_yield "";
	}
}