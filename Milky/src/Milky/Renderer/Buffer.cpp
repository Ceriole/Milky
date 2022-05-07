#include "mlpch.h"
#include "Buffer.h"

#include "Milky/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Milky {

	//////////////////////////////////////////////////////////////////////////////////
	// Vertex Buffer /////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ML_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
		}

		ML_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None : ML_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		ML_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// Index Buffer //////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* vertices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ML_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLIndexBuffer>(vertices, count);
		}

		ML_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
