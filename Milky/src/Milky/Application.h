#pragma once

#include "Core.h"

#include "Window.h"
#include "Milky/LayerStack.h"
#include "Events/Event.h"
#include "Milky/Events/ApplicationEvent.h"

#include "Milky/ImGui/ImGuiLayer.h"

namespace Milky {
	
	class  Application
	{
	public:
		Application();
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Close();

		Window& GetWindow() { return *m_Window; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application& Get() { return *s_Instance; }
		void Run();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}