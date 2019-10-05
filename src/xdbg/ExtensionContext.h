#pragma once

#include "PythonEngine.h"

namespace xdbg {

	class ExtensionContext
	{
	public:
		ExtensionContext();
		~ExtensionContext();

		ExtensionContext(ExtensionContext const&) = delete;
		ExtensionContext& operator=(ExtensionContext const&) = delete;

		int ExecuteFile(const char* args) noexcept;
		int Execute(const char* args) noexcept;
		int Evaluate(const char* args) noexcept;
		int SetScriptDirectory(const char* args) noexcept;

		void Output(const wchar_t* format, ...);
		void OutputError(const wchar_t* format, ...);
		void OutputWarning(const wchar_t* format, ...);

	private:
		const std::wstring			m_moduleDirectory;
		const std::wstring			m_configFile;
		std::wstring				m_scriptDirectory;
		PythonEngine				m_python;
	};

} // End of namespace xdbg
