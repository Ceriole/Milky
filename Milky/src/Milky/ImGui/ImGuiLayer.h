#pragma once

#include "Milky/Core/Layer.h"

#include "Milky/Events/ApplicationEvent.h"
#include "Milky/Events/KeyEvent.h"
#include "Milky/Events/MouseEvent.h"

namespace Milky {

	class  ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override; 

		void Begin();
		void End();

		virtual void OnEvent(Event& e) override;

		void SetBlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};

}
