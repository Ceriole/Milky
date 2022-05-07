#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// _ WATER
// . DIRT
static const uint32_t s_MapWidth = 24;
static const char* s_MapTiles =
"________________________"
"________________________"
"______..._______________"
"______....._____________"
"________......__________"
"_______..,,,,...__._____"
"______...,,,,,..__..____"
"______.,,....,,._...____"
"______...__..,,..,,..___"
"_____...____..,,,,,..___"
"____...______..,,,..____"
"___....._____.,,,...____"
"__...,,,,..,,,....______"
"_____,,,,,,,,.._________"
"__..______,,..___.._____"
"__..,,,,__,,...__.._____"
"___..,,,_,,,,..___._____"
"____..,,,,,,,,..__..____"
"_____....,,,...__...____"
"________....____..._____"
"______________....______"
"___________.....________"
"________________________"
"________________________"
;

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f), m_ParticleSystem(10000)
{
}

void Sandbox2D::OnAttach()
{
	ML_PROFILE_FUNCTION();

	m_CheckerTexture = Milky::Texture2D::Create("assets/textures/checkerboard.png");
	m_SpriteSheet = Milky::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png", {Milky::Texture2D::FilterType::NEAREST, Milky::Texture2D::FilterType::NEAREST });

	m_TextureStairs = Milky::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7, 6 }, { 128, 128 });
	m_TextureBarrel = Milky::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 8, 2 }, { 128, 128 });
	m_TextureTree = Milky::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 5, 1 }, { 128, 128 }, { 1, 2 });

	m_MapWidth = s_MapWidth;
	m_MapHeight = (uint32_t) strlen(s_MapTiles) / s_MapWidth;

	m_TextureMap['_'] = Milky::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11, 11 }, { 128, 128 });	// Water
	m_TextureMap['.'] = Milky::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6, 11 }, { 128, 128 });	// Dirt
	m_TextureMap[','] = Milky::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1, 11 }, { 128, 128 });	// Grass

	// Define particle properties
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 1.5f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 3.0f, 0.0f };
	m_Particle.Position = { 0.0f, 0.0f, 0.0f };

	m_CameraController.SetZoomLevel(5.0f);
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

#if 0
		static float rot = 0.0f;
		rot += ts * 90.0f;
		float size = 1.0f + 0.5f * glm::sin(glm::radians(rot));
		glm::vec4 inverseColor = { 1 - m_SquareColor.r, 1 - m_SquareColor.g, 1 - m_SquareColor.b, m_SquareColor.a };

		Milky::Renderer2D::BeginScene(m_CameraController.GetCamera()); // Lights, cameras, action!
		Milky::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, m_SquareColor);
		Milky::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.65f }, inverseColor);
		Milky::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerTexture, glm::vec4(1.0f), 20.0f);
		Milky::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f }, { 0.05f, 0.05f }, 45.0f, {1,0,0,1});
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
#endif
		if (Milky::Input::IsMouseButtonPressed(ML_MOUSE_BUTTON_LEFT))
		{
			auto [x, y] = Milky::Input::GetMousePosition();
			auto width = Milky::Application::Get().GetWindow().GetWidth();
			auto height = Milky::Application::Get().GetWindow().GetHeight();

			auto bounds = m_CameraController.GetBounds();
			auto pos = m_CameraController.GetCamera().GetPosition();
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
			m_Particle.Position = { x + pos.x, y + pos.y, pos.z };
			for (int i = 0; i < 5; i++)
				m_ParticleSystem.Emit(m_Particle);
		}

		m_ParticleSystem.OnUpdate(ts);
		m_ParticleSystem.OnRender(m_CameraController.GetCamera());

		Milky::Renderer2D::BeginScene(m_CameraController.GetCamera()); // Lights, cameras, action!

		for (uint32_t y = 0; y < m_MapHeight; y++)
		{
			for (uint32_t x = 0; x < m_MapWidth; x++)
			{
				char tileType = s_MapTiles[x + y * m_MapWidth];
				Milky::Ref<Milky::SubTexture2D> tileTexture = m_TextureBarrel;
				if (m_TextureMap.find(tileType) != m_TextureMap.end())
					tileTexture = m_TextureMap[tileType];
				Milky::Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f, m_MapHeight - y - m_MapHeight / 2.0f }, { 1.0f, 1.0f }, tileTexture);
			}
		}

		Milky::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.5f }, { 1.0f, 1.0f }, m_TextureStairs);
		Milky::Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.5f }, { 1.0f, 1.0f }, m_TextureBarrel);
		Milky::Renderer2D::DrawQuad({ -1.0f, 0.5f, 0.5f }, { 1.0f, 2.0f }, m_TextureTree);
		Milky::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	ML_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = Milky::Renderer2D::GetStats();
	
	ImGui::Text("Renderer2D Stats:");
	ImGui::Indent();
		ImGui::BulletText("Draw Calls: %d", stats.DrawCalls);
		ImGui::BulletText("Quads: %d", stats.QuadCount);
		ImGui::BulletText("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::BulletText("Indices: %d", stats.GetTotalIndexCount());
	ImGui::Unindent();

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::TextDisabled("Use [W],[A],[S],[D] to pan the camera.");
	ImGui::TextDisabled("Use [LMB] to emit particles at the mouse position.");
	if (ImGui::Button("Reset camera"))
	{
		m_CameraController.SetPosition({ 0,0,0 });
		m_CameraController.SetRotation(0);
		m_CameraController.SetZoomLevel(1.0f);
	}
	ImGui::End();

	ImGui::Begin("Particle");
	ImGui::ColorEdit4("Begin Color", glm::value_ptr(m_Particle.ColorBegin));
	ImGui::ColorEdit4("End Color", glm::value_ptr(m_Particle.ColorEnd));
	ImGui::DragFloat("Start Size", &m_Particle.SizeBegin, 0.01f, 0.0f);
	ImGui::DragFloat("End Size", &m_Particle.SizeEnd, 0.01f, 0.0f);
	ImGui::DragFloat("Variation", &m_Particle.SizeVariation, 0.01f, 0.0f);
	ImGui::DragFloat2("Velocity", glm::value_ptr(m_Particle.Velocity));
	ImGui::DragFloat2("Velocity Variation", glm::value_ptr(m_Particle.VelocityVariation));
	ImGui::End();
}

void Sandbox2D::OnEvent(Milky::Event& event)
{
	m_CameraController.OnEvent(event);
}
