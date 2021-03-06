// common.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>
#include <atlbase.h>
#include <assert.h>

// STL headers
#include <string>
#include <vector>

// namespace xdbg
namespace xdbg {

	using ArgmentArray = std::vector<std::string>;

	ArgmentArray SplitArgs(const char* line);

	/**
	 * 将文件版本号格式化为 xx.xx.xx.xx 的字符串形式，高位不补 0
	 */
	std::string FormatFileVersion(VS_FIXEDFILEINFO const& vi);

	/**
	 * 将产品版本号格式化为 xx.xx.xx.xx 的字符串形式，高位不补 0
	 */
	std::string FormatProductVersion(VS_FIXEDFILEINFO const& vi);

	std::wstring GetCurrentModuleFullPath();
	std::wstring GetCurrentModuleDirectory();

	std::wstring GetConfig(const wchar_t* config_file, const wchar_t* section, const wchar_t* key);

	std::string WideToMultiByteString(const wchar_t* str, unsigned code_page);
	std::wstring MutiByteToWideString(unsigned code_page, const char* str, int size);

	inline std::string WToU8(const wchar_t* str)
	{
		return WideToMultiByteString(str, CP_UTF8);
	}

	inline std::string WToU8(const std::wstring& str)
	{
		return WideToMultiByteString(str.c_str(), CP_UTF8);
	}

	inline std::string WToAnsi(const std::wstring& str)
	{
		return WideToMultiByteString(str.c_str(), CP_ACP);
	}

	inline std::string WToAnsi(const wchar_t* str)
	{
		return WideToMultiByteString(str, CP_ACP);
	}

	inline std::wstring U8ToW(const std::string& str)
	{
		return MutiByteToWideString(CP_UTF8, str.c_str(), (int)str.size());
	}

	inline std::wstring U8ToW(const char* str)
	{
		return (str != nullptr) ? MutiByteToWideString(CP_UTF8, str, (int)strlen(str)) : std::wstring();
	}

	inline std::wstring AnsiToW(const std::string& str)
	{
		return MutiByteToWideString(CP_ACP, str.c_str(), (int)str.size());
	}

	inline std::wstring AnsiToW(const char* str)
	{
		return (str != nullptr) ? MutiByteToWideString(CP_ACP, str, (int)strlen(str)) : std::wstring();
	}

	inline std::wstring FormatVaList(const wchar_t* format, va_list args)
	{
		int len = _vscwprintf(format, args) + 1;
		std::vector<wchar_t> buf(len, '\0');
		vswprintf_s(buf.data(), len, format, args);

		return buf.data();
	}

	inline std::string FormatVaListA(const char* format, va_list args)
	{
		int len = _vscprintf(format, args) + 1;
		std::vector<char> buf(len, '\0');
		vsprintf_s(buf.data(), len, format, args);
		return buf.data();
	}

	inline std::wstring Format(const wchar_t* format, ...)
	{
		va_list arglist;
		va_start(arglist, format);
		std::wstring wstr = FormatVaList(format, arglist);
		va_end(arglist);

		return wstr;
	}

	inline std::string FormatA(const char* format, ...)
	{
		va_list arglist;
		va_start(arglist, format);
		std::string str = FormatVaListA(format, arglist);
		va_end(arglist);

		return str;
	}

} // End of namesapce xdbg


// 全局变量声明

extern HMODULE	g_hModule;

static wchar_t const * const	EXTENSION_NAME		= L"xdbg";
static char const * const		EXTENSION_NAME_A	= "xdbg";

