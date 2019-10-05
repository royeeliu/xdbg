#include "common.h"
#include "DebugOutput.h"
#include "DebugSession.h"

namespace xdbg {

	void DebugOutput::Write(const char * text)
	{
		if (DebugSession::s_current)
		{
			CComQIPtr<IDebugControl4>(DebugSession::s_current)->OutputWide(DEBUG_OUTPUT_NORMAL, L"%s", U8ToW(text).c_str());
		}
		else
		{
			// 字符串带中文且作为变参参数时 dprintf 以及其他输出字符串的方法都无法正常显示，所以优先使用宽字符输出函数
			dprintf("%s", WToAnsi(U8ToW(text)).c_str());
		}
	}

} // End of namespace xdbg