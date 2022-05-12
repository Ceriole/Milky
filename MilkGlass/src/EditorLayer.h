#pragma once

#include <Milky.h>

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
		OrthographicCameraController m_CameraController;

		// v TEMPORARY v
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		// ^ ========= ^
		Ref<Framebuffer> m_Framebuffer;

		Ref<Texture2D> m_CheckerTexture;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize;

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	};
}