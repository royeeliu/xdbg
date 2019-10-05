#pragma once

#include "DebugEngine.h"

namespace xdbg {

	class Thread
	{
	public:
		Thread(IDebugSystemObjects4* system_objects, uint32_t engine_id, uint32_t system_id)
			: m_systemObjects(system_objects)
			, m_engineId(engine_id)
			, m_systemId(system_id)
		{
		}

		uint32_t GetEngineId() const { return m_engineId; }
		uint32_t GetSystemId() const { return m_systemId; }

	protected:
		CComPtr<IDebugSystemObjects4>	m_systemObjects;
		const uint32_t					m_engineId;
		const uint32_t					m_systemId;
	};

	class CurrentThread : public Thread
	{
	public:
		CurrentThread(IDebugSystemObjects4* system_objects, uint32_t engine_id, uint32_t system_id)
			: Thread(system_objects, engine_id, system_id)
		{
		}

		uint64_t GetDataOffset() const;
		uint64_t GetHandle() const;
	};

} // End of namespace xdbg
