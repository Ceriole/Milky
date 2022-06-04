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
	private:
		void SetEditorDefaultDockLayout();
		void ShowEditorMenuBar();

		void ShowFileMenu();
		void ShowRecentFilesMenu();
		void ShowSceneMenu();
		void ShowWindowMenu();
		void ShowHelpMenu();
		void ShowToolbar();

		void ShowWelcomePopup();
		void ShowHelpPopup();
		void ShowAboutPopup();

		bool OnKeyPressed(KeyPressedEvent& e);
	private:
		ImGuiID m_DockspaceID = NULL;

		Ref<EditorContext> m_Context;
		std::vector<EditorPanel*> m_Panels;
		ViewportPanel* m_ViewportPanel = nullptr;
		SceneHierarchyPanel* m_SceneHierarchyPanel = nullptr;
		PropertiesPanel* m_PropertiesPanel = nullptr;
		ContentBrowserPanel* m_ContentBrowserPanel = nullptr;
		StatsPanel* m_StatsPanel = nullptr;

		Ref<Texture2D> m_IconPlay, m_IconStop, m_IconPause;

		bool m_ShowWelcome = false, m_ShowHelp = false, m_ShowAbout = false;
	};
}
