#pragma once

#include "Milky/Core/Base.h"
#include "Milky/Core/Timestep.h"
#include "Milky/Events/Event.h"

namespace Milky {

	class  Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {};
		virtual void OnDetach() {};
		virtual void OnUpdateRuntime(Timestep ts) {};
		virtual void OnImGuiRender() {};
		virtual void OnEvent(Event& e) {};

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}

