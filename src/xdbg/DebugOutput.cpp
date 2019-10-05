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
			// �ַ�������������Ϊ��β���ʱ dprintf �Լ���������ַ����ķ������޷�������ʾ����������ʹ�ÿ��ַ��������
			dprintf("%s", WToAnsi(U8ToW(text)).c_str());
		}
	}

} // End of namespace xdbg