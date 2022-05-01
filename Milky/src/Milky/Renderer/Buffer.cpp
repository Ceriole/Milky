#include "mlpch.h"
#include "Milky/Renderer/Buffer.h"

#include "Milky/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Milky {

	//////////////////////////////////////////////////////////////////////////////////
	// Vertex Buffer /////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None : ML_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}

		ML_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// Index Buffer //////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	IndexBuffer* IndexBuffer::Create(uint32_t* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ML_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(vertices, size);
		}

		ML_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
