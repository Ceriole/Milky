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
		ML_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ML_CORE_ASSERT(status, "Failed to initialize Glad!");

		ML_CORE_INFO("OpenGL Context:");
		ML_CORE_INFO("\tVendor: {0}", glGetString(GL_VENDOR));
		ML_CORE_INFO("\tRenderer: {0}", glGetString(GL_RENDERER));
		ML_CORE_INFO("\tVersion: {0}", glGetString(GL_VERSION));

#ifdef ML_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		ML_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Milky requires at least OpenGL version 4.5!");
#endif

	}

	void OpenGLContext::SwapBuffers()
	{
		ML_PROFILE_FUNCTION();

		glfwPollEvents();
		glfwSwapBuffers(m_WindowHandle);
	}

}
