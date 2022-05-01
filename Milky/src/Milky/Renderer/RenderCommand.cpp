#include "mlpch.h"
#include "Milky/Renderer/RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Milky {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();

}
