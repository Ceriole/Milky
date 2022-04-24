#include "mlpch.h"
#include "Milky/Application.h"

#include "Milky/Log.h"

#include <glad/glad.h>

#include "Milky/Input.h"


namespace Milky {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		ML_CORE_ASSERT(!s_Instance, "Only one instance of Application is allowed!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEveventCallback(ML_BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(ML_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(ML_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			if (e.Handled)
				break;
			(*--it)->OnEvent(e);
		}
	}


	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(0.2f, 0.4f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate();

				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
				m_ImGuiLayer->End();
			}
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		
		return false;
	}
}