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

	Milky::Renderer2D::ResetStats();
	{
		ML_PROFILE_SCOPE("Render Prep");
		Milky::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Milky::RenderCommand::Clear();
	}

	{
		ML_PROFILE_SCOPE("Render Draw");

		static float rot = 0.0f;
		rot += ts * 90.0f;
		float size = 1.0f + 0.5f * glm::sin(glm::radians(rot));
		glm::vec4 inverseColor = { 1 - m_SquareColor.r, 1 - m_SquareColor.g, 1 - m_SquareColor.b, m_SquareColor.a };

		Milky::Renderer2D::BeginScene(m_CameraController.GetCamera()); // Lights, cameras, action!
		Milky::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, m_SquareColor);
		Milky::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.65f }, inverseColor);
		Milky::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerTexture, glm::vec4(1.0f), 20.0f);
		Milky::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f }, { 0.05f, 0.05f }, glm::radians(45.0f), {1,0,0,1});
		Milky::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f }, glm::vec2(size), glm::radians(rot), m_CheckerTexture, {1,0,1,1}, 2.0f);
		Milky::Renderer2D::EndScene(); // And cut!
		
		Milky::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y <= 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x <= 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.75f };
				Milky::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		Milky::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	ML_PROFILE_FUNCTION();
	static bool dockingEnabled = true;
	
	ImGui::Begin("Settings"); {
		auto stats = Milky::Renderer2D::GetStats();
	
		ImGui::Text("Renderer2D Stats:");
		ImGui::Indent();
			ImGui::BulletText("Draw Calls: %d", stats.DrawCalls);
			ImGui::BulletText("Quads: %d", stats.QuadCount);
			ImGui::BulletText("Vertices: %d", stats.GetTotalVertexCount());
			ImGui::BulletText("Indices: %d", stats.GetTotalIndexCount());
		ImGui::Unindent();

		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::TextDisabled("Use [W],[A],[S],[D] OR [MMB] + mouse to pan the camera.");
		if (ImGui::Button("Reset camera"))
		{
			m_CameraController.SetPosition({ 0,0,0 });
			m_CameraController.SetRotation(0);
			m_CameraController.SetZoomLevel(1.0f);
		}
	} ImGui::End();
}

void Sandbox2D::OnEvent(Milky::Event& event)
{
	m_CameraController.OnEvent(event);
}
