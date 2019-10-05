#include "common.h"
#include "SystemObjects.h"


namespace xdbg {

	using xdbg::Exceptions::DebugEngineError;

	uint64_t CurrentThread::GetDataOffset() const
	{
		ULONG64 offset = 0;
		HRESULT hr = m_systemObjects->GetCurrentThreadDataOffset(&offset);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSystemObjects4::GetCurrentThreadDataOffset", hr));

		return offset;
	}

	uint64_t CurrentThread::GetHandle() const
	{
		ULONG64 handle = 0;
		HRESULT hr = m_systemObjects->GetCurrentThreadHandle(&handle);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSystemObjects4::GetCurrentThreadHandle", hr));

		return handle;
	}

} // End of namespace xdbg
