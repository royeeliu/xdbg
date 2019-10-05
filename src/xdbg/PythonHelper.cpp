#include "common.h"
#include "PythonHelper.h"

using namespace boost::python;

namespace python_helper {

	u8string_t python_helper::repr(const boost::python::object& obj)
	{
		return u8string_t(extract<char*>(object(handle<>(allow_null(::PyObject_Repr(obj.ptr()))))));
	}

} // end of namespace python_helper
