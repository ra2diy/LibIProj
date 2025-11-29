#include "ByteStream.h"
#include "WeaverObj.h"
#include "LibIProj.h"

ByteInputStream& operator>>(ByteInputStream& stm, bool& v)
{
    if (!stm.Success())return stm;
    stm.Align(1);
    v = stm.Get<bool>();
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, BYTE& v)
{
	if (!stm.Success())return stm;
	stm.Align(1);
	v = stm.Get<BYTE>();
	return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, float& v)
{
    if (!stm.Success())return stm;
    stm.Align();
    v = stm.Get<float>();
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, uint32_t& v)
{
    if (!stm.Success())return stm;
    stm.Align();
    v = stm.Get<uint32_t>();
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, int32_t& v)
{
    if (!stm.Success())return stm;
    stm.Align();
    v = stm.Get<int32_t>();
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, uint64_t& v)
{
    if (!stm.Success())return stm;
    stm.Align(8);
    v = stm.Get<uint64_t>();
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, int64_t& v)
{
    if (!stm.Success())return stm;
    stm.Align(8);
    v = stm.Get<int64_t>();
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, std::string& v)
{
    if (!stm.Success())return stm;
    uint32_t x;
    stm >> x;
    stm.Seek(int(stm.GetSizeTypeSize() - sizeof(uint32_t)), SEEK_CUR); // align if needed
    v.resize(x);
    memcpy(const_cast<char*>(v.c_str()), stm.GetByte(x), x);
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, std::wstring& v)
{
    if (!stm.Success())return stm;
    uint32_t x;
    stm >> x;
    stm.Seek(int(stm.GetSizeTypeSize() - sizeof(uint32_t)), SEEK_CUR); // align if needed
    v.resize(x);
    memcpy(const_cast<wchar_t*>(v.c_str()), stm.GetByte(size_t(x) << 1), size_t(x) << 1);
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, IniToken& v)
{
    if (!stm.Success())return stm;
    stm >> v.Empty >> v.IsSection >> v.Desc >> v.Key >> v.Value;
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, FVec2& v)
{
    if (!stm.Success())return stm;
    stm >> v.x >> v.y;
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, PairClipString& v)
{
    if (!stm.Success())return stm;
    stm >> v.A >> v.B;
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, PairClipOnShow& v)
{
    if (!stm.Success())return stm;
    stm >> v.Show >> v.Str;
    return stm;
}
ByteInputStream& operator>>(ByteInputStream& stm, IBS_Project& v)
{
	if (!stm.Success())return stm;
	stm.NoAlign();
    stm.SetSizeTypeSize(8);

    int32_t Magic, RealVMajor, RealVMinor, RealVRelease, RealVersion, NBlock, BlockBeginOffset;

	stm >> Magic;
    if (Magic != IPROJ_HeaderSign)
    {
		stm.Fail();
		return stm;
    }
	stm >> BlockBeginOffset;
	stm >> RealVMajor;
	stm >> RealVMinor;
	stm >> RealVRelease;
	RealVersion = RealVMajor * 10000 + RealVMinor * 100 + RealVRelease;
	stm >> NBlock;
	stm.SetVersion(GetClipFormatVersion(RealVersion));

	stm.Seek(BlockBeginOffset, SEEK_SET);

	int32_t IntBuf, IntBuf2;

    for (int i = 0; i < NBlock; i++)
    {
		stm >> IntBuf;
		stm >> IntBuf2;
        if (IntBuf == 0)
        {
            stm.Seek(IntBuf2, SEEK_CUR);//Blank Of Many Bytes
        }
        else
        {
            //only support block "Project"
			std::string Str1;
			stm >> Str1;
			auto Pos = stm.Position();

            if (Str1 == "Project")
            {
                if (RealVersion > INIWeaver_Version)
                {
                    stm.Fail();
                }
                else if (RealVersion >= 204)
                {
                    stm >> v.CreateTime
                        >> v.CreateVersionMajor
                        >> v.CreateVersionMinor
                        >> v.CreateVersionRelease
                        >> v.FullView_Ratio
                        >> v.FullView_EqCenter
                        >> v.LastOutputDir
                        >> v.LastOutputIniName;
                    stm >> v.Data;
				}
                else if (RealVersion >= 203)
                {
                    stm >> v.CreateTime
                        >> v.CreateVersionMajor
                        >> v.CreateVersionMinor
                        >> v.CreateVersionRelease
                        >> v.FullView_Ratio
                        >> v.FullView_EqCenter
                        >> v.LastOutputDir;
                    stm >> v.Data;
                }
                else
                {
                    stm >> v.CreateTime
                        >> v.CreateVersionMajor
                        >> v.CreateVersionMinor
                        >> v.CreateVersionRelease
                        >> v.FullView_Ratio
                        >> v.FullView_EqCenter;
                    stm >> v.Data;
                }
				auto CreateVersion = v.CreateVersionMajor * 10000 + v.CreateVersionMinor * 100 + v.CreateVersionRelease;
                v.Data.SetVersion(GetClipFormatVersion(CreateVersion));
            }
            stm.Seek(int(Pos + IntBuf2), SEEK_SET);
        }
    }

	return stm;
}


ByteInputStream& operator>>(ByteInputStream& stm, ModuleClipData& v)
{
    if (!stm.Success())return stm;
    stm >> v.IsLinkGroup;
    if (!v.IsLinkGroup)
    {
        stm >> v.IsComment;
        /*
        IsLinkGroup=false
        IsComment=false
        Ignore
        Desc
        Lines
        Inherit
        Register
        DefaultLinkKey
        DisplayName
        EqSize
        EqDelta
        VarList
        */
        if (!v.IsComment)
        {
            stm >> v.Ignore >> v.Desc >> v.Lines >> v.Inherit >> v.Register >> v.DefaultLinkKey
                >> v.DisplayName >> v.EqSize >> v.EqDelta >> v.VarList;
        }
        /*
        IsLinkGroup=false
        IsComment=true
        Desc
        EqSize
        EqDelta
        Comment
        */
        else
        {
            stm >> v.Desc >> v.EqSize >> v.EqDelta >> v.Comment;
        }
    }
    /*
    IsLinkGroup=true
    Desc
    DefaultLinkKey
    EqSize
    EqDelta
    VarList
    LinkTo
    */
    else
    {
        stm >> v.Desc >> v.DefaultLinkKey >> v.EqSize >> v.EqDelta >> v.VarList >> v.LinkGroup_LinkTo;
    }

    /*
    IncludedBySection
    IncludingSections
    */
    if (stm.VersionAtLeast(10006))
    {
        stm >> v.IncludedBySection >> v.IncludingSections >> v.CollapsedInComposed >> v.Frozen >> v.Hidden;
    }
    else
    {
        v.IncludedBySection = { "","" };
        v.IncludingSections.clear();
        v.CollapsedInComposed = false;
        v.Frozen = false;
        v.Hidden = false;
    }
    return stm;
}

ByteInputStream& operator>>(ByteInputStream& stm, ClipBoardData& v)
{
    if (!stm.Success())return stm;
    stm >> v.ProjectRID >> v.Modules;
	return stm;
}

ByteInputStream& operator>>(ByteInputStream& stm, ByteInputStream& v)
{
    if (!stm.Success())return stm;
    std::vector<BYTE> vec;
    stm >> vec;
    v.Set(std::move(vec));
	return stm;
}