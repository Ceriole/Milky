#include "EditorLayer.h"

#include <imgui/imgui_internal.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Milky/ImGui/ImGuiUtil.h"

#include "Milky/Scene/SceneSerializer.h"

#include "Milky/Utils/PlatformUtils.h"

#include "Milky/Math/Math.h"

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

		m_RecentPaths.reserve(2);

		NewScene();
#if 0
		// Generate default scene
		SetActiveFilepath("assets/scenes/Default.milky");

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

		SaveScene();
#endif
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
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Editor Dockspace", NULL, window_flags);
		ImGui::PopStyleVar(3);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 380.0f;
		m_DockspaceID = ImGui::GetID("EditorDockspace");
		if (ImGui::DockBuilderGetNode(m_DockspaceID) == NULL)
			SetEditorDefaultDockLayout();
		ImGui::DockSpace(m_DockspaceID, ImVec2(0.0f, 0.0f), dockspace_flags);
		style.WindowMinSize.x = minWinSizeX;

		ShowEditorMenuBar();

		m_ScenePanels.OnImguiRender();

		ShowEditorViewport();
		ShowEditorSettings();

		ImGui::End();
	}

	void EditorLayer::SetEditorDefaultDockLayout()
	{
		m_ScenePanels.SetShown(true);
		m_ShowViewport = true, m_ShowStats = false;

		ImGui::DockBuilderRemoveNode(m_DockspaceID);
		ImGui::DockBuilderAddNode(m_DockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(m_DockspaceID, ImGui::GetWindowSize());

		ImGuiID dock_main_id = m_DockspaceID;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.30f, NULL, &dock_main_id);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.40f, NULL, &dock_main_id);

		ImGuiID dock_id_properties = NULL;
		ImGuiID dock_id_scene_hierarchy = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Up, 0.60f, NULL, &dock_id_properties);

		ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_id_scene_hierarchy);
		ImGui::DockBuilderDockWindow("Properties", dock_id_properties);
		ImGui::DockBuilderDockWindow("Viewport", dock_main_id);
		ImGui::DockBuilderDockWindow("Stats", dock_id_right);
		ImGui::DockBuilderFinish(m_DockspaceID);
	}

	void EditorLayer::ShowEditorMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N", false)) NewScene();
				if (ImGui::MenuItem("Open...", "Ctrl+O", false)) OpenSceneDialog();
				if (ImGui::MenuItem("Save", "Ctrl+S", false)) SaveScene();
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S", false)) SaveSceneDialog();
				ImGui::Separator();
				ShowRecentFilesMenu();
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "Ctrl+Escape", false)) Application::Get().Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene"))
			{
				m_ScenePanels.ShowNewEntityMenu();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				m_ScenePanels.ShowWindowMenuItems();
				ImGui::MenuItem("Viewport", "Ctrl+Shift+P", &m_ShowViewport);
				ImGui::MenuItem("Stats", NULL, &m_ShowStats);
				ImGui::Separator();
				if (ImGui::MenuItem("Reset Layout", "Ctrl+Shift+R", false))
					ImGui::DockBuilderRemoveNode(m_DockspaceID);
				ImGui::Separator();
				GuiThemeManager::ShowThemeMenu();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::ShowEditorViewport()
	{
		if (m_ShowViewport)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			if (ImGui::Begin("Viewport", &m_ShowViewport))
			{
				m_ViewportFocused = ImGui::IsWindowFocused();
				m_ViewportHovered = ImGui::IsWindowHovered();
				Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused || !m_ViewportHovered);

				ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
				m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

				uint32_t textureId = m_Framebuffer->GetColorAttachmentRendererID();
				ImGui::Image((void*)(uint64_t)textureId, viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			}
			
			// Gizmos
			Entity selectedEntity = m_ScenePanels.GetSelectedEntity(); // TODO: change
			if (selectedEntity && m_GizmoType >= 0)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				
				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

				// Camera
				auto camEntity = m_ActiveScene->GetPrimaryCameraEntity();
				if (camEntity)
				{
					const auto& cc = camEntity.GetComponent<CameraComponent>();
					const glm::mat4& cameraProjection = cc.Camera.GetProjection();
					glm::mat4 cameraView = glm::inverse(camEntity.GetComponent<TransformComponent>().GetTransform());

					// Entity Transform
					auto& tc = selectedEntity.GetComponent<TransformComponent>();
					glm::mat4 transform = tc.GetTransform();
					
					// Snapping
					bool snap = Input::IsKeyPressed(Key::LeftControl); // TODO: Snap setting? Set snap values in UI
					float snapValue = 0.5f; // Snap to 0.5m for translation and scale
					if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
						snapValue = 45.0f; // Snap to 45 degrees for rotation

					float snapValues[3] = { snapValue, snapValue, snapValue };

					// Manupulate
					if (ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_GizmoType, m_GizmoMode, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr))
					{
						glm::vec3 translation, rotation, scale;
						Math::DecomposeTransform(transform, translation, rotation, scale);

						glm::vec3 deltaRotation = rotation - tc.Rotation;
						tc.Translation = translation;
						tc.Rotation += deltaRotation;
						tc.Scale = scale;
					}
				}
			}

			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	void EditorLayer::ShowEditorSettings()
	{
		if (m_ShowStats)
		{
			if (ImGui::Begin("Stats", &m_ShowStats))
			{
				auto stats = Renderer2D::GetStats();
				if (ImGui::TreeNodeEx("Renderer2D", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
					if (ImGui::BeginTable("renderer2DStats", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
					{
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
					}
					ImGui::PopStyleColor();
					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
					if (ImGui::BeginTable("sceneStats", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
					{
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("Entity Count");
						ImGui::TableNextColumn();
						ImGui::Text("%d", m_ActiveScene->GetNumEntites());
						ImGui::EndTable();
					}
					ImGui::PopStyleColor();
					ImGui::TreePop();
				}
			}
			ImGui::End();
		}
	}

	void EditorLayer::ShowRecentFilesMenu()
	{
		if (ImGui::BeginMenu("Open Recent", !m_RecentPaths.empty()))
		{
			for (int i = 0; i < m_RecentPaths.size(); i++)
			{
				std::string recentFileTitle = std::to_string(i) + " " + m_RecentPaths.at(i);
				if (ImGui::MenuItem(recentFileTitle.c_str()))
					OpenScene(m_RecentPaths.at(i));
			}
			ImGui::EndMenu();
		}
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_ScenePanels.SetContext(m_ActiveScene);
		SetActiveFilepath(std::string());
	}

	void EditorLayer::OpenScene(std::string filepath)
	{
		if (!filepath.empty())
		{
			Ref<Scene> oldScene = m_ActiveScene;
			NewScene();
			SceneSerializer serializer(m_ActiveScene);
			if (serializer.Deserialize(filepath))
				SetActiveFilepath(filepath);
			else
				m_ActiveScene = oldScene;
		}
	}

	void EditorLayer::SaveScene(std::string filepath)
	{
		SceneSerializer serializer(m_ActiveScene);
		if (!filepath.empty())
		{
			serializer.Serialize(filepath);
			SetActiveFilepath(filepath);
		}
		else if(!m_ActivePath.empty())
		{
			serializer.Serialize(m_ActivePath);
			SetActiveFilepath(m_ActivePath);
		}
		else
		{
			std::string newFilepath = FileDialogs::Save("Milky Scene (*.milky)\0*.milky\0");
			serializer.Serialize(newFilepath);
			SetActiveFilepath(newFilepath);
		}
	}

	void EditorLayer::OpenSceneDialog()
	{
		std::string filepath = FileDialogs::Open("Milky Scene (*.milky)\0*.milky\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::SaveSceneDialog()
	{
		std::string filepath = FileDialogs::Save("Milky Scene (*.milky)\0*.milky\0");
		if (!filepath.empty())
			SaveScene(filepath);
	}

	void EditorLayer::SetActiveFilepath(std::string filepath)
	{
		if (!m_ActivePath.empty())
		{
			m_RecentPaths.erase(std::remove(m_RecentPaths.begin(), m_RecentPaths.end(), m_ActivePath), m_RecentPaths.end());
			m_RecentPaths.insert(m_RecentPaths.begin(), m_ActivePath);
		}
		m_ActivePath = filepath;
		if (!m_ActivePath.empty())
		{
			Application::Get().GetWindow().SetTitle("MilkGlass - " + m_ActivePath);
		}
		else
			Application::Get().GetWindow().SetTitle("MilkGlass - Unsaved Scene");
	}


	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(ML_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		const bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		const bool ctrl = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);


		switch (e.GetKeyCode())
		{
			// File
		case Key::S:
			if (ctrl)
				if (shift)
					SaveSceneDialog();
				else
					SaveScene();
			break;
		case Key::O:
			if (ctrl)
				OpenSceneDialog();
			break;
		case Key::N:
			if (ctrl)
				NewScene();
			break;
			// Gizmos
		case Key::Q:
			m_GizmoType = -1;
			break;
		case Key::W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Key::E:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case Key::R:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}

		return false;
	}

}
