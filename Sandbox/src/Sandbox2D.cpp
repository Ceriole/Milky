#include "Sandbox2D.h"

#include <Platform/OpenGL/OpenGLShader.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	m_SquareVA = Milky::VertexArray::Create();
	float squareVertices[4 * 3] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	Milky::Ref<Milky::VertexBuffer> squareVB;
	squareVB = Milky::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout({
			{ Milky::ShaderDataType::Float3, "a_Position" }
		});
	m_SquareVA->AddVertexBuffer(squareVB);

	unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Milky::Ref<Milky::IndexBuffer> squareIB;
	squareIB = Milky::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);

	m_FlatColorShader = Milky::Shader::Create("assets/shaders/FlatColor.glsl");
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(Milky::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Milky::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Milky::RenderCommand::Clear();

	Milky::Renderer::BeginScene(m_CameraController.GetCamera()); // Lights, cameras, action!

	std::dynamic_pointer_cast<Milky::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Milky::OpenGLShader>(m_FlatColorShader)->UploadUniform("u_Color", m_SquareColor);

	m_FlatColorShader->Bind();
	Milky::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	Milky::Renderer::EndScene(); // And cut!
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Milky::Event& event)
{
	m_CameraController.OnEvent(event);
}
