#pragma once

#include <Milky.h>

#include "Panels/EditorPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/StatsPanel.h"

#include <imgui/imgui.h>
#include <ImGuizmo.h>

#define MILKY_SCENE_FILE_EXT		".mlscn"
#define MILKY_SCENE_FILE_DECRIPTION "Milky Scene (*" MILKY_SCENE_FILE_EXT ")"
#define MILKY_SCENE_FILE_FILTER		MILKY_SCENE_FILE_DECRIPTION "\0*" MILKY_SCENE_FILE_EXT "\0"

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

		void ShowFileMenu();
		void ShowRecentFilesMenu();
		void ShowSceneMenu();
		void ShowWindowMenu();
		void ShowHelpMenu();

		void ShowWelcomePopup();
		void ShowHelpPopup();
		void ShowAboutPopup();

		bool OnKeyPressed(KeyPressedEvent& e);
	private:
		Ref<EditorData> m_Context;

		std::vector<std::filesystem::path> m_RecentPaths;
		std::filesystem::path m_ActivePath;

		ImGuiID m_DockspaceID = NULL;

		bool m_ShowWelcome = false, m_ShowHelp = false, m_ShowAbout = false;

		std::vector<EditorPanel*> m_Panels;

		ViewportPanel* m_ViewportPanel = nullptr;
		SceneHierarchyPanel* m_SceneHierarchyPanel = nullptr;
		PropertiesPanel* m_PropertiesPanel = nullptr;
		ContentBrowserPanel* m_ContentBrowserPanel = nullptr;
		StatsPanel* m_StatsPanel = nullptr;
	};
}
