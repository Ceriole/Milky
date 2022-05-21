#pragma once

#include <Milky.h>

#include "Panels/ScenePanels.h"

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
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

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
		Entity m_SquareEntity0, m_SquareEntity1;
		Entity m_CameraEntity;

		ImGuiID m_DockspaceID = NULL;

		bool m_ShowViewport = true;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		int m_GizmoType = -1, m_GizmoMode = (int)ImGuizmo::MODE::LOCAL;

		ScenePanels m_ScenePanels;
	};
}
