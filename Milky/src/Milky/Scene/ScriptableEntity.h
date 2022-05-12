#pragma once

#include "Entity.h"

namespace Milky {

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() = default;

		template<typename... Comps>
		decltype(auto) GetComponent()
		{
			return m_Entity.GetComponent<Comps...>();
		}

		template<typename... Comps>
		bool HasComponent()
		{
			return m_Entity.HasComponent<Comps...>();
		}
	protected:
		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnUpdate(Timestep ts) = 0;
	private:
		Entity m_Entity;
		friend class Scene;
	};

}
