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
	ML_PROFILE_FUNCTION();

	m_CheckerTexture = Milky::Texture2D::Create("assets/textures/checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	ML_PROFILE_FUNCTION();

}

void Sandbox2D::OnUpdate(Milky::Timestep ts)
{
	ML_PROFILE_FUNCTION();

	m_CameraController.OnUpdate(ts);

	{
		ML_PROFILE_SCOPE("Render Prep");
		Milky::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Milky::RenderCommand::Clear();
	}

	Milky::Renderer2D::BeginScene(m_CameraController.GetCamera()); // Lights, cameras, action!
	
	{
		ML_PROFILE_SCOPE("Render Draw");
		Milky::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, m_SquareColor);
		glm::vec4 inverseColor = { 1 - m_SquareColor.r, 1 - m_SquareColor.g, 1 - m_SquareColor.b, m_SquareColor.a };
		Milky::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, inverseColor);
		Milky::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerTexture, glm::vec4(1.0f), 10.0f);

		Milky::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f }, { 0.05f, 0.05f }, 45.0f, {1,0,0,1});

		/*for (int x = 0; x < 30; x++)
		{
			for (int y = 0; y < 30; y++)
			{
				Milky::Renderer2D::DrawQuad({ 0.5f + x * 0.1f , 0.5f + y * 0.1f }, { 0.08f, 0.08f }, (x % 2 || y % 2) ? m_SquareColor : inverseColor);
			}
		}*/
		Milky::Renderer2D::EndScene(); // And cut!
	}
}

void Sandbox2D::OnImGuiRender()
{
	ML_PROFILE_FUNCTION();

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
