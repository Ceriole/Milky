#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	m_CheckerTexture = Milky::Texture2D::Create("assets/textures/checkerboard.png");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Milky::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Milky::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Milky::RenderCommand::Clear();

	Milky::Renderer2D::BeginScene(m_CameraController.GetCamera()); // Lights, cameras, action!
	
	Milky::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, m_SquareColor);
	glm::vec4 inverseColor = {1 - m_SquareColor.r, 1 - m_SquareColor.g, 1 - m_SquareColor.b, m_SquareColor.a};
	Milky::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, inverseColor, 45.0f);

	Milky::Renderer2D::DrawQuad({ 0.2f, 0.5f }, { 0.5f, 0.75f }, m_CheckerTexture, m_SquareColor);

	Milky::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 0.05f, 0.05f }, {1,0,0,1}, 45.0f);
	Milky::Renderer2D::EndScene(); // And cut!
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::TextDisabled("Use [LMB] + mouse OR\n[W],[A],[S],[D] to pan the camera.");
	if (ImGui::Button("Reset camera"))
	{
		m_CameraController.SetPosition({ 0,0,0 });
		m_CameraController.SetRotation(0);
		m_CameraController.SetZoomLevel(1.0f);
	}
	ImGui::End();
}

void Sandbox2D::OnEvent(Milky::Event& event)
{
	m_CameraController.OnEvent(event);
}
