#include "EditorLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Milky {

	EditorLayer::EditorLayer()
		: Layer("MilkGlassEditorLayer"), m_CameraController(1280.0f / 720.0f)
	{
	}

	void EditorLayer::OnAttach()
	{
		ML_PROFILE_FUNCTION();

		m_CheckerTexture = Milky::Texture2D::Create("assets/textures/checkerboard.png");

		Milky::FramebufferSpecification framebufferSpec;
		framebufferSpec.Width = 1280;
		framebufferSpec.Height = 720;
		m_Framebuffer = Milky::Framebuffer::Create(framebufferSpec);
	}

	void EditorLayer::OnDetach()
	{
		ML_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(Milky::Timestep ts)
	{
		ML_PROFILE_FUNCTION();

		m_CameraController.OnUpdate(ts);

		Milky::Renderer2D::ResetStats();
		{
			ML_PROFILE_SCOPE("Render Prep");
			m_Framebuffer->Bind();
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
			Milky::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f }, { 0.05f, 0.05f }, glm::radians(45.0f), { 1,0,0,1 });
			Milky::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f }, glm::vec2(size), glm::radians(rot), m_CheckerTexture, { 1,0,1,1 }, 2.0f);
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
			m_Framebuffer->Unbind();
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		ML_PROFILE_FUNCTION();
		static bool dockingEnabled = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockingEnabled, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{

				if (ImGui::MenuItem("Exit", NULL, false)) Milky::Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if(m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
		}
		uint32_t textureId = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureId, viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Milky::Event& event)
	{
		m_CameraController.OnEvent(event);
	}

}
