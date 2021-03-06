#include "common.h"
#include <shellapi.h>
#include <regex>
#include <sstream>

namespace xdbg {

	inline bool IsSpaceChar(char ch)
	{
		switch (ch)
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case '\f':
			return true;

		default:
			return false;
		}
	}

	static void PopBackSpaces(std::string& str)
	{
		while (!str.empty())
		{
			if (IsSpaceChar(str.back()))
			{
				str.pop_back();
			}
			else
			{
				break;
			}
		}
	}

	ArgmentArray SplitArgs(const char* line)
	{
		ArgmentArray args;

		if (!line)
		{
			return args;
		}

		const char* pos = line;
		bool ended = false;
		bool quot_start = false;
		bool arg_start = false;
		bool arg_end = false;

		std::string arg;

		while (*pos != 0 && !ended)
		{
			const char ch = *pos;

			if (quot_start)
			{
				if (ch == '\"')
				{
					quot_start = false;

					if (arg_start)
					{
						arg_end = true;
					}
				}
				else
				{
					if (!arg_start)
					{
						if (!IsSpaceChar(ch))
						{
							arg_start = true;
							arg.push_back(ch);
						}
					}
					else
					{
						arg.push_back(ch);
					}
				}
			}
			else
			{
				switch (ch)
				{
				case '\"':
				{
					quot_start = true;

					if (arg_start)
					{
						arg_end = true;
					}
				}
				break;

				case ' ':
				case '\t':
				case '\r':
				case '\n':
				case '\f':
				{
					if (arg_start)
					{
						arg_end = true;
					}
				}
				break;

				default:
				{
					if (!arg_start)
					{
						assert(arg.empty());
						arg_start = true;
					}
					arg.push_back(ch);
				}
				break;
				}
			}

			if (arg_end)
			{
				arg_end = false;
				arg_start = false;

				PopBackSpaces(arg);
				assert(!arg.empty());
				args.push_back(std::move(arg));
			}

			pos++;
		}

		PopBackSpaces(arg);
		if (!arg.empty())
		{
			args.push_back(std::move(arg));
		}

		return args;
	}


	std::string FormatFileVersion(VS_FIXEDFILEINFO const& vi)
	{
		int major = HIWORD(vi.dwFileVersionMS);
		int minor = LOWORD(vi.dwFileVersionMS);
		int revision = HIWORD(vi.dwFileVersionLS);
		int build = LOWORD(vi.dwFileVersionLS);

		std::ostringstream oss;
		oss << major
			<< "." << minor
			<< "." << revision
			<< "." << build
			;

		return oss.str();
	}

	std::string FormatProductVersion(VS_FIXEDFILEINFO const& vi)
	{
		int major = HIWORD(vi.dwProductVersionMS);
		int minor = LOWORD(vi.dwProductVersionMS);
		int revision = HIWORD(vi.dwProductVersionLS);
		int build = LOWORD(vi.dwProductVersionLS);

		std::ostringstream oss;
		oss << major
			<< "." << minor
			<< "." << revision
			<< "." << build
			;

		return oss.str();
	}

	std::wstring GetCurrentModuleFullPath()
	{
		wchar_t path[MAX_PATH] = { 0 };
		::GetModuleFileName(g_hModule, path, MAX_PATH);
		return path;
	}

	std::wstring _GetCurrentModuleDirectory()
	{
		wchar_t path[MAX_PATH] = { 0 };
		::GetModuleFileName(g_hModule, path, MAX_PATH);

		wchar_t* pos = wcsrchr(path, '\\');

		if (pos)
		{
			*pos = L'\0';
		}

		return path;
	}
	
	std::wstring GetCurrentModuleDirectory()
	{
		static std::wstring s_dir = _GetCurrentModuleDirectory();
		return s_dir;
	}

	std::wstring GetConfig(const wchar_t* config_file, const wchar_t* section, const wchar_t* key)
	{
		wchar_t buf[256] = { 0 };
		const int buf_char_size = sizeof(buf) / sizeof(buf[0]);
		::GetPrivateProfileString(section, key, L"", buf, buf_char_size, config_file);

		return buf;
	}

	std::string WideToMultiByteString(const wchar_t* str, unsigned code_page)
	{
		std::string result;

		do
		{
			if (!str || str[0] == L'\0')
				break;

			int count = ::WideCharToMultiByte(code_page, 0, str, -1, NULL, 0, NULL, NULL);
			if (count == 0)
				break;

			result.resize(count);
			::WideCharToMultiByte(code_page, 0, str, -1, &result.front(), count, NULL, NULL);

		} while (0);

		while (!result.empty() && result.back() == '\0')
		{
			result.pop_back();
		}

		return result;
	}

	std::wstring MutiByteToWideString(unsigned code_page, const char* str, int size)
	{
		std::wstring wstr;

		do
		{
			if (!str || str[0] == '\0' || size <= 0) break;

			int count = ::MultiByteToWideChar(code_page, 0, str, size, NULL, 0);
			if (count == 0) break;

			wstr.resize(count);

			count = ::MultiByteToWideChar(code_page, 0, str, size, &wstr[0], (int)wstr.size());
			if (count == 0) break;

		} while (0);

		return wstr;
	}

} // End of namesapce xdbg
