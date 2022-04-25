#include "mlpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Milky {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();

}
