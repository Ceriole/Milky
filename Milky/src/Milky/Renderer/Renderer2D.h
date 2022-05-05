#pragma once

#include "Milky/Renderer/OrthographicCamera.h"
#include "Milky/Renderer/Texture.h"

namespace Milky {

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation = 0.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation = 0.0f);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint = { 1.0f, 1.0f, 1.0f, 1.0f }, float rotation = 0.0f, float textureScale = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint = { 1.0f, 1.0f, 1.0f, 1.0f }, float rotation = 0.0f, float textureScale = 1.0f);
	};

}
