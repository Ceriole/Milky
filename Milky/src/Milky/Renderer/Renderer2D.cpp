#include "mlpch.h"
#include "Renderer2D.h"

#include "Milky/Renderer/VertexArray.h"
#include "Milky/Renderer/Shader.h"
#include "Milky/Renderer/UniformBuffer.h"
#include "Milky/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Milky {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		// Editor-only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 Position;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// Editor-only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor-only
		int EntityID;
	};

	struct Renderer2DData
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;
		
		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<VertexArray> QuadLineVertexArray;
		Ref<VertexBuffer> QuadLineVertexBuffer;
		Ref<VertexArray> CircleLineVertexArray;
		Ref<VertexBuffer> CircleLineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;
		uint32_t QuadLineCount = 0;
		LineVertex* QuadLineVertexBufferBase = nullptr;
		LineVertex* QuadLineVertexBufferPtr = nullptr;
		uint32_t CircleLineCount = 0;
		LineVertex* CircleLineVertexBufferBase = nullptr;
		LineVertex* CircleLineVertexBufferPtr = nullptr;

		float LineWidth = 2.0f;

		Ref<Texture2D> BlankTexutre;
		std::array<Ref<Texture2D>, Renderer2DConstants::MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // Slot 0 = 1x1 white texture

		glm::vec4 QuadVertexPositions[4];
		glm::vec4 CircleVertexPositions[Renderer2DConstants::CircleResolution];

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		ML_PROFILE_FUNCTION();

		// Quads
		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DConstants::MaxQuadVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Position"	},
			{ ShaderDataType::Float4,	"a_Color"		},
			{ ShaderDataType::Float2,	"a_TexCoord"	},
			{ ShaderDataType::Float,	"a_TexIndex"	},
			{ ShaderDataType::Float,	"a_TilingFactor"},
			{ ShaderDataType::Int,		"a_EntityID"	}
		});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DConstants::MaxQuadVertices];

		uint32_t* quadIndices = new uint32_t[Renderer2DConstants::MaxQuadIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < Renderer2DConstants::MaxQuadIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, Renderer2DConstants::MaxQuadIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Circles
		s_Data.CircleVertexArray = VertexArray::Create();

		s_Data.CircleVertexBuffer = VertexBuffer::Create(Renderer2DConstants::MaxCircleVertices * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Position"		},
			{ ShaderDataType::Float3,	"a_LocalPosition"	},
			{ ShaderDataType::Float4,	"a_Color"			},
			{ ShaderDataType::Float,	"a_Thickness"		},
			{ ShaderDataType::Float,	"a_Fade"			},
			{ ShaderDataType::Int,		"a_EntityID"		}
			});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.CircleVertexBufferBase = new CircleVertex[Renderer2DConstants::MaxCircleVertices];
		
		uint32_t* circleIndices = new uint32_t[Renderer2DConstants::MaxCircleIndices];

		offset = 0;
		for (uint32_t i = 0; i < Renderer2DConstants::MaxCircleIndices; i += 6)
		{
			circleIndices[i + 0] = offset + 0;
			circleIndices[i + 1] = offset + 1;
			circleIndices[i + 2] = offset + 2;
								   
			circleIndices[i + 3] = offset + 2;
			circleIndices[i + 4] = offset + 3;
			circleIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> circleIB = IndexBuffer::Create(circleIndices, Renderer2DConstants::MaxCircleIndices);
		s_Data.CircleVertexArray->SetIndexBuffer(circleIB);
		delete[] circleIndices;

		// Lines
		s_Data.LineVertexArray = VertexArray::Create();

		s_Data.LineVertexBuffer = VertexBuffer::Create(Renderer2DConstants::MaxLineVertices * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Position"},
			{ ShaderDataType::Float4,	"a_Color"	},
			{ ShaderDataType::Int,		"a_EntityID"}
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
		s_Data.LineVertexBufferBase = new LineVertex[Renderer2DConstants::MaxLineVertices];

		// Quad Lines
		s_Data.QuadLineVertexArray = VertexArray::Create();

		s_Data.QuadLineVertexBuffer = VertexBuffer::Create(Renderer2DConstants::MaxQuadLineVertices * sizeof(LineVertex));
		s_Data.QuadLineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Position"},
			{ ShaderDataType::Float4,	"a_Color"	},
			{ ShaderDataType::Int,		"a_EntityID"}
			});
		s_Data.QuadLineVertexArray->AddVertexBuffer(s_Data.QuadLineVertexBuffer);
		s_Data.QuadLineVertexBufferBase = new LineVertex[Renderer2DConstants::MaxQuadLineVertices];

		// Lines
		s_Data.CircleLineVertexArray = VertexArray::Create();

		s_Data.CircleLineVertexBuffer = VertexBuffer::Create(Renderer2DConstants::MaxCircleLineVertices * sizeof(LineVertex));
		s_Data.CircleLineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Position"},
			{ ShaderDataType::Float4,	"a_Color"	},
			{ ShaderDataType::Int,		"a_EntityID"}
			});
		s_Data.CircleLineVertexArray->AddVertexBuffer(s_Data.CircleLineVertexBuffer);
		s_Data.CircleLineVertexBufferBase = new LineVertex[Renderer2DConstants::MaxCircleLineVertices];

		// Create blank texture
		s_Data.BlankTexutre = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.BlankTexutre->SetData(&whiteTextureData, sizeof(uint32_t));

		// Set sampler array for shaders [1, 2, 3 ... 31]
		int32_t samplers[Renderer2DConstants::MaxTextureSlots] = {};
		for (int32_t i = 0; i < Renderer2DConstants::MaxTextureSlots; i++)
			samplers[i] = i;

		// Load and compile shaders
		s_Data.QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");
		s_Data.CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");
		s_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");

		// Put blank texture in slot 0
		s_Data.TextureSlots[0] = s_Data.BlankTexutre;

		// Base vertices for rendering
		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		for (size_t i = 0; i < Renderer2DConstants::CircleResolution; i++)
		{
			float theta = 2.0f * 3.1415926f * float(i) / float(Renderer2DConstants::CircleResolution);//get the current angle
			s_Data.CircleVertexPositions[i] = {0.5f * cosf(theta), 0.5f * sinf(theta), 0.0f, 1.0f};
		}

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		ML_PROFILE_FUNCTION();

		delete[] s_Data.QuadVertexBufferBase;
		delete[] s_Data.CircleVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		ML_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraBuffer));

		StartBatchQuads();
		StartBatchCircles();
		StartBatchLines();
		StartBatchQuadLines();
		StartBatchCircleLines();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		ML_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraBuffer));

		StartBatchQuads();
		StartBatchCircles();
		StartBatchLines();
		StartBatchQuadLines();
		StartBatchCircleLines();
	}

	void Renderer2D::EndScene()
	{
		ML_PROFILE_FUNCTION();

		FlushQuads();
		FlushCircles();
		FlushLines();
		FlushQuadLines();
		FlushCircleLines();
	}

	// QUADS =====================================================================================

	void Renderer2D::StartBatchQuads()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::FlushQuads()
	{
		ML_PROFILE_FUNCTION();

		if (!s_Data.QuadIndexCount)
			return; // Nothing to draw

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);

		s_Data.QuadShader->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::NextBatchQuads()
	{
		FlushQuads();
		StartBatchQuads();
	}

	// CIRCLES =====================================================================================

	void Renderer2D::StartBatchCircles()
	{
		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;
	}

	void Renderer2D::FlushCircles()
	{
		ML_PROFILE_FUNCTION();

		if (!s_Data.CircleIndexCount)
			return; // Nothing to draw

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
		s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

		s_Data.CircleShader->Bind();
		RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::NextBatchCircles()
	{
		FlushCircles();
		StartBatchCircles();
	}

	// LINES =====================================================================================

	void Renderer2D::StartBatchLines()
	{
		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
	}

	void Renderer2D::FlushLines()
	{
		ML_PROFILE_FUNCTION();

		if (!s_Data.LineVertexCount)
			return; // Nothing to draw

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
		s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

		s_Data.LineShader->Bind();
		RenderCommand::SetLineWidth(s_Data.LineWidth);
		RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::NextBatchLines()
	{
		FlushLines();
		StartBatchLines();
	}

	// QUAD LINES ===============================================================================

	void Renderer2D::StartBatchQuadLines()
	{
		s_Data.QuadLineCount = 0;
		s_Data.QuadLineVertexBufferPtr = s_Data.QuadLineVertexBufferBase;
	}

	void Renderer2D::FlushQuadLines()
	{
		ML_PROFILE_FUNCTION();

		if (!s_Data.QuadLineCount)
			return; // Nothing to draw

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadLineVertexBufferPtr - (uint8_t*)s_Data.QuadLineVertexBufferBase);
		s_Data.QuadLineVertexBuffer->SetData(s_Data.QuadLineVertexBufferBase, dataSize);

		s_Data.LineShader->Bind();
		RenderCommand::SetLineWidth(s_Data.LineWidth);
		RenderCommand::DrawLineLoops(s_Data.QuadLineVertexArray, 4, s_Data.QuadLineCount);
		s_Data.Stats.DrawCalls += s_Data.QuadLineCount;
	}

	void Renderer2D::NextBatchQuadLines()
	{
		FlushQuadLines();
		StartBatchQuadLines();
	}

	// CIRCLE LINES ================================================================================

	void Renderer2D::StartBatchCircleLines()
	{
		s_Data.CircleLineCount = 0;
		s_Data.CircleLineVertexBufferPtr = s_Data.CircleLineVertexBufferBase;
	}

	void Renderer2D::FlushCircleLines()
	{
		ML_PROFILE_FUNCTION();

		if (!s_Data.CircleLineCount)
			return; // Nothing to draw

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleLineVertexBufferPtr - (uint8_t*)s_Data.CircleLineVertexBufferBase);
		s_Data.CircleLineVertexBuffer->SetData(s_Data.CircleLineVertexBufferBase, dataSize);

		s_Data.LineShader->Bind();
		RenderCommand::SetLineWidth(s_Data.LineWidth);
		RenderCommand::DrawLineLoops(s_Data.CircleLineVertexArray, Renderer2DConstants::CircleResolution, s_Data.CircleLineCount);
		s_Data.Stats.DrawCalls += s_Data.CircleLineCount;
	}

	void Renderer2D::NextBatchCircleLines()
	{
		FlushCircleLines();
		StartBatchCircleLines();
	}

	// DRAW FUNCTIONS =================================================================================

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		ML_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint, float tilingFactor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tint, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint, float tilingFactor)
	{
		ML_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tint, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, const glm::vec4& tint, float tilingFactor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, subTexture, tint, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, const glm::vec4& tint, float tilingFactor)
	{
		ML_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, subTexture, tint, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		DrawQuad(transform, s_Data.BlankTexutre, color, 1.0f, entityID);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tint, float tilingFactor, int entityID)
	{
		ML_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Data.QuadIndexCount >= Renderer2DConstants::MaxQuadIndices)
			NextBatchQuads();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DConstants::MaxTextureSlots)
				NextBatchQuads();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tint;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subTexture, const glm::vec4& tint, float tilingFactor, int entityID)
	{
		ML_PROFILE_FUNCTION();
		const glm::vec2* textureCoords = subTexture->GetTexCoords();
		const Ref<Texture2D> texture = subTexture->GetTexture();

		if (s_Data.QuadIndexCount >= Renderer2DConstants::MaxQuadIndices)
			NextBatchQuads();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DConstants::MaxTextureSlots)
				NextBatchQuads();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tint;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		ML_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& subTexture, const glm::vec4& tint, float tilingFactor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, subTexture, tint, tilingFactor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& subTexture, const glm::vec4& tint, float tilingFactor)
	{
		ML_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, subTexture, tint, tilingFactor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& texture, const glm::vec4& tint, float tilingFactor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tint, tilingFactor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& texture, const glm::vec4& tint, float tilingFactor)
	{
		ML_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tint, tilingFactor);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		ML_PROFILE_FUNCTION();

		if (s_Data.CircleIndexCount >= Renderer2DConstants::MaxCircleIndices)
			NextBatchCircles();

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.CircleVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = color;
			s_Data.CircleVertexBufferPtr->Thickness = thickness;
			s_Data.CircleVertexBufferPtr->Fade = fade;
			s_Data.CircleVertexBufferPtr->EntityID = entityID;
			s_Data.CircleVertexBufferPtr++;
		}
		s_Data.CircleIndexCount += 6;

		s_Data.Stats.CircleCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& sprite, int entityID)
	{
		if (sprite.Texture)
			DrawQuad(transform, sprite.Texture, sprite.Color, sprite.TilingFactor, entityID);
		else
			DrawQuad(transform, sprite.Color, entityID);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, CircleRendererComponent& circle, int entityID)
	{
		DrawCircle(transform, circle.Color, circle.Thickness, circle.Fade, entityID);
	}

	void Renderer2D::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, int entityID)
	{
		DrawLine({ p0.x, p0.y, 0.0f }, { p1.x, p1.y, 0.0f }, color, entityID);
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		ML_PROFILE_FUNCTION();

		if (s_Data.LineVertexCount >= Renderer2DConstants::MaxLineVertices)
			NextBatchLines();

		s_Data.LineVertexBufferPtr->Position = p0;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexBufferPtr->Position = p1;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;
		
		s_Data.LineVertexCount += 2;

		s_Data.Stats.LineCount++;
	}

	void Renderer2D::DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		DrawRect({ position.x, position.y, 0.0f }, size, color, entityID);
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		ML_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		
		DrawRect(transform, color, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		ML_PROFILE_FUNCTION();

		if (s_Data.QuadLineCount >= Renderer2DConstants::MaxQuadLines)
			NextBatchQuadLines();

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.QuadLineVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadLineVertexBufferPtr->Color = color;
			s_Data.QuadLineVertexBufferPtr->EntityID = entityID;
			s_Data.QuadLineVertexBufferPtr++;
		}
		s_Data.QuadLineCount++;

		s_Data.Stats.QuadLineCount++;
	}

	void Renderer2D::DrawLineCircle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		DrawLineCircle({ position.x, position.y, 0.0f }, size, color, entityID);
	}

	void Renderer2D::DrawLineCircle(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		ML_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawLineCircle(transform, color, entityID);
	}

	void Renderer2D::DrawLineCircle(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		ML_PROFILE_FUNCTION();

		if (s_Data.CircleLineCount >= Renderer2DConstants::MaxCircleLines)
			NextBatchCircleLines();

		for (size_t i = 0; i < Renderer2DConstants::CircleResolution; i++)
		{
			s_Data.CircleLineVertexBufferPtr->Position = transform * s_Data.CircleVertexPositions[i];
			s_Data.CircleLineVertexBufferPtr->Color = color;
			s_Data.CircleLineVertexBufferPtr->EntityID = entityID;
			s_Data.CircleLineVertexBufferPtr++;
		}

		s_Data.CircleLineCount++;
		s_Data.Stats.CircleLineCount++;
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_Data.LineWidth = width;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}
}
