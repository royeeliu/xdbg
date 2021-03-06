// xdbg.cpp: 定义 DLL 应用程序的导出函数。
//

#include "common.h"
#include "ExtensionContext.h"
#include "DebugSession.h"

#pragma comment(lib, "dbgeng.lib")

#ifdef XDBG_EXPORTS
#define XDBG_API __declspec(dllexport)
#else
#define XDBG_API __declspec(dllimport)
#endif

using namespace xdbg;

WINDBG_EXTENSION_APIS64					ExtensionApis{};
std::unique_ptr<xdbg::ExtensionContext>	g_global;
HMODULE									g_hModule = nullptr;

extern "C" XDBG_API HRESULT CALLBACK DebugExtensionInitialize(PULONG pVersion, PULONG pFlags)
{
	*pVersion = DEBUG_EXTENSION_VERSION(1, 0);
	*pFlags = 0;

	HRESULT hr = S_OK;
	
	do 
	{
		if (g_global)
		{
			dprintf("Extension is loaded already: %s\n", WToAnsi(GetCurrentModuleFullPath()).c_str());
			break;
		}

		CComPtr<IDebugClient> spDebugClient;
		hr = ::DebugCreate(IID_PPV_ARGS(&spDebugClient));
		if (FAILED(hr)) break;

		CComPtr<IDebugControl> spDebugControl;
		hr = spDebugClient->QueryInterface(IID_PPV_ARGS(&spDebugControl));
		if (FAILED(hr)) break;

		//
		// Get the windbg-style extension APIS
		//
		ExtensionApis.nSize = sizeof(ExtensionApis);
		hr = spDebugControl->GetWindbgExtensionApis64(&ExtensionApis);
		if (FAILED(hr)) break;

		g_global = std::make_unique<xdbg::ExtensionContext>();

	} while (0);

	return hr;
}

extern "C" XDBG_API void CALLBACK DebugExtensionNotify(ULONG Notify, ULONG64 Argument)
{
	UNREFERENCED_PARAMETER(Argument);

	switch (Notify)
	{
	case DEBUG_NOTIFY_SESSION_ACTIVE:
	case DEBUG_NOTIFY_SESSION_INACTIVE:
	case DEBUG_NOTIFY_SESSION_ACCESSIBLE:
	case DEBUG_NOTIFY_SESSION_INACCESSIBLE:
		{
			break;
		}
	}
}

extern "C" XDBG_API void CALLBACK DebugExtensionUninitialize(void)
{
	g_global.reset();
	::ZeroMemory(&ExtensionApis, sizeof(ExtensionApis));
}

/**
 * A built-in help for the extension dll
 */
extern "C" XDBG_API HRESULT CALLBACK help(PDEBUG_CLIENT4 pClient, PCSTR args)
{
	DebugSession::Scope use(pClient);

	g_global->Output(L"Help for xdbg.pyd\n"
		L"\t help                - Show this help\n"
		L"\t py                  - Execute python script\n"
		L"\t pyrun               - Execute a python string\n"
		L"\t eval                - Evaluate a python expression\n"
		L"\t setscriptdir        - Set python script directory\n"
	);

	return DEBUG_EXTENSION_CONTINUE_SEARCH;
}

extern "C" XDBG_API HRESULT CALLBACK py(PDEBUG_CLIENT4 pClient, PCSTR args)
{
	DebugSession::Scope use(pClient);
	int ret = g_global->ExecuteFile(args);
	return (ret >= 0) ? S_OK : E_FAIL;
}

extern "C" XDBG_API HRESULT CALLBACK pyrun(PDEBUG_CLIENT4 pClient, PCSTR args)
{
	DebugSession::Scope use(pClient);
	int ret = g_global->Execute(args);
	return (ret >= 0) ? S_OK : E_FAIL;
}

extern "C" XDBG_API HRESULT CALLBACK eval(PDEBUG_CLIENT4 pClient, PCSTR args)
{
	DebugSession::Scope use(pClient);
	int ret = g_global->Evaluate(args);
	return (ret >= 0) ? S_OK : E_FAIL;
}

extern "C" XDBG_API HRESULT CALLBACK setscriptdir(PDEBUG_CLIENT4 pClient, PCSTR args)
{
	DebugSession::Scope use(pClient);
	int ret = g_global->SetScriptDirectory(args);
	return (ret >= 0) ? S_OK : E_FAIL;
}


