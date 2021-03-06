#include "mlpch.h"
#include "Texture.h"

#include "Milky/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Milky {

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, TextureFlags flags)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ML_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height, flags);
		}

		ML_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path, TextureFlags flags)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: ML_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path, flags);
		}

		ML_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
