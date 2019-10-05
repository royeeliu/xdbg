#include "common.h"
#include "DebugEngine.h"
#include "PythonHelper.h"
#include "DebugSession.h"
#include "DebugOutput.h"

#include <boost/python.hpp>
using namespace boost;

namespace xdbg {


	static python::dict get_loaded_modules(DebugSession& session)
	{
		python::dict dict;
		
		auto modules = session.GetLoadedModules();

		for (auto& module : modules)
		{
			dict[WToU8(module.GetModuleName())] = module;
		}

		return dict;
	}

	static python::dict get_unloaded_modules(DebugSession& session)
	{
		python::dict dict;

		auto modules = session.GetUnloadedModules();

		for (auto& module : modules)
		{
			dict[WToU8(module.GetModuleName())] = module;
		}

		return dict;
	}

	static python::list get_threads(DebugSession& session)
	{
		python::list list;

		auto threads = session.GetThreads();

		for (auto& thread : threads)
		{
			list.append(thread);
		}

		return list;
	}

	static python::list get_stack_frames(DebugSession& session, uint64_t frame_offset, uint64_t stack_offset, uint64_t instruction_offset)
	{
		python::list list;

		auto frames = session.GetStackFrames(frame_offset, stack_offset, instruction_offset);

		for (auto& frame : frames)
		{
			list.append(frame);
		}

		return list;
	}

	static python::tuple get_file_version(Module& module)
	{
		auto version = module.GetFileVersion();
		return python::make_tuple(version.major, version.minor, version.revision, version.build);
	}

	static python::tuple get_product_version(Module& module)
	{
		auto version = module.GetProductVersion();
		return python::make_tuple(version.major, version.minor, version.revision, version.build);
	}

	static python::list get_stack_frame_params(StackFrame& frame)
	{
		python::list list;
		auto params = frame.GetParams();

		for (auto& param : params)
		{
			list.append(param);
		}

		return list;
	}

	static python::object ModuleRepr(const Module& module)
	{
		return "(Module %s @ %08x)" % python::make_tuple(module.GetModuleName(), module.GetBase());
	}

	static python::object TypeRepr(const Type& type)
	{
		return "(Type %s!%s)" % python::make_tuple(type.GetModule().GetModuleName(), type.GetName());
	}

	static python::object ThreadRepr(const Thread& thread)
	{
		return "(Thread %d %d)" % python::make_tuple(thread.GetEngineId(), thread.GetSystemId());
	}

	static python::object StackFrameRepr(const StackFrame& frame)
	{
		return "(StackFrame %d @ %08x)" % python::make_tuple(frame.GetFrameNumber(), frame.GetFrameOffset());
	}

	static void ExportClassType()
	{
		python::class_<Type>("Type", python::no_init)
			.add_property("module", &Type::GetModule, "the module containing the symbol")
			.add_property("id", &Type::GetId, "the type ID")
			.add_property("name", &Type::GetName, "name of the type symbol")
			.add_property("size", &Type::GetSize, "the number of bytes of memory an instance of the type.")
			.def("__repr__", &TypeRepr)
			;
	}

	static void ExprotClassModule()
	{
		python::class_<Module>("Module", python::no_init)
			.add_property("base", &Module::GetBase)
			.add_property("image_name", &Module::GetImageName, "The name of the executable file, including the extension.")
			.add_property("module_name", &Module::GetModuleName, "Just the file name without the extension.")
			.add_property("loaded_image_name", &Module::GetLoadedImageName, "The loaded image name.")
			.add_property("symbol_file_name", &Module::GetSymbolFileName, "The path and name of the symbol file.")
			.add_property("mapped_image_name", &Module::GetMappedImageName, "The mapped image name.")
			.add_property("file_version", &get_file_version, "File version")
			.add_property("product_version", &get_product_version, "Product version.")
			.def("get_type", &Module::GetTypeByName, "look up the specified type and return its type ID.")
			.def("reload", &Module::Reload, "delete the engine's symbol information for the specified module and reload these symbols as needed.")
			.def("__repr__", &ModuleRepr)
			;
	}

