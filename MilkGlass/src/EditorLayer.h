#pragma once

#include <Milky.h>

#include "Panels/ScenePanels.h"

#include <imgui/imgui.h>

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
	private:
		void SetEditorDefaultDockLayout();
		void ShowEditorMenuBar();
		void ShowEditorViewport();
		void ShowEditorSettings(); // TODO: Temporary
	private:
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity0, m_SquareEntity1;
		Entity m_CameraEntity;

		ImGuiID m_DockspaceID = NULL;

		bool m_ShowViewport = true;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		ScenePanels m_ScenePanels;
	};
}
