// LibIProj.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../LibIProj.h"
#include <fstream>
#include <cstdio>
#include <cstdlib>

std::vector<BYTE> LoadFileToByteVector(const std::wstring& filepath)
{
	std::ifstream file(filepath, std::ios::binary);
	if (!file)return {};
	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<BYTE> buffer(fileSize);
	file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
	return buffer;
}

void OutputTime(time_t time)
{
	struct tm timeinfo;
	char buffer[80];

	localtime_s(&timeinfo ,&time); // 转换为本地时间
	strftime(buffer, 80, " %Y-%m-%d %H:%M:%S", &timeinfo); // 格式化输出

	printf("%s\n", buffer);
}

int main()
{
	auto path = L"example.iproj";
	system("chcp 65001 > nul"); // 设置控制台为UTF-8编码

	INIWeaverProject p;
	p.Load(LoadFileToByteVector(path));

	// output everything to console
	printf("Project File: %ls\n", path);
	printf("Last Output Dir: %ls\n", p.GetLastOutputDir().c_str());
	printf("Last Output Ini Names:\n");
	for (const auto& pair : p.GetLastOutputIniName())
	{
		printf("  %s : %ls\n", pair.first.c_str(), pair.second.c_str());
	}
	printf("Create Time: ");
	OutputTime(p.GetCreateTimeAsTimeT());
	printf("Create Version: %s\n", p.GetCreateVersionStr().c_str());
	printf("Full View Ratio: %f\n", p.GetViewRatio());
	printf("Full View Eq Center: (%f, %f)\n", p.GetViewCenter().x, p.GetViewCenter().y);

	printf("Project RID: %u\n", p.GetProjectID());
	const auto& modules = p.GetModules();
	printf("Modules Count: %zu\n", modules.size());
	for (auto& mod : modules)
	{
		printf("  Is Link Group: %s\n", mod.IsLinkGroup() ? "true" : "false");
		printf("  Eq Size: (%f, %f)\n", mod.GetEqSize().x, mod.GetEqSize().y);
		printf("  Eq Delta: (%f, %f)\n", mod.GetEqSize().x, mod.GetEqSize().y);
		printf("  Ignore: %s\n", mod.Ignore() ? "true" : "false");
		printf("  Hidden: %s\n", mod.Hidden() ? "true" : "false");
		printf("  Frozen: %s\n", mod.Frozen() ? "true" : "false");
		printf("  Collapsed In Composed: %s\n", mod.CollapsedInComposed() ? "true" : "false");
		if (!mod.IsLinkGroup())
		{
			printf("  Is Comment: %s\n", mod.IsComment() ? "true" : "false");
			if (!mod.IsComment())
			{
				printf("    Desc: %s:[%s]\n", mod.GetININame().c_str(), mod.GetSectionName().c_str());
				printf("    Display Name: %s\n", mod.GetDisplayName().c_str());
				printf("    Inherit: %s\n", mod.GetInherit().c_str());
				printf("    RegisterType: %s\n", mod.GetRegister().c_str());
				printf("    Lines: \n");
				for (const auto& line : mod.GetLines())
				{
					printf("      %s = %s\n", line.Key.c_str(), line.Value.c_str());
				}
				printf("    Default Link Keys Size: %zu\n", mod.GetDefaultLinkKey().size());
				printf("    VarList Size: %zu\n", mod.GetVarList().size());
			}
			else
			{
				printf("    Comment: %s\n", mod.GetComment().c_str());
			}
		}
		else
		{
			printf("  Link To Size: %zu\n", mod.GetLinkGroup_LinkTo().size());
		}
	}

	return 0;
}