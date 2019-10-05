#pragma once

#include "DebugEngine.h"
#include <array>

namespace xdbg {

	class StackFrame
	{
	public:
		StackFrame(const DEBUG_STACK_FRAME& frame)
			: m_frame(frame)
		{
		}

		~StackFrame(){}

		uint64_t GetInstructionOffset(void) const { return m_frame.InstructionOffset; }
		uint64_t GetReturnOffset(void) const { return m_frame.ReturnOffset; }
		uint64_t GetFrameOffset(void) const { return m_frame.FrameOffset; }
		uint64_t GetStackOffset(void) const { return m_frame.StackOffset; }
		uint64_t GetFuncTableEntry(void) const { return m_frame.FuncTableEntry; }
		std::array<uint64_t, 4> GetParams() const { return std::array<uint64_t, 4>{ m_frame.Params[0], m_frame.Params[1], m_frame.Params[2], m_frame.Params[3] }; }
		bool IsVirtual(void) const { return m_frame.Virtual; }
		uint32_t GetFrameNumber(void) const { return m_frame.FrameNumber; }

	private:
		const DEBUG_STACK_FRAME m_frame;
	};

} // End of namespace xdbg
