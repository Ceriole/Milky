#include "ViewportPanel.h"

#include "Milky/Core/Application.h"
#include "Milky/Core/Input.h"
#include "Milky/Scene/Components/Components.h"
#include "Milky/Math/Math.h"

#include "Editor/EditorUtils.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Milky {

	extern const std::filesystem::path g_AssetPath;

	ViewportPanel::ViewportPanel(const Ref<EditorContext>& context, const std::string& title, const std::string& icon, const std::string& shortcut)
		: EditorPanel(context, title, icon, shortcut)
	{}

	void ViewportPanel::OnUpdate(Timestep ts)
	{
		auto viewportSize = Size();
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_Bounds[0].x;
		my -= m_Bounds[0].y;
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Context->Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_Context->HoveredEntity = pixelData == -1 ? Entity{} : Entity{ (entt::entity)pixelData, m_Context->ActiveScene.get() };
		}
		else
			m_Context->HoveredEntity = Entity{};
	}

	void ViewportPanel::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ML_BIND_EVENT_FN(ViewportPanel::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(ML_BIND_EVENT_FN(ViewportPanel::OnMouseButtonPressed));
	}

	void ViewportPanel::ShowContent()
	{
		auto viewportSize = Size();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		
		Application::Get().GetImGuiLayer()->SetBlockEvents(!IsFocused() && !IsHovered());

		// Draw framebuffer onto window
		uint32_t textureId = m_Context->Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureId), ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ML_PAYLOAD_TYPE_ASSET))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				m_Context->OpenScene(std::filesystem::path(g_AssetPath) / path);
			}

			ImGui::EndDragDropTarget();
		}

		// Gizmos
		Entity selectedEntity = m_Context->Selection->GetEntity();
		if (selectedEntity && m_GizmoType >= 0)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_Bounds[0].x, m_Bounds[0].y, viewportSize.x, viewportSize.y);

			// Camera

			/*
			// Gizmo camera from entity
			auto camEntity = m_ActiveScene->GetPrimaryCameraEntity();
			const auto& cc = camEntity.GetComponent<CameraComponent>();
			const glm::mat4& cameraProjection = cc.Camera.GetProjection();
			glm::mat4 cameraView = glm::inverse(camEntity.GetComponent<TransformComponent>().GetTransform());
			*/

			const glm::mat4& cameraProjection = m_Context->Camera->GetProjection();
			glm::mat4 cameraView = m_Context->Camera->GetViewMatrix();

			// Entity Transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();

			glm::mat4 transform = tc.GetTransform();
			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl); // TODO: Snap setting? Set snap values in UI
			float snapValue = 0.5f; // Snap to 0.5m for translation and scale
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f; // Snap to 45 degrees for rotation

			float snapValues[3] = { snapValue, snapValue, snapValue };

			// Manupulate
			if (ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_GizmoType, m_GizmoMode, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr))
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		ImGui::PopStyleVar();
	}

	bool ViewportPanel::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		switch (e.GetKeyCode())
		{
			// Gizmos
		case Key::Q:
			m_GizmoType = -1;
			break;
		case Key::W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Key::E:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case Key::R:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}

		return false;
	}

	bool ViewportPanel::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (CanMousePick())
				m_Context->SelectEntity(m_Context->HoveredEntity);
		}
		return false;
	}

	bool ViewportPanel::CanMousePick()
	{
		return IsHovered() && !ImGuizmo::IsOver() && !m_Context->Camera->IsUsing();
	}
}
