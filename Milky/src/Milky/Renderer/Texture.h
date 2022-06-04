#pragma once

#include "Milky/Core/Base.h"
#include <glm/glm.hpp>
#include <string>

namespace Milky {

	class Texture
	{
	public:
		enum class FilterType
		{
			NEAREST = 0,
			LINEAR,
			NEAREST_MIPMAP_NEAREST,
			NEAREST_MIPMAP_LINEAR,
			LINEAR_MIPMAP_NEAREST,
			LINEAR_MIPMAP_LINEAR
		};
		enum class WrapType
		{
			REPEAT = 0,
			MIRRORED_REPEAT,
			CLAMP_TO_EDGE,
			MIRROR_CLAMP_TO_EDGE,
			CLAMP_TO_BORDER
		};
		struct TextureFlags {
			FilterType minFilter = FilterType::LINEAR, magFilter = FilterType::LINEAR;
			WrapType wrapping = WrapType::REPEAT;
			glm::vec4 borderColor = { 0,0,0,0 };
		};
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureFlags flags = TextureFlags());
		static Ref<Texture2D> Create(const std::string& path, TextureFlags flags = TextureFlags());
	};

}
