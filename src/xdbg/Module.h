#pragma once

#include "DebugEngine.h"

namespace xdbg {

	class Module
	{
	public:
		Module(IDebugSymbols3* symbols, uint64_t base);
		~Module();

		Module(Module const&) = default;
		Module& operator=(Module const&) = default;

		uint64_t GetBase(void) const { return m_base; }

		std::wstring GetImageName(void) const { return GetModuleNameString(DEBUG_MODNAME_IMAGE); }
		std::wstring GetModuleName(void) const { return GetModuleNameString(DEBUG_MODNAME_MODULE); }
		std::wstring GetLoadedImageName(void) const { return GetModuleNameString(DEBUG_MODNAME_LOADED_IMAGE); }
		std::wstring GetSymbolFileName(void) const { return GetModuleNameString(DEBUG_MODNAME_SYMBOL_FILE); }
		std::wstring GetMappedImageName(void) const { return GetModuleNameString(DEBUG_MODNAME_MAPPED_IMAGE); }
		Version GetProductVersion(void) const;
		Version GetFileVersion(void) const;
		
		Type GetTypeByName(const std::wstring& name) const;

		void Reload(void) const;

	private:
		std::wstring GetModuleNameString(uint32_t which) const;

	private:
		CComPtr<IDebugSymbols3>		m_symbols;
		const uint64_t				m_base;
	};

} // End of namespace xdbg
