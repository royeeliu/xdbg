#pragma once

#include "Exceptions.h"

//
// Define KDEXT_64BIT to make all wdbgexts APIs recognize 64 bit addresses
// It is recommended for extensions to use 64 bit headers from wdbgexts so
// the extensions could support 64 bit targets.
//
#define KDEXT_64BIT
#include <wdbgexts.h>
#include <dbgeng.h>


namespace xdbg {
	
	static const size_t MAX_PATH_LENGTH = 4096;
	static const size_t MAX_NAME_LENGTH = 1024;
	static const size_t MAX_MACRO_SIZE	= 1024;
	static const size_t MAX_STACK_DEPTH = 64;

	class DebugSession;
	class Type;
	class Module;

	struct Version
	{
		int major = 0;
		int minor = 0;
		int revision = 0;
		int build = 0;
	};

	namespace Exceptions {

		class DebugEngineError : public RuntimeError
		{
		public:
			DebugEngineError(const wchar_t* method, HRESULT hr)
				: RuntimeError(RuntimeError::_MakeMessage(L"DebugEngineError", method))
			{
				std::wostringstream oss;
				oss << L"0x" << std::hex << hr;
				AppendErrorInfo(MakeErrorInfo(L"hr", oss.str()));
			}
		};

	} // End of namespace Exceptions

} // End of namespace xdbg

