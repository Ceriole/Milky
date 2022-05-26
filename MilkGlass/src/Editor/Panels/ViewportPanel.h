#pragma once

#include "EditorPanel.h"

#include <imgui/imgui.h>
#include <ImGuizmo.h>

namespace Milky {

	class ViewportPanel : public EditorPanel
	{
	public:
		ViewportPanel(const Ref<EditorData>& context, const std::string& title, const std::string& icon, const std::string& shortcut);

		virtual void OnUpdate(Timestep ts) override;

		virtual void OnEvent(Event& e) override;
	protected:
		virtual void ShowContent() override;

		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	private:
		bool CanMousePick();
	private:
		int m_GizmoType = -1;
		ImGuizmo::MODE m_GizmoMode = ImGuizmo::MODE::LOCAL;
	};

}
