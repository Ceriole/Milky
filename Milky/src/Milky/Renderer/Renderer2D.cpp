#include "mlpch.h"
#include "Renderer2D.h"

#include "Milky/Renderer/VertexArray.h"
#include "Milky/Renderer/Shader.h"
#include "Milky/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Milky {

	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> FlatColorShader;
		Ref<Shader> TextureShader;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();
		s_Data->QuadVertexArray = VertexArray::Create();

		float squareVertices[4 * 5] = {
			-0.5f, -0.5f, 0.0f, 0, 0,
			 0.5f, -0.5f, 0.0f, 1, 0,
			 0.5f,  0.5f, 0.0f, 1, 1,
			-0.5f,  0.5f, 0.0f, 0, 1
		};

		Ref<VertexBuffer> squareVB;
		squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
			});
		s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

		unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB;
		squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

		s_Data->FlatColorShader = Shader::Create("assets/shaders/FlatColor.glsl");

		s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data->TextureShader->Set("u_Texture", 0);
		s_Data->TextureShader->Set("u_Color", { 1, 1, 1, 1 });
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data->FlatColorShader->Bind();
		s_Data->FlatColorShader->Set("u_ViewProjection", camera.GetViewProjectionMatrix());
		s_Data->FlatColorShader->Set("u_Transform", glm::mat4(1.0f));
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->Set("u_ViewProjection", camera.GetViewProjectionMatrix());
		s_Data->TextureShader->Set("u_Transform", glm::mat4(1.0f));
	}

	void Renderer2D::EndScene()
	{
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, color, rotation);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation)
	{
		s_Data->FlatColorShader->Bind();
		s_Data->FlatColorShader->Set("u_Color", color);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0, 0, 1 })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->FlatColorShader->Set("u_Transform", transform);
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint, float rotation)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tint, rotation);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tint, float rotation)
	{
		s_Data->TextureShader->Bind();

		s_Data->TextureShader->Set("u_Color", tint);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0, 0, 1 }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->Set("u_Transform", transform);

		texture->Bind();

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

}
