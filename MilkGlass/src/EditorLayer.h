#pragma once

#include <Milky.h>

#include "Panels/ScenePanels.h"

#include <imgui/imgui.h>
#include <ImGuizmo.h>

#include "Milky/Renderer/EditorCamera.h"

#define VIEWPORT_TITLE		"Viewport"
#define VIEWPORT_ICON		ICON_FA_CUBE
#define VIEWPORT_TAB_TITLE	VIEWPORT_ICON " " VIEWPORT_TITLE
#define STATS_TITLE			"Stats"
#define STATS_ICON			ICON_FA_COG
#define STATS_TAB_TITLE		ICON_FA_COG " " STATS_TITLE

#define MILKY_SCENE_FILE_EXT		".mlscn"
#define MILKY_SCENE_FILE_DECRIPTION

namespace Milky {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdateRuntime(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;

		void NewScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene(const std::filesystem::path& path = std::string());
		void OpenSceneDialog();
		void SaveSceneDialog();
	private:
		void CreateEmptyScene();
		void SetActiveFilepath(const std::filesystem::path& path);
		void SetEditorDefaultDockLayout();
		void ShowEditorMenuBar();
		void ShowEditorViewport();
		void ShowEditorStats(); // TODO: Temporary

		void ShowFileMenu();
		void ShowRecentFilesMenu();
		void ShowSceneMenu();
		void ShowWindowMenu();
		void ShowHelpMenu();

		void ShowWelcomePopup();
		void ShowHelpPopup();
		void ShowAboutPopup();

		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		bool CanMousePick();
	private:
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_Framebuffer;

		std::vector<std::filesystem::path> m_RecentPaths;
		std::filesystem::path m_ActivePath;
		Ref<Scene> m_ActiveScene;
		Entity m_HoveredEntity;

		EditorCamera m_EditorCamera;

		ImGuiID m_DockspaceID = NULL;

		bool m_ShowViewport = true, m_ShowStats = true;
		bool m_ShowWelcome = false, m_ShowHelp = false, m_ShowAbout = false;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2] = { { 0.0f, 0.0f }, { 0.0f, 0.0f } };

		int m_GizmoType = -1;
		ImGuizmo::MODE m_GizmoMode = ImGuizmo::MODE::LOCAL;

		ScenePanels m_ScenePanels;
	};
}
