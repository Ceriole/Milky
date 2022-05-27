#pragma once

#include "Milky/Renderer/Framebuffer.h"
#include "Milky/Scene/Scene.h"
#include "Milky/Renderer/EditorCamera.h"

#include "Milky/Events/Event.h"
#include "Milky/Events/KeyEvent.h"
#include "Milky/Events/MouseEvent.h"

#include "Editor/EditorContext.h"

namespace Milky {

	class EditorPanel
	{
	public:
		EditorPanel(const Ref<EditorContext>& context, const std::string& title, const std::string& icon, const std::string& shortcut);

		void SetContext(const Ref<EditorContext>& context) { m_Context = context; }
		
		void OnImGuiRender();
		void ShowMenuItem();

		virtual void OnUpdate(Timestep ts) {};
		virtual void OnEvent(Event& e) {};

		void ToggleOpen() { Show = !Show; };

		bool IsHovered() const { return m_Hovered; };
		bool IsFocused() const { return m_Focused; };

		const char* Title() const { return m_Title.c_str(); };
		const char* Icon() const { return m_Icon.c_str(); };
		const char* Shortcut() const { return m_Icon.c_str(); };
		const char* TabTitle() const { return m_TabTitle.c_str(); };

		const glm::vec2 MinBounds() const { return m_Bounds[0]; };
		const glm::vec2 MaxBounds() const { return m_Bounds[1]; };
		const glm::vec2 Size() const { return m_Bounds[1] - m_Bounds[0]; };
	private:
		void CalculateBounds();
	protected:
		virtual void ShowContent() = 0;
	public:
		bool Show = true;
	private:
		bool m_Hovered = false, m_Focused = false;
	protected:
		std::string m_Title, m_Icon, m_TabTitle, m_Shortcut;
		Ref<EditorContext> m_Context;
		glm::vec2 m_Bounds[2];
	};

}
