#include "common.h"
#include "Module.h"
#include "Exceptions.h"
#include "Type.h"


namespace xdbg {
	
	using xdbg::Exceptions::DebugEngineError;

	Module::Module(IDebugSymbols3* symbols, uint64_t base)
		: m_symbols(symbols)
		, m_base(base)
	{
	}

	Module::~Module()
	{
	}

	std::wstring Module::GetModuleNameString(uint32_t which) const
	{
		wchar_t name[MAX_PATH];
		ULONG name_length = _countof(name);

		HRESULT hr = m_symbols->GetModuleNameStringWide(which, DEBUG_ANY_ID, m_base, name, name_length, &name_length);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSymbols3::GetModuleNameStringWide", hr));

		return std::wstring(name, name_length - 1);
	}

	xdbg::Version Module::GetProductVersion(void) const
	{
		VS_FIXEDFILEINFO vi{};
		ULONG size = sizeof(vi);
		HRESULT hr = m_symbols->GetModuleVersionInformationWide(DEBUG_ANY_ID, m_base, L"\\", &vi, size, &size);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSymbols3::GetModuleVersionInformationWide", hr));
		
		Version version;
		version.major = HIWORD(vi.dwProductVersionMS);
		version.minor = LOWORD(vi.dwProductVersionMS);
		version.revision = HIWORD(vi.dwProductVersionLS);
		version.build = LOWORD(vi.dwProductVersionLS);

		return version;
	}

	xdbg::Version Module::GetFileVersion(void) const
	{
		VS_FIXEDFILEINFO vi{};
		ULONG size = sizeof(vi);
		HRESULT hr = m_symbols->GetModuleVersionInformationWide(DEBUG_ANY_ID, m_base, L"\\", &vi, size, &size);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSymbols3::GetModuleVersionInformationWide", hr));

		Version version;
		version.major = HIWORD(vi.dwFileVersionMS);
		version.minor = LOWORD(vi.dwFileVersionMS);
		version.revision = HIWORD(vi.dwFileVersionLS);
		version.build = LOWORD(vi.dwFileVersionLS);

		return version;
	}

	xdbg::Type Module::GetTypeByName(const std::wstring& name) const
	{
		ULONG id = 0;
		HRESULT hr = m_symbols->GetTypeIdWide(m_base, name.c_str(), &id);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSymbols3::GetTypeIdWide", hr) << MakeErrorInfo(L"name", name));

		return Type(m_symbols, m_base, id);
	}

	void Module::Reload(void) const
	{
		HRESULT hr = m_symbols->ReloadWide(GetModuleName().c_str());
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSymbols3::ReloadWide", hr));
	}

} // End of namespace xdbg
