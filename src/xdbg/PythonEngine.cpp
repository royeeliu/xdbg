#include "common.h"
#include "PythonEngine.h"
#include "DebugOutput.h"

#include <Python.h>
#include <boost/python.hpp>

using namespace boost;

extern "C" PyObject* PyInit_xdbg();

namespace xdbg {

	PythonEngine::PythonEngine(const wchar_t* module_name)
	{
		::Py_SetProgramName(module_name);
		::PyImport_AppendInittab("xdbg", PyInit_xdbg);;
		::Py_Initialize();

		python::object main_module = python::import("__main__");
		python::object main_namespace = main_module.attr("__dict__");
		python::object xdbg_module = python::import("xdbg");
		python::object sys_module = python::import("sys");

		sys_module.attr("stdout") = python::object(DebugOutput());
		sys_module.attr("stderr") = python::object(DebugOutput());

		python::exec("import os, sys, xdbg\n", main_namespace);
		python::exec("from xdbg import *\n", main_namespace);
	}

	PythonEngine::~PythonEngine()
	{
		::Py_FinalizeEx();
	}

} // End of namespace xdbg