	static void ExportClassThread()
	{
		python::class_<Thread>("Thread", python::no_init)
			.add_property("engine_id", &Thread::GetEngineId)
			.add_property("system_id", &Thread::GetSystemId)
			.def("__repr__", &ThreadRepr)
			;

		python::class_<CurrentThread, python::bases<Thread> >("CurrentThread", python::no_init)
			.add_property("data_offset", &CurrentThread::GetDataOffset, "the location of the system data structure for the current thread.")
			.add_property("handle", &CurrentThread::GetHandle, "the system handle for the current thread.")
			;
	}

	static void ExportClassStackFrame()
	{
		python::class_<StackFrame>("StackFrame", python::no_init)
			.add_property("instruction_offset", &StackFrame::GetInstructionOffset,
				"The location in the process's virtual address space of the related instruction for the stack frame.")
			.add_property("return_offset", &StackFrame::GetReturnOffset,
				"The location in the process's virtual address space of the return address for the stack frame.")
			.add_property("frame_offset", &StackFrame::GetFrameOffset,
				"The location in the process's virtual address space of the stack frame, if known.")
			.add_property("stack_offset", &StackFrame::GetStackOffset,
				"The location in the process's virtual address space of the processor stack.")
			.add_property("func_table_entry", &StackFrame::GetFuncTableEntry,
				"The location in the target's virtual address space of the function entry for this frame, if available.")
			.add_property("params", &get_stack_frame_params,
				"The values of the first four stack slots that are passed to the function, if available.")
			.add_property("virtual", &StackFrame::IsVirtual,
				"his stack frame was generated by the debugger by unwinding.")
			.add_property("frame_number", &StackFrame::GetFrameNumber,
				"The index of the frame. This index counts the number of frames from the top of the call stack. ")
			.def("__repr__", &StackFrameRepr)
			;
	}

	static void ExportClassDebugSession()
	{
		python::class_<DebugSession>("DebugSession", "debug session", python::no_init)
			.add_static_property("current", &DebugSession::GetCurrent, "get current session, only valid in WinDbg extension mode")
			.add_property("loaded_modules", &get_loaded_modules, "loaded modules in the current process's module list")
			.add_property("unloaded_modules", &get_unloaded_modules, "unloaded modules in the current process's module list.")
			.add_property("threads", &get_threads, "threads in the current process.")
			.add_property("current_thread", &DebugSession::GetCurrentThread, &DebugSession::SetCurrentThread, "the current thread.")

			.def("get_stack_frames", &get_stack_frames, (python::arg("frame_offset") = 0,
				python::arg("stack_offset") = 0, python::arg("instruction_offset") = 0),
				"the frames at the top of the specified call stack.")
			.def("get_name_by_offset", &DebugSession::GetNameByOffset, ("offset", python::arg("delta") = 0),
				"the name and displacement of the symbol at the specified location in the target's virtual address space.")
			.def("append_source_path", &DebugSession::AppendSourcePath, 
				"appends directories to the source path.")
			;
	}

	static void ExportClassDebugOutput()
	{
		python::class_<DebugOutput>("DebugOutput", "debug output", python::no_init)
			.def("write", &DebugOutput::Write)
			.def("flush", &DebugOutput::flush)
			.add_property("closed", &DebugOutput::closed)
			;
	}

	static void ExportGlobalFunctions()
	{
		python::def("open_dump_file", DebugSession::OpenDumpFile, "Create debugging session by opening a dump file");
	}

	static void ExportAll()
	{
		ExportClassType();
		ExprotClassModule();
		ExportClassThread();
		ExportClassStackFrame();
		ExportClassDebugOutput();
		ExportClassDebugSession();
		ExportGlobalFunctions();
	}

} // end of namespace xdbg

BOOST_PYTHON_MODULE(xdbg)
{
	xdbg::ExportAll();
}