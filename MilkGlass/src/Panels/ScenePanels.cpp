#include "ScenePanels.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Milky/Scene/Components.h"
#include <cstring>

#include "Milky/ImGui/ImGuiUtil.h"

#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif


namespace Milky {

	ScenePanels::ScenePanels(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void ScenePanels::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void ScenePanels::ShowWindowMenuItems()
	{
		ImGui::MenuItem("Scene Hierarchy", "Ctrl+Shift+H", &m_ShowSceneHierarchyPanel);
		ImGui::MenuItem("Properties", "Ctrl+Shift+P", &m_ShowPropertiesPanel);
	}

	void ScenePanels::OnImguiRender()
	{
		if (m_ShowSceneHierarchyPanel)
		{
			if (ImGui::Begin("Scene Hierarchy", &m_ShowSceneHierarchyPanel))
			{
				m_Context->m_Registry.each([&](auto entityID)
					{
						Entity entity{ entityID, m_Context.get() };
						ShowEntityNode(entity);
					});

				if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
					m_SelectedEntity = {};

				if (ImGui::BeginPopupContextWindow(NULL, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
				{
					ShowNewEntityMenu();
					ImGui::EndPopup();
				}
			}
			ImGui::End();
		}

		if (m_ShowPropertiesPanel)
		{
			if (ImGui::Begin("Properties", &m_ShowPropertiesPanel))
			{
				if (m_SelectedEntity)
				{
					bool entityDeleted = false;
					ShowEntityHeader(m_SelectedEntity, entityDeleted);
					ShowComponents(m_SelectedEntity);
					if (entityDeleted)
					{
						m_Context->DestroyEntity(m_SelectedEntity);
						m_SelectedEntity = {};
					}
				}
				else
				{
					constexpr const char* noSelectionText = "No Entity Selected!\nSelect an entity in the Scene Hierarchy.";
					ImVec2 windowSize = ImGui::GetWindowSize();
					ImVec2 textSize = ImGui::CalcTextSize(noSelectionText);
					ImVec2 textPos = ImVec2{ (windowSize.x - textSize.x) * 0.5f , (windowSize.y - textSize.y) * 0.5f };
					ImGui::SetCursorPos(textPos);
					ImGui::TextDisabled(noSelectionText);
				}
			}
			ImGui::End();
		}
	}

	static void ShowEntityMenuItems(Entity entity, bool& entityDeleted)
	{
		if (ImGui::MenuItem("Delete Entity"))
			entityDeleted = true;
	}

	void ScenePanels::ShowNewEntityMenu()
	{
		if (ImGui::BeginMenu("New Entity"))
		{
			if (ImGui::MenuItem("Empty"))
				m_Context->CreateEntity("New Entity");
			if (ImGui::MenuItem("Square"))
			{
				Entity entity = m_Context->CreateEntity("New Square");
				entity.AddComponent<SpriteRendererComponent>();
			}
			if (ImGui::BeginMenu("Camera"))
			{
				if (ImGui::MenuItem("Orthographic"))
				{
					Entity entity = m_Context->CreateEntity("New Camera");
					auto& cam = entity.AddComponent<CameraComponent>();
					cam.Primary = false;
				}
				if (ImGui::MenuItem("Perspective"))
				{
					Entity entity = m_Context->CreateEntity("New Camera");
					auto& cam = entity.AddComponent<CameraComponent>();
					cam.Camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);
					cam.Primary = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
	}

	void ScenePanels::DrawAddComponentMenuItems(Entity entity)
	{
		if (ImGui::MenuItem("Camera", NULL, false, !entity.HasComponent<CameraComponent>()))
		{
			if (!entity.HasComponent<CameraComponent>())
				entity.AddComponent<CameraComponent>();
			else
				ML_CORE_WARN("This entity already has the Camera Component!");
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Sprite Renderer", NULL, false, !entity.HasComponent<SpriteRendererComponent>()))
		{
			if (!entity.HasComponent<SpriteRendererComponent>())
				entity.AddComponent<SpriteRendererComponent>();
			else
				ML_CORE_WARN("This entity already has the Sprite Renderer Component!");
			ImGui::CloseCurrentPopup();
		}
	}

	void ScenePanels::ShowEntityHeader(Entity entity, bool& entityDeleted)
	{
		const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
		const ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		static float contextButtonWidth = 50.0f;
		static float addComponentButtonWidth = 100.0f;

		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			ImGui::PushItemWidth(contentRegionAvailable.x - (contextButtonWidth * 0.5f) - addComponentButtonWidth - (itemSpacing * 3.0f));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer);
			ImGui::PopItemWidth();
		}
		float pos = contextButtonWidth * 0.5f;
		ImGui::SameLine(contentRegionAvailable.x - pos);
		if (ImGui::Button("..."))
			ImGui::OpenPopup("ComponentContextMenu");
		contextButtonWidth = ImGui::GetItemRectSize().x;

		if (ImGui::BeginPopup("ComponentContextMenu"))
		{
			ShowEntityMenuItems(entity, entityDeleted);
			ImGui::EndPopup();
		}

		pos += addComponentButtonWidth + itemSpacing;
		ImGui::SameLine(contentRegionAvailable.x - pos);
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");
		addComponentButtonWidth = ImGui::GetItemRectSize().x;

		if (ImGui::BeginPopup("AddComponent"))
		{
			DrawAddComponentMenuItems(entity);
			ImGui::EndPopup();
		}
	}

	void ScenePanels::ShowEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			ShowEntityMenuItems(entity, entityDeleted);
			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TextDisabled("No Children");
			/* TODO: Entity children and parenting
			for (Entity child : entity.Children)
				DrawEntityNode(child);
			*/
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}
	}

	template<typename Comp, typename UIFunction>
	static void ShowComponent(const std::string& name, Entity entity, UIFunction uiFunction, bool deleteable = true)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<Comp>())
		{
			auto& component = entity.GetComponent<Comp>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(Comp).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component", 0, false, deleteable))
					removeComponent = true;
				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<Comp>();
		}
	}

