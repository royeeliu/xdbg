#include "common.h"
#include "Type.h"
#include "Module.h"
#include "Exceptions.h"

namespace xdbg {

	using xdbg::Exceptions::DebugEngineError;

	Type::Type(IDebugSymbols3* symbols, uint64_t module_base, uint32_t id)
		: m_symbols(symbols)
		, m_module(module_base)
		, m_id(id)
	{
	}

	Type::~Type()
	{
	}

	Module Type::GetModule(void) const
	{
		return Module(m_symbols, m_module); 
	}

	const std::wstring Type::GetName(void) const
	{
		wchar_t buffer[MAX_NAME_LENGTH];
		ULONG size = _countof(buffer);

		HRESULT hr = m_symbols->GetTypeNameWide(m_module, m_id, buffer, size, &size);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSymbols3::GetTypeNameWide", hr));

		return std::wstring(buffer, size - 1);
	}

	uint32_t Type::GetSize(void) const
	{
		ULONG size = 0;
		HRESULT hr = m_symbols->GetTypeSize(m_module, m_id, &size);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSymbols3::GetTypeSize", hr));

		return size;
	}
};
