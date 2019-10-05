#pragma once

#include <boost/python.hpp>

namespace python_helper {

	using u8char_t = char;
	using u8string_t = std::string;

	u8string_t repr(const boost::python::object& obj) throw(...);

} // end of namespace python_helper