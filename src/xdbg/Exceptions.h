#pragma once

#include "common.h"
#include <exception>
#include <list>
#include <sstream>

namespace xdbg {

	struct SourceLocation
	{
		const wchar_t*	file_name;
		int				file_line;
		const wchar_t*	function_name;
	};

	struct ErrorInfo
	{
		std::wstring key;
		std::wstring value;
	};


	class RuntimeError : virtual public std::exception
	{
	public:
		explicit
		RuntimeError(std::wstring msg)
			: m_message(!msg.empty() ? std::move(msg) : L"runtime error")
		{
			// ��֪��Ϊʲô�����û��� std::exception ��������Ķ��� what ��ϢΪ�� 
		}

		explicit
		RuntimeError(const wchar_t* msg)
			: m_message(msg != nullptr ? msg : L"runtime error")
		{
		}

		virtual ~RuntimeError()
		{
		}

		RuntimeError(RuntimeError const& src)
			: m_message(src.m_message)
			, m_locations(src.m_locations)
			, m_infos(src.m_infos)
		{
		}

		RuntimeError(RuntimeError&& src)
			: m_message(std::move(src.m_message))
			, m_locations(std::move(src.m_locations))
			, m_infos(std::move(src.m_infos))
		{
		}

		virtual char const* what() const override
		{
			m_what = xdbg::WToU8(GetDiagnosticInformation(L"\t< "));
			return m_what.c_str();
		}

		void AppendSourceLocation(const SourceLocation& location) const
		{
			m_locations.push_back(location);
		}

		void AppendErrorInfo(ErrorInfo&& info) const
		{
			m_infos.push_back(std::move(info));
		}

		const wchar_t* GetDiagnosticInformation(wchar_t const* prefix = L"",  bool verbose = true) const
		{
			if (!prefix)
				prefix = L"";

			std::wostringstream oss;

			oss << m_message << std::endl;

			for (ErrorInfo& item : m_infos)
			{
				oss << prefix << item.key << L": " << item.value << std::endl;
			}

			if (verbose)
			{
				for (SourceLocation& location : m_locations)
				{
					oss << prefix << L"location: " << (location.file_name != nullptr ? location.file_name : L"(unknown file)") << "(" << location.file_line << ")";

					if (location.function_name != nullptr)
					{
						oss << L": " << location.function_name;
					}

					oss << std::endl;
				}
			}

			m_diagnostic = oss.str();
			return m_diagnostic.c_str();
		}

	protected:
		static inline std::wstring _MakeMessage(std::wstring msg, const wchar_t* description)
		{
			if (!msg.empty())
			{
				msg.append(L" - ");
			}
			msg.append(description ? description : L"(none)");

			return msg;
		}

		static inline std::wstring _MakeMessage(std::wstring msg, const std::wstring& description)
		{
			if (!msg.empty())
			{
				msg.append(L" - ");
			}
			msg.append(!description.empty() ? description : L"(none)");

			return msg;
		}

	private:
		template<class E>
		friend E const& operator<<(E const& x, const ::xdbg::SourceLocation& v);

		template<class E>
		friend E const& operator<<(E const& x, ::xdbg::ErrorInfo&& v);

		template<class E>
		friend E const& operator<<(E const& x, std::list<::xdbg::ErrorInfo>&& items_);

	private:
		mutable std::list<SourceLocation>	m_locations;
		mutable std::list<ErrorInfo>		m_infos;
		mutable std::string					m_what;
		mutable std::wstring				m_diagnostic;
		const std::wstring					m_message;
	};

	template<class E>
	inline E const& operator<<(E const& x, ::xdbg::ErrorInfo&& v)
	{
		static_assert(std::is_base_of<::xdbg::RuntimeError, E>::value, "class E should be derived from QLC::Exception");
		static_cast<::xdbg::RuntimeError const&>(x).AppendErrorInfo(std::move(v));
		return x;
	}

	template<class E>
	inline E const& operator<<(E const& x, const ::xdbg::SourceLocation& v)
	{
		static_assert(std::is_base_of<::xdbg::RuntimeError, E>::value, "class E should be derived from QLC::Exception");
		static_cast<::xdbg::RuntimeError const&>(x).AppendSourceLocation(v);
		return x;
	}

	template<class E>
	inline E const& operator<<(E const& x, std::list<::xdbg::ErrorInfo>&& items_)
	{
		static_assert(std::is_base_of<::xdbg::RuntimeError, E>::value, "class E should be derived from QLC::Exception");

		for (::xdbg::ErrorInfo& v : items_)
		{
			static_cast<::xdbg::RuntimeError const&>(x).AppendErrorInfo(std::move(v));
		}
		return x;
	}

	template<class E>
	inline __declspec(noreturn) void ThrowException(E const& x)
	{
		throw x;
	}

	template<typename V>
	ErrorInfo MakeErrorInfo(const wchar_t* key, V value)
	{
		std::wostringstream oss;
		oss << value;
		return ErrorInfo{ key ? key : L"(null)", oss.str() };
	}


	//////////////////////////////////////////////////////////////////////////
	// Exceptions
	namespace Exceptions {

		// �ڴ治��
		class OutOfMemory : public RuntimeError
		{
		public:
			OutOfMemory(const wchar_t* msg = nullptr)
				: RuntimeError(_MakeMessage(msg ? msg : L""))
			{
			}

			OutOfMemory(const std::wstring& msg)
				: RuntimeError(_MakeMessage(msg))
			{
			}

			OutOfMemory(int size, const std::wstring& msg)
				: OutOfMemory(msg)
			{
				__super::AppendErrorInfo(MakeErrorInfo(L"alloc size", size));
			}

			OutOfMemory(int size, const wchar_t* msg = nullptr)
				: OutOfMemory(size, std::wstring(msg ? msg : L""))
			{
			}

		private:
			static inline std::wstring _MakeMessage(std::wstring msg)
			{
				return __super::_MakeMessage(L"OutOfMemory", std::move(msg));
			}
		};

		// �߼�����
		class LogicError : public RuntimeError
		{
		public:
			LogicError(const wchar_t* msg = nullptr)
				: RuntimeError(_MakeMessage(msg ? msg : L""))
			{
			}

			LogicError(const std::wstring& msg)
				: RuntimeError(_MakeMessage(msg))
			{
			}

		private:
			static inline std::wstring _MakeMessage(std::wstring msg)
			{
				return __super::_MakeMessage(L"LogicError", std::move(msg));
			}
		};



	} // End of namespace Exceptions

} // End of namespace xdbg

#define XDBG_MAKE_SOURCE_LOCATION()			(::xdbg::SourceLocation{ __FILEW__, __LINE__, __FUNCTIONW__ })
#define XDBG_THROW_EXCEPTION(x)				::xdbg::ThrowException((x) << XDBG_MAKE_SOURCE_LOCATION())
#define XDBG_THROW_EXCEPTION_IF(expr, x)	do { if(!!(expr)) XDBG_THROW_EXCEPTION(x); } while(0)