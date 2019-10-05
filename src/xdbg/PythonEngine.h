#pragma once


namespace xdbg {

	class PythonEngine
	{
	public:
		explicit
		PythonEngine(const wchar_t* module_name);
		~PythonEngine();

		PythonEngine(PythonEngine const&) = delete;
		PythonEngine& operator=(PythonEngine const&) = delete;
	};

} // End of namespace xdbg