	void ScenePanels::ShowComponents(Entity entity)
	{
		ShowComponent<TransformComponent>("Transform", entity, [](TransformComponent& component)
			{
				UIControls::ShowXYZControl("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				UIControls::ShowXYZControl("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				UIControls::ShowXYZControl("Scale", component.Scale, 1.0f);
			}, false);

		ShowComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
			{
				auto& camera = component.Camera;

				UIControls::ShowBoolControl("Primary", &component.Primary);
				std::vector<std::string> projectionTypeStrings = { "Perspective", "Orthographic" };
				int projType = (int)camera.GetProjectionType();
				if (UIControls::ShowComboControl("Projection", projectionTypeStrings, projType))
					camera.SetProjectionType((SceneCamera::ProjectionType)projType);

				switch (camera.GetProjectionType())
				{
				case SceneCamera::ProjectionType::Perspective:
					{
						float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
						if (UIControls::ShowFloatControl("Vertical FOV", &perspectiveVerticalFov, glm::degrees(s_CameraDefaults.PerspectiveFOV)))
							camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

						float perspectiveNear = camera.GetPerspectiveNearClip();
						if (UIControls::ShowFloatControl("Near", &perspectiveNear, s_CameraDefaults.PerspectiveNear))
							camera.SetPerspectiveNearClip(perspectiveNear);

						float perspectiveFar = camera.GetPerspectiveFarClip();
						if (UIControls::ShowFloatControl("Far", &perspectiveFar, s_CameraDefaults.PerspectiveFar))
							camera.SetPerspectiveFarClip(perspectiveFar);
						break;
					}
				case SceneCamera::ProjectionType::Orthographic:
					{
						float orthoSize = camera.GetOrthographicSize();
						if (UIControls::ShowFloatControl("Size", &orthoSize, s_CameraDefaults.OrthographicSize))
							camera.SetOrthographicSize(orthoSize);

						float orthoNear = camera.GetOrthographicNearClip();
						if (UIControls::ShowFloatControl("Near", &orthoNear, s_CameraDefaults.OrthographicNear))
							camera.SetOrthographicNearClip(orthoNear);

						float orthoFar = camera.GetOrthographicFarClip();
						if (UIControls::ShowFloatControl("Far", &orthoFar, s_CameraDefaults.OrthographicFar))
							camera.SetOrthographicFarClip(orthoFar);

						UIControls::ShowBoolControl("Fixed Aspect\nRatio", &component.FixedAspectRatio);

						break;
					}
				}
			});

		ShowComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
			{
				// TODO: Textures and materials
				UIControls::DrawColorControl("Color", component.Color);
			});
	}
}
