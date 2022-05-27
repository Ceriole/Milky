#include "EditorLayer.h"

#include <imgui/imgui_internal.h>
#include <iconfonts/IconsFontAwesome5.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Milky/ImGui/ImGuiUtil.h"
#include "Milky/Scene/SceneSerializer.h"
#include "Milky/Utils/PlatformUtils.h"
#include "Milky/Math/Math.h"

#include "EditorUtils.h"

namespace Milky {

	EditorLayer::EditorLayer()
		: Layer("MilkGlassEditorLayer")
	{}

	void EditorLayer::OnAttach()
	{
		ML_PROFILE_FUNCTION();

		m_Context = CreateRef<EditorContext>();

		m_Context->Selection = CreateRef<SelectionContext>();

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Context->Framebuffer = Framebuffer::Create(fbSpec);

		m_ViewportPanel = new ViewportPanel(m_Context, "Viewport", ICON_FA_CUBE, "Ctrl+Shift+V");
		m_SceneHierarchyPanel = new SceneHierarchyPanel(m_Context, "Scene Hierarchy", ICON_FA_LIST, "F3");
		m_PropertiesPanel = new PropertiesPanel(m_Context, "Properties", ICON_FA_WRENCH, "F4");
		m_ContentBrowserPanel = new ContentBrowserPanel(m_Context, "Content Browser", ICON_FA_ARCHIVE, "");
		m_StatsPanel = new StatsPanel(m_Context, "Stats", ICON_FA_CODE, "");

		m_Panels.push_back(m_ViewportPanel);
		m_Panels.push_back(m_SceneHierarchyPanel);
		m_Panels.push_back(m_PropertiesPanel);
		m_Panels.push_back(m_ContentBrowserPanel);
		m_Panels.push_back(m_StatsPanel);

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			m_Context->OpenScene(sceneFilePath);
		}
		else
			m_Context->NewScene();

		m_ShowWelcome = true;
	}

	void EditorLayer::OnDetach()
	{
		ML_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdateRuntime(Timestep ts)
	{
		ML_PROFILE_FUNCTION();

		auto viewportSize = m_ViewportPanel->Size();

		if (Milky::FramebufferSpecification spec = m_Context->Framebuffer->GetSpecification(); viewportSize.x > 0.0f && viewportSize.y > 0.0f && (spec.Width != viewportSize.x || spec.Height != viewportSize.y))
		{
			m_Context->Framebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			m_Context->Camera->SetViewportSize(viewportSize.x, viewportSize.y);
			m_Context->ActiveScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		}

		m_Context->Camera->OnUpdate(ts);

		// Render
		Renderer2D::ResetStats();
		m_Context->Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();
		// Clear entity ID attachment to -1
		m_Context->Framebuffer->ClearAttachment(1, -1);

		// Update scene
		m_Context->ActiveScene->OnUpdateEditor(ts, *m_Context->Camera);

		for (auto& p : m_Panels)
			p->OnUpdate(ts);

		m_Context->Framebuffer->Unbind();
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

		for (auto& p : m_Panels)
			p->OnImGuiRender();

		ShowWelcomePopup();
		ShowHelpPopup();
		ShowAboutPopup();

		ImGui::End();
	}

	void EditorLayer::SetEditorDefaultDockLayout()
	{
		for (auto& p : m_Panels)
			p->Show = true;

		ImGui::DockBuilderRemoveNode(m_DockspaceID);
		ImGui::DockBuilderAddNode(m_DockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(m_DockspaceID, ImGui::GetWindowSize());

		ImGuiID dock_viewport_id = m_DockspaceID;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_viewport_id, ImGuiDir_Left, 0.30f, NULL, &dock_viewport_id);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_viewport_id, ImGuiDir_Right, 0.40f, NULL, &dock_viewport_id);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dock_viewport_id, ImGuiDir_Down, 0.40f, NULL, &dock_viewport_id);

		ImGuiID dock_id_properties = NULL;
		ImGuiID dock_id_scene_hierarchy = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Up, 0.60f, NULL, &dock_id_properties);

		ImGui::DockBuilderDockWindow(m_ViewportPanel->TabTitle(), dock_viewport_id);
		ImGui::DockBuilderDockWindow(m_SceneHierarchyPanel->TabTitle(), dock_id_scene_hierarchy);
		ImGui::DockBuilderDockWindow(m_PropertiesPanel->TabTitle(), dock_id_properties);
		ImGui::DockBuilderDockWindow(m_ContentBrowserPanel->TabTitle(), dock_id_down);
		ImGui::DockBuilderDockWindow(m_StatsPanel->TabTitle(), dock_id_right);
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

	void EditorLayer::ShowFileMenu()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItemEx("New", ICON_FA_FILE, "Ctrl+N", false)) m_Context->NewScene();
			if (ImGui::MenuItemEx("Open...", ICON_FA_FOLDER_OPEN, "Ctrl+O", false)) m_Context->OpenSceneDialog();
			if (ImGui::MenuItemEx("Save", ICON_FA_SAVE, "Ctrl+S", false)) m_Context->SaveScene();
			if (ImGui::MenuItemEx("Save As...", NULL, "Ctrl+Shift+S", false)) m_Context->SaveSceneDialog();
			ImGui::Separator();
			ShowRecentFilesMenu();
			ImGui::Separator();
			if (ImGui::MenuItemEx("Exit", NULL, "Alt+F4", false)) Application::Get().Close();
			ImGui::EndMenu();
		}
	}

	void EditorLayer::ShowRecentFilesMenu()
	{
		if (ImGui::BeginMenuEx("Open Recent", ICON_FA_FOLDER_OPEN, !m_Context->RecentPaths().empty()))
		{
			auto& recentPaths = m_Context->RecentPaths();
			if (!recentPaths.empty())
				for (int i = 0; i < recentPaths.size(); i++)
				{
					if (ImGui::MenuItemEx(recentPaths.at(i).string().c_str(), std::to_string(i).c_str()))
						m_Context->OpenScene(recentPaths.at(i).string());
				}
			else
				ImGui::MenuItem("No recent paths.", NULL, nullptr, false);
			ImGui::EndMenu();
		}
	}

	void EditorLayer::ShowSceneMenu()
	{
		if (ImGui::BeginMenu("Scene"))
		{
			EditorUtils::ShowNewEntityMenu(m_Context->ActiveScene);
			ImGui::EndMenu();
		}
	}

	void EditorLayer::ShowWindowMenu()
	{
		if (ImGui::BeginMenu("Window"))
		{
			for (auto& p : m_Panels)
				p->ShowMenuItem();
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

	

	void EditorLayer::OnEvent(Event& e)
	{
		m_Context->Camera->OnEvent(e);

		for (auto& p : m_Panels)
			p->OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ML_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
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
					m_Context->SaveSceneDialog();
				else
					m_Context->SaveScene();
			break;
		case Key::O:
			if (ctrl)
				m_Context->OpenSceneDialog();
			break;
		case Key::N:
			if (ctrl)
				m_Context->NewScene();
			break;
		case Key::F3:
			m_SceneHierarchyPanel->ToggleOpen();
			break;
		case Key::F4:
			if (alt)
				Application::Get().Close();
			else
				m_PropertiesPanel->ToggleOpen();
			break;
		}

		return false;
	}

}
