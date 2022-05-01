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

		ML_CORE_INFO("OpenGL Context:");
		ML_CORE_INFO("\tVendor: {0}", glGetString(GL_VENDOR));
		ML_CORE_INFO("\tRenderer: {0}", glGetString(GL_RENDERER));
		ML_CORE_INFO("\tVersion: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_WindowHandle);
	}

	std::string OpenGLContext::GetInfoString()
	{
		std::stringstream ss;
		ss << "OpenGL Context:" << std::endl;
		ss << "\tVendor: " << glGetString(GL_VENDOR) << std::endl;
		ss << "\tRenderer: " << glGetString(GL_RENDERER) << std::endl;
		ss << "\tVersion: " << glGetString(GL_VERSION) << std::endl;
		return ss.str();
	}

}
