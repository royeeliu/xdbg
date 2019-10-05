#pragma once

#include "DebugEngine.h"

namespace xdbg {

	class Type
	{
	public:
		Type(IDebugSymbols3* symbols, uint64_t module_base, uint32_t id);
		~Type();

		Type(Type const&) = default;
		Type& operator=(Type const&) = default;

		Module GetModule(void) const; 
		uint32_t GetId(void) const { return m_id; }

		const std::wstring GetName(void) const;
		uint32_t GetSize(void) const;

	private:
		CComPtr<IDebugSymbols3>		m_symbols;
		const uint64_t				m_module;
		const uint32_t				m_id;
	};

} // End of namespace xdbg
