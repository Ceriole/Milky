#include "EditorLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Milky/ImGui/ImGuiUtil.h"

namespace Milky {

	EditorLayer::EditorLayer()
		: Layer("MilkGlassEditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		ML_PROFILE_FUNCTION();

		FramebufferSpecification framebufferSpec;
		framebufferSpec.Width = 1280;
		framebufferSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(framebufferSpec);

		m_ActiveScene = CreateRef<Scene>();

		m_SquareEntity0 = m_ActiveScene->CreateEntity("Square 0");
		m_SquareEntity0.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		auto& transform0 = m_SquareEntity0.GetComponent<TransformComponent>();
		transform0.Translation.x = -1.0f;

		m_SquareEntity1 = m_ActiveScene->CreateEntity("Square 1");
		m_SquareEntity1.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

		auto& transform1 = m_SquareEntity1.GetComponent<TransformComponent>();
		transform1.Translation.x = 1.0f;

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity.AddComponent<CameraComponent>();

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate() override
			{}

			void OnDestroy() override
			{}

			void OnUpdate(Timestep ts) override
			{
				if (!(HasComponent<CameraComponent>() && HasComponent<TransformComponent>()))
					return;

				auto& camera = GetComponent<CameraComponent>();

				if (!camera.Primary)
					return;

				auto& transform = GetComponent<TransformComponent>();
				constexpr float speed = 5.0f;
				constexpr float rotationSpeed = glm::pi<float>() / 2.0f;

				switch (camera.Camera.GetProjectionType())
				{
				case SceneCamera::ProjectionType::Orthographic:
					if (Input::IsKeyPressed(Key::A))
						transform.Translation.x -= speed * ts;
					if (Input::IsKeyPressed(Key::D))
						transform.Translation.x += speed * ts;
					if (Input::IsKeyPressed(Key::W))
						transform.Translation.y += speed * ts;
					if (Input::IsKeyPressed(Key::S))
						transform.Translation.y -= speed * ts;
					break;
				case SceneCamera::ProjectionType::Perspective:
					if (Input::IsKeyPressed(Key::A))
					{
						transform.Translation.x -= speed * glm::cos(transform.Rotation.y) * ts;
						transform.Translation.z += speed * glm::sin(transform.Rotation.y) * ts;
					}
					if (Input::IsKeyPressed(Key::D))
					{
						transform.Translation.x += speed * glm::cos(transform.Rotation.y) * ts;
						transform.Translation.z -= speed * glm::sin(transform.Rotation.y) * ts;
					}
					if (Input::IsKeyPressed(Key::W))
					{
						transform.Translation.z -= speed * glm::cos(transform.Rotation.y) * ts;
						transform.Translation.x -= speed * glm::sin(transform.Rotation.y) * ts;
					}
					if (Input::IsKeyPressed(Key::S))
					{
						transform.Translation.z += speed * glm::cos(transform.Rotation.y) * ts;
						transform.Translation.x += speed * glm::sin(transform.Rotation.y) * ts;
					}
					if (Input::IsKeyPressed(Key::Q))
						transform.Rotation.y += rotationSpeed * ts;
					if (Input::IsKeyPressed(Key::E))
						transform.Rotation.y -= rotationSpeed * ts;
					break;
				}
			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		ML_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		ML_PROFILE_FUNCTION();

		if (Milky::FramebufferSpecification spec = m_Framebuffer->GetSpecification(); m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t) m_ViewportSize.x, (uint32_t) m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		// Update scene
		m_ActiveScene->OnUpdate(ts);

		m_Framebuffer->Unbind();
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
		ImGui::Begin("Editor DockSpace", &dockingEnabled, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspace_id;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			dockspace_id = ImGui::GetID("EditorDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit", NULL, false)) Application::Get().Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Options"))
			{
				GuiThemeManager::ShowThemeMenu();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImguiRender();

		if (ImGui::Begin("Settings"))
		{
			auto stats = Renderer2D::GetStats();
			if (ImGui::TreeNodeEx("Renderer2D Stats", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::BeginTable("renderer2Dstats", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
				{
					ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("Draw Calls");
					ImGui::TableNextColumn();
					ImGui::Text("%d", stats.DrawCalls);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("Quads");
					ImGui::TableNextColumn();
					ImGui::Text("%d", stats.QuadCount);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("Vertices");
					ImGui::TableNextColumn();
					ImGui::Text("%d", stats.GetTotalVertexCount());
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("Indices");
					ImGui::TableNextColumn();
					ImGui::Text("%d", stats.GetTotalIndexCount());
					ImGui::EndTable();
					ImGui::PopStyleColor();
				}
				ImGui::TreePop();
			}

			ImGui::End();
		} // Settings window end

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		if (ImGui::Begin("Viewport"))
		{
			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused || !m_ViewportHovered);
			
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint32_t textureId = m_Framebuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)(uint64_t)textureId, viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::End();
		}
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& event)
	{}

}
