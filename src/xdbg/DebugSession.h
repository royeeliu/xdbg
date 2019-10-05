#pragma once

#include "DebugEngine.h"
#include "Module.h"
#include "Type.h"
#include "SystemObjects.h"
#include "StackFrame.h"

namespace xdbg {

	class DebugSession 
	{
	public:
		static CComQIPtr<IDebugClient4> s_current;

		struct Scope
		{
			CComQIPtr<IDebugClient4> m_last;

			Scope(IDebugClient4* client)
				: m_last(s_current)
			{
				DebugSession::s_current = client;
			}

			~Scope(void)
			{
				DebugSession::s_current = m_last;
			}
		};

	public:
		static DebugSession OpenDumpFile(std::wstring const& file_name);
		static DebugSession GetCurrent();

	public:
		explicit
		DebugSession(IDebugClient4* client);
		~DebugSession();

		DebugSession(DebugSession const&) = default;
		DebugSession& operator=(DebugSession const&) = default;

		std::vector<Module> GetLoadedModules();
		std::vector<Module> GetUnloadedModules();
		std::vector<Thread> GetThreads();
		std::vector<StackFrame> GetStackFrames(uint64_t frame_offset = 0, uint64_t stack_offset = 0, uint64_t instruction_offset = 0);

		CurrentThread GetCurrentThread();
		void SetCurrentThread(const Thread& thread);

		std::wstring GetNameByOffset(uint64_t offset, int32_t delta = 0);

		void AppendSourcePath(std::wstring const& path);

	private:
		CComQIPtr<IDebugClient4>		m_client;
		CComQIPtr<IDebugControl4>		m_control;
		CComQIPtr<IDebugSymbols3>		m_symbols;
		CComQIPtr<IDebugSystemObjects4> m_systemObjects;
	};


} // End of namespace xdbg
