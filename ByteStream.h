#pragma once
#include <string>
#include <vector>
#include <windows.h>

class ByteInputStream
{
private:
    std::vector<BYTE> Buffer;
    LPBYTE Begin;
    LPBYTE Cursor;
    LPBYTE End;
    int ClipFormatVersion;
    bool _Success;
    bool UseAlign;
    size_t SizeTypeSize;
public:

    void Init(LPBYTE begin, size_t Size)
    {
        Cursor = Begin = begin;
		End = Begin + Size;
        _Success = true;
		UseAlign = true;
		SizeTypeSize = 4;
    }

    void Rewind()
    {
        Cursor = Begin;
    }

    LPBYTE GetByte(size_t MoveAfterRead)
    {
		if (Cursor + MoveAfterRead > End)
		{
			MoveAfterRead = size_t(End - Cursor);
		}
        LPBYTE Cur2 = Cursor;
        Cursor += MoveAfterRead;
        return Cur2;
    }

    void Align(size_t N = 4)
    {
		if (!UseAlign) return;
        while ((Cursor - Begin) % N)++Cursor;
    }

	size_t Position() const
	{
		return Cursor - Begin;
	}

	bool Eof() const
	{
		return Cursor >= End;
	}

    template<typename T>
    T& Get(size_t MoveAfterRead = sizeof(T))
    {
        return *((T*)GetByte(MoveAfterRead));
    }

    void Seek(int pos, int method = SEEK_SET)
    {
		if (method == SEEK_SET)
			Cursor = Begin + pos;
		else if (method == SEEK_CUR)
			Cursor += pos;
		else if (method == SEEK_END)
			Cursor = Begin + Buffer.size() + pos;
    }

    //Here removed function : read from Base64

    void Set(const std::vector<BYTE>& Vec)
    {
        Buffer = Vec;
        Init(Buffer.data(), Buffer.size());
    }

	void Set(std::vector<BYTE>&& Vec)
	{
		Buffer = std::move(Vec);
		Init(Buffer.data(), Buffer.size());
	}

	void Set(LPCVOID data, size_t size, bool Clone)
	{
        if (Clone)
        {
            Buffer.resize(size);
            memcpy(Buffer.data(), data, size);
			Init(Buffer.data(), Buffer.size());
        }
        else
        {
            Init((LPBYTE)data, size);
        }
	}

	void NoAlign() { UseAlign = false; }
	void Fail() { _Success = false; }
	bool Success() const { return _Success; }
	void SetSizeTypeSize(size_t sz) { SizeTypeSize = sz; }
	size_t GetSizeTypeSize() const { return SizeTypeSize; }

    void SetVersion(int v) { ClipFormatVersion = v; }
    bool VersionAtLeast(int v) const { return ClipFormatVersion >= v; }
};

struct IBS_Project;
struct FVec2;
struct IniToken;
struct PairClipString;
struct PairClipOnShow;
struct ClipBoardData;
struct ModuleClipData;


ByteInputStream& operator>>(ByteInputStream& stm, bool& v);
ByteInputStream& operator>>(ByteInputStream& stm, BYTE& v);
ByteInputStream& operator>>(ByteInputStream& stm, float& v);
ByteInputStream& operator>>(ByteInputStream& stm, uint32_t& v);
ByteInputStream& operator>>(ByteInputStream& stm, int32_t& v);
ByteInputStream& operator>>(ByteInputStream& stm, uint64_t& v);
ByteInputStream& operator>>(ByteInputStream& stm, int64_t& v);
ByteInputStream& operator>>(ByteInputStream& stm, std::string& v);
ByteInputStream& operator>>(ByteInputStream& stm, std::wstring& v);
ByteInputStream& operator>>(ByteInputStream& stm, IBS_Project& v);
ByteInputStream& operator>>(ByteInputStream& stm, IniToken& v);
ByteInputStream& operator>>(ByteInputStream& stm, FVec2& v);
ByteInputStream& operator>>(ByteInputStream& stm, PairClipString& v);
ByteInputStream& operator>>(ByteInputStream& stm, PairClipOnShow& v);
ByteInputStream& operator>>(ByteInputStream& stm, ModuleClipData& v);
ByteInputStream& operator>>(ByteInputStream& stm, ClipBoardData& v);
ByteInputStream& operator>>(ByteInputStream& stm, ByteInputStream& v);

template<typename T>
ByteInputStream& operator>>(ByteInputStream& stm, std::vector<T>& v)
{
    uint32_t x;
    stm >> x;
	stm.Seek(int(stm.GetSizeTypeSize() - sizeof(uint32_t)), SEEK_CUR); // align if needed
    v.resize(x);
    for (auto& w : v)
        stm >> w;
    return stm;
}

template<typename A, typename B>
ByteInputStream& operator>>(ByteInputStream& stm, std::pair<A, B>& v)
{
	stm >> v.first >> v.second;
	return stm;
}

