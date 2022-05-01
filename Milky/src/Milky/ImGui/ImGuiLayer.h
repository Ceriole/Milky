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
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}
