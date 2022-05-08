#include "mlpch.h"
#include "Milky/Renderer/GraphicsContext.h"

#include "Milky/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Milky {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    ML_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		ML_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
