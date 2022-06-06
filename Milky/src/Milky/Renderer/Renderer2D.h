#pragma once

#include "Milky/Renderer/OrthographicCamera.h"

#include "Milky/Renderer/Texture.h"
#include "Milky/Renderer/SubTexture2D.h"

#include "Milky/Renderer/Camera.h"
#include "Milky/Renderer/EditorCamera.h"

#include "Milky/Scene/Components/ComponentsVisual.h"

namespace Milky {

	namespace Renderer2DConstants {
		constexpr uint32_t MaxQuads = 5000; // Per single draw call
		constexpr uint32_t MaxQuadVertices = MaxQuads * 4;
		constexpr uint32_t MaxQuadIndices = MaxQuads * 6;

		constexpr uint32_t MaxCircles = 1000; // Per single draw call
		constexpr uint32_t MaxCircleVertices = MaxCircles * 4;
		constexpr uint32_t MaxCircleIndices = MaxCircles * 6;

		constexpr uint32_t MaxLines = 5000; // Per single draw call
		constexpr uint32_t MaxLineVertices = MaxLines * 2;

		constexpr uint32_t MaxQuadLines = 5000; // Per single draw call
		constexpr uint32_t MaxQuadLineVertices = MaxQuadLines * 4;

		constexpr uint32_t CircleResolution = 16;
		constexpr uint32_t MaxCircleLines = 800; // Per single draw call
		constexpr uint32_t MaxCircleLineVertices = MaxCircleLines * CircleResolution;

		constexpr uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
	}

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();
		static void FlushQuads();
		static void FlushCircles();
		static void FlushLines();
		static void FlushQuadLines();
		static void FlushCircleLines();

		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint = glm::vec4(1.0f), float tilingFactor = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint = glm::vec4(1.0f), float tilingFactor = 1.0f);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, const glm::vec4& tint = glm::vec4(1.0f), float tilingFactor = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, const glm::vec4& tint = glm::vec4(1.0f), float tilingFactor = 1.0f);

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tint, float tilingFactor = 1.0f, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subTexture, const glm::vec4& tint, float tilingFactor = 1.0f, int entityID = -1);

		// Rotation is in radians
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& tint = glm::vec4(1.0f), float tilingFactor = 1.0f);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& tint = glm::vec4(1.0f), float tilingFactor = 1.0f);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, const glm::vec4& tint = glm::vec4(1.0f), float tilingFactor = 1.0f);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, const glm::vec4& tint = glm::vec4(1.0f), float tilingFactor = 1.0f);

		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.0f, int entityID = -1);

		static void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, int entityID = -1);
		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID = -1);

		static void DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		static void DrawLineCircle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawLineCircle(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawLineCircle(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		static float GetLineWidth();
		static void SetLineWidth(float width);

		// Entities
		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& sprite, int entityID);
		static void DrawCircle(const glm::mat4& transform, CircleRendererComponent& circle, int entityID);

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t CircleCount = 0;
			uint32_t LineCount = 0;
			uint32_t QuadLineCount = 0;
			uint32_t CircleLineCount = 0;

			uint32_t GetQuadVertexCount() const { return QuadCount * 4; }
			uint32_t GetQuadIndexCount() const { return QuadCount * 6; }

			uint32_t GetCircleVertexCount() const { return CircleCount * 4; }
			uint32_t GetCircleIndexCount() const { return CircleCount * 6; }

			uint32_t GetLineVertexCount() const { return LineCount * 2 + QuadLineCount * 4 + CircleLineCount * Renderer2DConstants::CircleResolution; }

			uint32_t GetTotalVertexCount() const { return GetQuadVertexCount() + GetCircleVertexCount() + GetLineVertexCount(); }
			uint32_t GetTotalIndexCount() const { return GetQuadIndexCount() + GetCircleIndexCount(); }
		};

		static void ResetStats();
		static Statistics GetStats();
	private:
		static void StartBatchQuads();
		static void NextBatchQuads();

		static void StartBatchCircles();
		static void NextBatchCircles();

		static void StartBatchLines();
		static void NextBatchLines();
		static void StartBatchQuadLines();
		static void NextBatchQuadLines();
		static void StartBatchCircleLines();
		static void NextBatchCircleLines();
	};

}
