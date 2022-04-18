#include "Application.h"

#include "Milky/Events/ApplicationEvent.h"
#include "Milky/Log.h"

namespace Milky {

	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		ML_TRACE(e);
		while (true);
	}

}