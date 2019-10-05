#include "common.h"
#include "DebugSession.h"
#include "Exceptions.h"

namespace xdbg {

	using xdbg::Exceptions::DebugEngineError;
	using xdbg::Exceptions::LogicError;

	ATL::CComQIPtr<IDebugClient4> DebugSession::s_current;

	DebugSession DebugSession::OpenDumpFile(std::wstring const& file_name)
	{
		try
		{
			CComPtr<IDebugClient4> debug_client;
			HRESULT hr = ::DebugCreate(IID_PPV_ARGS(&debug_client));
			XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"DebugCreate", hr));

			hr = debug_client->OpenDumpFileWide(file_name.c_str(), 0);
			XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugClient4::OpenDumpFileWide", hr));

			CComQIPtr<IDebugControl4> debug_control(debug_client);
			hr = debug_control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
			XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugControl4::WaitForEvent", hr));

			hr = debug_control->OutputCurrentState(DEBUG_OUTCTL_IGNORE, DEBUG_CURRENT_SYMBOL);
			XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugControl4::OutputCurrentState", hr));

			return DebugSession(debug_client);
		}
		catch (RuntimeError& ex)
		{
			ex << MakeErrorInfo(L"dump file", file_name);
			throw;
		}

	}

	DebugSession DebugSession::GetCurrent()
	{
		XDBG_THROW_EXCEPTION_IF(FAILED(!s_current), LogicError(L"no current session, is not WinDbg extension mode?"));
		return DebugSession(s_current);
	}

	DebugSession::DebugSession(IDebugClient4* client)
		: m_client(client)
		, m_control(client)
		, m_symbols(client)
		, m_systemObjects(client)
	{
	}

	DebugSession::~DebugSession()
	{
	}

	std::vector<Module> DebugSession::GetLoadedModules()
	{
		ULONG loaded = 0;
		ULONG unloaded = 0;

		HRESULT hr = m_symbols->GetNumberModules(&loaded, &unloaded);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSymbols3::GetNumberModules", hr));

		std::vector<Module> modules;

		for (ULONG i = 0; i < loaded; i++)
		{
			ULONG64 base = 0;
			hr = m_symbols->GetModuleByIndex(i, &base);
			XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSymbols3::GetModuleByIndex", hr) << MakeErrorInfo(L"index", i));
			
			modules.push_back(Module(m_symbols, base));
		}

		return modules;
	}

	std::vector<xdbg::Module> DebugSession::GetUnloadedModules()
	{
		ULONG loaded = 0;
		ULONG unloaded = 0;

		HRESULT hr = m_symbols->GetNumberModules(&loaded, &unloaded);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSymbols3::GetNumberModules", hr));

		std::vector<Module> modules;

		for (ULONG i = loaded; i < loaded + unloaded; i++)
		{
			ULONG64 base = 0;
			hr = m_symbols->GetModuleByIndex(i, &base);
			XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSymbols3::GetModuleByIndex", hr) << MakeErrorInfo(L"index", i));

			modules.push_back(Module(m_symbols, base));
		}

		return modules;
	}

	std::vector<xdbg::Thread> DebugSession::GetThreads()
	{
		ULONG number = 0;

		HRESULT hr = m_systemObjects->GetNumberThreads(&number);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSystemObjects4::GetNumberThreads", hr));

		std::vector<ULONG> engine_ids(number);
		std::vector<ULONG> system_ids(number);

		hr = m_systemObjects->GetThreadIdsByIndex(0, number, &engine_ids[0], &system_ids[0]);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSystemObjects4::GetThreadIdsByIndex", hr));

		std::vector<Thread> threads;

		for (ULONG i = 0; i < number; i++)
		{
			threads.push_back(Thread(m_systemObjects, engine_ids[i], system_ids[i]));
		}

		return threads;
	}

	std::vector<xdbg::StackFrame> DebugSession::GetStackFrames(uint64_t frame_offset /*= 0*/, uint64_t stack_offset /*= 0*/, uint64_t instruction_offset /*= 0*/)
	{
		DEBUG_STACK_FRAME frames[MAX_STACK_DEPTH];
		ULONG filled = _countof(frames);

		HRESULT hr = m_control->GetStackTrace(frame_offset, stack_offset, instruction_offset, 
			frames, _countof(frames), &filled);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugControl4::GetStackTrace", hr));

		std::vector<StackFrame> result;

		for (ULONG i = 0; i < filled; i++)
		{
			result.push_back(StackFrame(frames[i]));
		}

		return result;
	}

	xdbg::CurrentThread DebugSession::GetCurrentThread()
	{
		ULONG engine_id = 0;
		ULONG system_id = 0;

		HRESULT hr = m_systemObjects->GetCurrentThreadId(&engine_id);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSystemObjects4::GetCurrentThreadId", hr));

		hr = m_systemObjects->GetCurrentThreadSystemId(&system_id);
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSystemObjects4::GetCurrentThreadSystemId", hr));

		return CurrentThread(m_systemObjects, engine_id, system_id);
	}

	void DebugSession::SetCurrentThread(const Thread& thread)
	{
		HRESULT hr = m_systemObjects->SetCurrentThreadId(thread.GetEngineId());
		XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSystemObjects4::SetCurrentThreadId", hr));
	}

	std::wstring DebugSession::GetNameByOffset(uint64_t offset, int32_t delta /*= 0*/)
	{
		wchar_t name[MAX_NAME_LENGTH];
		ULONG size = _countof(name);
		ULONG64 displacement = 0;

		if (delta != 0)
		{
			HRESULT hr = m_symbols->GetNearNameByOffsetWide(offset, delta, name, size, &size, &displacement);
			XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSystemObjects4::GetNearNameByOffsetWide", hr));
		}
		else
		{
			HRESULT hr = m_symbols->GetNameByOffsetWide(offset, name, size, &size, &displacement);
			XDBG_THROW_EXCEPTION_IF(FAILED(hr), DebugEngineError(L"IDebugSystemObjects4::GetNameByOffsetWide", hr));
		}

		return std::wstring(name, size - 1);
	}

	void DebugSession::AppendSourcePath(std::wstring const& path)
	{
		m_symbols->AppendSourcePathWide(path.c_str());
	}

} // End of namespace xdbg
