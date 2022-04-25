#pragma once

#include "Milky/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Milky {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual std::string GetInfoString() override;
	private:
		GLFWwindow* m_WindowHandle;
	};

}