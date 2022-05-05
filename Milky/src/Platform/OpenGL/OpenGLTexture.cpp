#include "mlpch.h"
#include "OpenGLTexture.h"

#include <stb_image.h>

#include <glm/gtc/type_ptr.hpp>

namespace Milky {

	GLint FilterTypeToGL(Texture::FilterType type)
	{
		switch (type)
		{
		default:
		case Texture::FilterType::NEAREST: return GL_NEAREST;
		case Texture::FilterType::LINEAR: return GL_LINEAR;
		case Texture::FilterType::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
		case Texture::FilterType::NEAREST_MIPMAP_LINEAR: return GL_NEAREST_MIPMAP_LINEAR;
		case Texture::FilterType::LINEAR_MIPMAP_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
		case Texture::FilterType::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
		}
	}

	GLint WrapTypeToGL(Texture::WrapType type)
	{
		switch (type)
		{
		default:
		case Texture::WrapType::REPEAT: return GL_REPEAT;
		case Texture::WrapType::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
		case Texture::WrapType::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
		case Texture::WrapType::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, TextureFlags flags)
		: m_Width(width), m_Height(height)
	{
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, FilterTypeToGL(flags.minFilter));
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, FilterTypeToGL(flags.magFilter));

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, WrapTypeToGL(flags.wrapping));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, WrapTypeToGL(flags.wrapping));

		glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(flags.borderColor));
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, TextureFlags flags)
		: m_Path(path)
	{
		int width, height, channels;
		ML_CORE_TRACE("Loading Texture: '{0}'", path);
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		ML_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if(channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		ML_CORE_ASSERT(internalFormat & dataFormat, "Image format not supported!");

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, FilterTypeToGL(flags.minFilter));
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, FilterTypeToGL(flags.magFilter));

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, WrapTypeToGL(flags.wrapping));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, WrapTypeToGL(flags.wrapping));

		glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(flags.borderColor));

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
#ifdef ML_ENABLE_ASSERTS
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		ML_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be size of the image!");
#endif
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}
