#include "EditorLayer.h"

#include <imgui/imgui_internal.h>
#include <iconfonts/IconsFontAwesome5.h>

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

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			OpenScene(sceneFilePath);
		}
		else
			NewScene();

		m_ShowWelcome = true;
	}

	void EditorLayer::OnDetach()
	{
		ML_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdateRuntime(Timestep ts)
	{
		ML_PROFILE_FUNCTION();

		if (Milky::FramebufferSpecification spec = m_Framebuffer->GetSpecification(); m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t) m_ViewportSize.x, (uint32_t) m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		m_EditorCamera.OnUpdate(ts);

		// Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		// Clear entity ID attachment to -1
		m_Framebuffer->ClearAttachment(1, -1);
		// Update scene
		m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);

		auto[mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity{} : Entity{ (entt::entity)pixelData, m_ActiveScene.get() };
		}
		else
			m_HoveredEntity = Entity{};

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
		ShowEditorStats();

		ShowWelcomePopup();
		ShowHelpPopup();
		ShowAboutPopup();

		ImGui::End();
	}

	void EditorLayer::SetEditorDefaultDockLayout()
	{
		m_ScenePanels.SetShown(true);
		m_ShowViewport = true, m_ShowStats = true;

		ImGui::DockBuilderRemoveNode(m_DockspaceID);
		ImGui::DockBuilderAddNode(m_DockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(m_DockspaceID, ImGui::GetWindowSize());

		ImGuiID dock_main_id = m_DockspaceID;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.30f, NULL, &dock_main_id);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.40f, NULL, &dock_main_id);

		ImGuiID dock_id_properties = NULL;
		ImGuiID dock_id_scene_hierarchy = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Up, 0.60f, NULL, &dock_id_properties);

		ImGui::DockBuilderDockWindow(SCENE_HIERARCHY_TAB_TITLE, dock_id_scene_hierarchy);
		ImGui::DockBuilderDockWindow(PROPERTIES_TAB_TITLE, dock_id_properties);
		ImGui::DockBuilderDockWindow(VIEWPORT_TAB_TITLE, dock_main_id);
		ImGui::DockBuilderDockWindow(STATS_TAB_TITLE, dock_id_right);
		ImGui::DockBuilderFinish(m_DockspaceID);
	}

	void EditorLayer::ShowEditorMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			ShowFileMenu();
			ShowSceneMenu();
			ShowWindowMenu();
			ShowHelpMenu();

			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::ShowEditorViewport()
	{
		if (m_ShowViewport)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			if (ImGui::Begin(VIEWPORT_TAB_TITLE, &m_ShowViewport))
			{
				auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
				auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
				auto viewportOffset = ImGui::GetWindowPos(); // Where ImGui will start rendering for the viewport.
				m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
				m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
				m_ViewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

				m_ViewportFocused = ImGui::IsWindowFocused();
				m_ViewportHovered = ImGui::IsWindowHovered();
				Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused && !m_ViewportHovered);

				uint32_t textureId = m_Framebuffer->GetColorAttachmentRendererID();
				ImGui::Image((void*)(uint64_t)textureId, ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			}
			
			// Gizmos
			Entity selectedEntity = m_ScenePanels.GetSelectedEntity(); // TODO: change
			if (selectedEntity && m_GizmoType >= 0)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				
				ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportSize.x, m_ViewportSize.y);

				// Camera

				/*
				// Gizmo camera from entity
				auto camEntity = m_ActiveScene->GetPrimaryCameraEntity();
				const auto& cc = camEntity.GetComponent<CameraComponent>();
				const glm::mat4& cameraProjection = cc.Camera.GetProjection();
				glm::mat4 cameraView = glm::inverse(camEntity.GetComponent<TransformComponent>().GetTransform());
				*/

				const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
				glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();
				
				// Entity Transform
				auto & tc = selectedEntity.GetComponent<TransformComponent>();

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

			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	void EditorLayer::ShowEditorStats()
	{
		if (m_ShowStats)
		{
			if (ImGui::Begin(STATS_TAB_TITLE, &m_ShowStats))
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

				if (ImGui::TreeNodeEx("Editor", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
					if (ImGui::BeginTable("editorStats", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
					{
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("Entity Count");
						ImGui::TableNextColumn();
						ImGui::Text("%d", m_ActiveScene->GetNumEntites());

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("Selected Entity");
						ImGui::TableNextColumn();

						if (m_ScenePanels.GetSelectedEntity())
							ImGui::Text(m_ScenePanels.GetSelectedEntity().GetTag().c_str());
						else
							ImGui::TextDisabled("None");

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("Hovered Entity");
						ImGui::TableNextColumn();
						if (m_HoveredEntity)
							ImGui::Text(m_HoveredEntity.GetTag().c_str());
						else
							ImGui::TextDisabled("None");
						ImGui::EndTable();
					}
					ImGui::PopStyleColor();
					ImGui::TreePop();
				}
			}
			ImGui::End();
		}
	}

	void EditorLayer::ShowFileMenu()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItemEx("New", ICON_FA_FILE, "Ctrl+N", false)) NewScene();
			if (ImGui::MenuItemEx("Open...", ICON_FA_FOLDER_OPEN, "Ctrl+O", false)) OpenSceneDialog();
			if (ImGui::MenuItemEx("Save", ICON_FA_SAVE, "Ctrl+S", false)) SaveScene();
			if (ImGui::MenuItemEx("Save As...", NULL, "Ctrl+Shift+S", false)) SaveSceneDialog();
			ImGui::Separator();
			ShowRecentFilesMenu();
			ImGui::Separator();
			if (ImGui::MenuItemEx("Exit", NULL, "Alt+F4", false)) Application::Get().Close();
			ImGui::EndMenu();
		}
	}

	void EditorLayer::ShowRecentFilesMenu()
	{
		if (ImGui::BeginMenuEx("Open Recent", ICON_FA_FOLDER_OPEN, !m_RecentPaths.empty()))
		{
			for (int i = 0; i < m_RecentPaths.size(); i++)
			{
				if (ImGui::MenuItemEx(m_RecentPaths.at(i).string().c_str(), std::to_string(i).c_str()))
					OpenScene(m_RecentPaths.at(i).string());
			}
			ImGui::EndMenu();
		}
	}

	void EditorLayer::ShowSceneMenu()
	{
		if (ImGui::BeginMenu("Scene"))
		{
			m_ScenePanels.ShowNewEntityMenu();
			ImGui::EndMenu();
		}
	}

	void EditorLayer::ShowWindowMenu()
	{
		if (ImGui::BeginMenu("Window"))
		{
			m_ScenePanels.ShowWindowMenuItems();
			if (ImGui::MenuItemEx(VIEWPORT_TITLE, VIEWPORT_ICON, "Ctrl+Shift+V", m_ShowViewport)) m_ShowViewport = !m_ShowViewport;
			if (ImGui::MenuItemEx(STATS_TITLE, STATS_ICON, NULL, m_ShowStats)) m_ShowStats = !m_ShowStats;
			ImGui::Separator();
			if (ImGui::MenuItemEx("Reset Layout", ICON_FA_REDO, "Ctrl+Shift+R", false))
				ImGui::DockBuilderRemoveNode(m_DockspaceID);
			ImGui::Separator();
			GuiThemeManager::ShowThemeMenu();
			ImGui::EndMenu();
		}
	}

	void EditorLayer::ShowHelpMenu()
	{
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItemEx("Welcome", ICON_FA_GLOBE)) m_ShowWelcome = true;
			if (ImGui::MenuItemEx("Help", ICON_FA_QUESTION)) m_ShowHelp = true;
			if (ImGui::MenuItemEx("About", ICON_FA_ADDRESS_BOOK)) m_ShowAbout = true;
			ImGui::EndMenu();
		}
	}

	void EditorLayer::ShowWelcomePopup()
	{
		if (m_ShowWelcome)
		{
			ImGui::OpenPopup("Welcome");
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("Welcome", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
			{
				ImGui::PushFont(GuiThemeManager::GetFont(GuiFont::Header));
				ImGui::Text("Welcome to Milky!");
				ImGui::PopFont();
				ImGui::Separator();
				ImGui::Text("Milky is an open-source independently written interactive application and rendering engine for Windows.");
				ImGui::Text("Please report bugs to https://github.com/Ceriole/Milky/issues");
				if (ImGui::Button("OK")) { ImGui::CloseCurrentPopup(); m_ShowWelcome = false; }
				ImGui::SetItemDefaultFocus();
				ImGui::EndPopup();
			}
		}
	}

	void EditorLayer::ShowHelpPopup()
	{
		if (m_ShowHelp)
		{
			ImGui::OpenPopup("Help");
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("Help", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
			{
				ImGui::Text("TODO"); // TODO: Fill out help text
				if (ImGui::Button("OK")) { ImGui::CloseCurrentPopup(); m_ShowHelp = false; }
				ImGui::SetItemDefaultFocus();
				ImGui::EndPopup();
			}
			
		}
	}

	void EditorLayer::ShowAboutPopup()
	{
		if (m_ShowAbout)
		{
			ImGui::OpenPopup("About");
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
			{
				ImGui::PushFont(GuiThemeManager::GetFont(GuiFont::Header));
				ImGui::Text("Milky Engine");
				ImGui::PopFont();
				ImGui::Separator();
				ImGui::PushFont(GuiThemeManager::GetFont(GuiFont::Bold));
				ImGui::Text("Milky Core Team");
				ImGui::PopFont();
				ImGui::Separator();
				ImGui::Text("Ceriole");
				ImGui::Separator();
				ImGui::TextDisabled("This engine is still a Work-In-Progress, features and operation are subject to change.");
				if (ImGui::Button("OK")) { ImGui::CloseCurrentPopup(); m_ShowAbout = false; }
				ImGui::SetItemDefaultFocus();
				ImGui::EndPopup();
			}
			
		}
	}


	void EditorLayer::CreateEmptyScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_EditorCamera = EditorCamera(30.0f, 1.78f, 0.1f, 1000.0f);
		m_ScenePanels.SetContext(m_ActiveScene);

		if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0)
		{
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}
	}

	void EditorLayer::NewScene()
	{
		CreateEmptyScene();
		SetActiveFilepath("");
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (!path.empty())
		{
			if (path.extension().string() != ".milky")
			{
				ML_WARN("Could not load '{0}' - Not a scene file.", path.filename().string());
				return;
			}
			Ref<Scene> oldScene = m_ActiveScene;
			CreateEmptyScene();
			SceneSerializer serializer(m_ActiveScene);
			if (serializer.Deserialize(path.string()))
				SetActiveFilepath(path);
			else
				m_ActiveScene = oldScene;
		}
	}

	void EditorLayer::SaveScene(const std::filesystem::path& path)
	{
		SceneSerializer serializer(m_ActiveScene);
		if (!path.empty())
		{
			serializer.Serialize(path.string());
			SetActiveFilepath(path);
		}
		else if(!m_ActivePath.empty())
		{
			serializer.Serialize(m_ActivePath.string());
			SetActiveFilepath(m_ActivePath);
		}
		else
		{
			std::filesystem::path newFilepath = FileDialogs::Save("Milky Scene (*.milky)\0*.milky\0");
			serializer.Serialize(newFilepath.string());
			SetActiveFilepath(newFilepath);
		}
	}

	void EditorLayer::OpenSceneDialog()
	{
		std::filesystem::path filepath = FileDialogs::Open("Milky Scene (*.milky)\0*.milky\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::SaveSceneDialog()
	{
		std::string filepath = FileDialogs::Save("Milky Scene (*.milky)\0*.milky\0");
		if (!filepath.empty())
			SaveScene(filepath);
	}

	void EditorLayer::SetActiveFilepath(const std::filesystem::path& path)
	{
		if (!m_ActivePath.empty())
		{
			m_RecentPaths.erase(std::remove(m_RecentPaths.begin(), m_RecentPaths.end(), m_ActivePath), m_RecentPaths.end());
			m_RecentPaths.insert(m_RecentPaths.begin(), m_ActivePath.string());
		}
		m_ActivePath = path.string();
		Application::Get().GetWindow().SetTitle("MilkGlass - " + (m_ActivePath.empty() ? "Unsaved Scene" : m_ActivePath.string()));
	}


	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ML_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(ML_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		const bool ctrl = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		const bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		const bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);

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
		case Key::F3:
			m_ScenePanels.ShowSceneHierarchyPanel = !m_ScenePanels.ShowSceneHierarchyPanel;
			break;
		case Key::F4:
			if (alt)
				Application::Get().Close();
			else
				m_ScenePanels.ShowPropertiesPanel = !m_ScenePanels.ShowPropertiesPanel;
			break;
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if(CanMousePick())
				m_ScenePanels.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	bool EditorLayer::CanMousePick()
	{
		return m_ViewportHovered && !ImGuizmo::IsOver() && !m_EditorCamera.IsUsing();
	}

}
