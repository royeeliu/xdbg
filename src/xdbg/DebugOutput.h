#pragma once

namespace xdbg {

	class DebugOutput
	{
	public:
		DebugOutput() = default;
		~DebugOutput() = default;

		DebugOutput(DebugOutput const&) = default;
		DebugOutput& operator=(DebugOutput const&) = default;

		void Write(const char* text);
		void flush() {}
		bool closed() { return false; }
	};

} // End of namesapce xdbg
