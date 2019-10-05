#include "common.h"
#include "ExtensionContext.h"
#include "Exceptions.h"
#include "PythonHelper.h"
#include "DebugSession.h"

#include <Shlwapi.h>
#include <sstream>

#include <Python.h>
#include <boost/python.hpp>

using namespace boost;

namespace xdbg {

	ExtensionContext::ExtensionContext()
		: m_moduleDirectory(GetCurrentModuleDirectory())
		, m_configFile(GetCurrentModuleDirectory() + L"\\" + EXTENSION_NAME + L".ini")
		, m_python(GetCurrentModuleFullPath().c_str())
	{
	}

	ExtensionContext::~ExtensionContext()
	{
	}

	int ExtensionContext::ExecuteFile(const char* _args) noexcept
	{
		try
		{
			ArgmentArray args = SplitArgs(_args);
			XDBG_THROW_EXCEPTION_IF(args.empty(), RuntimeError(L"no argment"));

			std::wstring file_name = AnsiToW(args[0]);

			if (file_name.find(L".py") == std::wstring::npos)
			{
				file_name.append(L".py");
			}

			if (!::PathFileExists(file_name.c_str()))
			{
				wchar_t path[MAX_PATH]{};
				wcscpy_s(path, file_name.c_str());

				std::vector<const wchar_t*> path_array;

				if (!m_scriptDirectory.empty())
				{
					path_array.push_back(m_scriptDirectory.c_str());
				}
				path_array.push_back(m_moduleDirectory.c_str());
				path_array.push_back(nullptr);

				BOOL br = ::PathFindOnPathW(path, path_array.data());
				XDBG_THROW_EXCEPTION_IF(!br, RuntimeError(L"no such file") << MakeErrorInfo(L"file", file_name));

				file_name = path;
				Output(L"Find script: %s\n", path);
			}

			FILE* pFile = _Py_wfopen(file_name.c_str(), L"r");
			XDBG_THROW_EXCEPTION_IF(!pFile, RuntimeError(L"open file failed") << MakeErrorInfo(L"file", file_name) << MakeErrorInfo(L"errno", errno));

			int pyerr = ::PyRun_SimpleFileEx(pFile, WToU8(file_name).c_str(), 1);
			pFile = nullptr;
			XDBG_THROW_EXCEPTION_IF(pyerr != 0, RuntimeError(L"run file failed") << MakeErrorInfo(L"file", file_name) << MakeErrorInfo(L"pyerr", pyerr));

			return 0;
		}
		catch (RuntimeError& ex)
		{
			OutputError(L"Execute file failed: \n%s", ex.GetDiagnosticInformation(L"\t< "));
			return -1;
		}
	}

	int ExtensionContext::Execute(const char* args) noexcept
	{
		try
		{
			std::string str = WToU8(AnsiToW(args));
			int pyerr = ::PyRun_SimpleString(str.c_str());
			XDBG_THROW_EXCEPTION_IF(pyerr != 0, RuntimeError(L"run string failed") << MakeErrorInfo(L"str", AnsiToW(args).c_str()) << MakeErrorInfo(L"pyerr", pyerr));

			return 0;
		}
		catch (RuntimeError& ex)
		{
			OutputError(L"Execute file failed: \n%s", ex.GetDiagnosticInformation(L"\t< "));
			return -1;
		}
	}

	int ExtensionContext::Evaluate(const char* args) noexcept
	{
		try
		{
			python::object main_module = python::import("__main__");
			python::object main_namespace = main_module.attr("__dict__");

			std::string str = WToU8(AnsiToW(args));
			auto result = python::object(python::handle<>(python::allow_null(
				::PyRun_String(str.c_str(), Py_eval_input, main_namespace.ptr(), nullptr))));
			Output(L"%s\n", U8ToW(python_helper::repr(result)).c_str());

			return 0;
		}
		catch (python::error_already_set)
		{
			PyErr_Print();
			return -1;
		}
	}

	int ExtensionContext::SetScriptDirectory(const char* args) noexcept
	{
		std::wstring dir = AnsiToW(args);
		BOOL br = ::PathIsDirectory(dir.c_str());

		if (br)
		{
			m_scriptDirectory = std::move(dir);
		}
		else
		{
			OutputError(L"no such directory: %s%d\n", dir.c_str());
		}

		return br ? 0 : -1;
	}

	void ExtensionContext::Output(const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);

		if (DebugSession::s_current)
		{
			CComQIPtr<IDebugControl4>(DebugSession::s_current)->OutputVaListWide(DEBUG_OUTPUT_NORMAL, format, args);
		}
		else
		{
			dprintf("%s", WToAnsi(FormatVaList(format, args)).c_str());
		}

		va_end(args);
	}

	void ExtensionContext::OutputError(const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);

		if (DebugSession::s_current)
		{
			CComQIPtr<IDebugControl4>(DebugSession::s_current)->OutputVaListWide(DEBUG_OUTPUT_NORMAL, format, args);
		}
		else
		{
			dprintf("%s", WToAnsi(FormatVaList(format, args)).c_str());
		}		
		
		va_end(args);
	}

	void ExtensionContext::OutputWarning(const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);

		if (DebugSession::s_current)
		{
			CComQIPtr<IDebugControl4>(DebugSession::s_current)->OutputVaListWide(DEBUG_OUTPUT_NORMAL, format, args);
		}
		else
		{
			dprintf("%s", WToAnsi(FormatVaList(format, args)).c_str());
		}

		va_end(args);
	}

} // End of namespace xdbg
