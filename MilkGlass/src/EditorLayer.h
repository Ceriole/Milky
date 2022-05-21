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
		void OpenScene(std::string filepath);
		void SaveScene(std::string filepath = std::string());
		void OpenSceneDialog();
		void SaveSceneDialog();
	private:
		void SetActiveFilepath(std::string filepath);
		void SetEditorDefaultDockLayout();
		void ShowEditorMenuBar();
		void ShowEditorViewport();
		void ShowEditorSettings(); // TODO: Temporary
		void ShowRecentFilesMenu();

		bool OnKeyPressed(KeyPressedEvent& e);
	private:
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_Framebuffer;

		std::vector<std::string> m_RecentPaths;
		std::string m_ActivePath;
		Ref<Scene> m_ActiveScene;

		EditorCamera m_EditorCamera;

		ImGuiID m_DockspaceID = NULL;

		bool m_ShowViewport = true, m_ShowStats = false;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		int m_GizmoType = -1;
		ImGuizmo::MODE m_GizmoMode = ImGuizmo::MODE::LOCAL;

		ScenePanels m_ScenePanels;
	};
}
