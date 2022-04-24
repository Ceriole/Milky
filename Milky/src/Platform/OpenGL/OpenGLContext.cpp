#include "mlpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Milky {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		ML_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ML_CORE_ASSERT(status, "Failed to initialize Glad!");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_WindowHandle);
	}


}