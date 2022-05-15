#include "SceneHierarchyPanel.h"

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

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImguiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_Context->m_Registry.each([&](auto entityID)
		{
			Entity entity{ entityID, m_Context.get() };
			DrawEntityNode(entity);
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			DrawNewEntityMenu();
			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectedEntity)
			DrawComponents(m_SelectedEntity);
		else
		{
			constexpr const char* noSelectionText = "No Entity Selected!\nSelect an entity in the Scene Hierarchy.";
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 textSize = ImGui::CalcTextSize(noSelectionText);
			ImVec2 textPos = ImVec2{ (windowSize.x - textSize.x) * 0.5f , (windowSize.y - textSize.y) * 0.5f };
			ImGui::SetCursorPos(textPos);
			ImGui::TextDisabled(noSelectionText);
		}
		ImGui::End();
	}

	static void DrawEntityMenu(bool& entityDeleted)
	{
		if (ImGui::MenuItem("Delete Entity"))
			entityDeleted = true;
	}

	void SceneHierarchyPanel::DrawNewEntityMenu()
	{
		if (ImGui::MenuItem("Create Empty Entity"))
			m_Context->CreateEntity();

		if (ImGui::MenuItem("Create Square"))
		{
			Entity entity = m_Context->CreateEntity("New Square");
			entity.AddComponent<SpriteRendererComponent>();
		}

		if (ImGui::BeginMenu("Create Camera"))
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
	}

	void SceneHierarchyPanel::DrawAddComponentMenu()
	{
		if (ImGui::MenuItem("Camera", NULL, false, !m_SelectedEntity.HasComponent<CameraComponent>()))
		{
			if (!m_SelectedEntity.HasComponent<CameraComponent>())
				m_SelectedEntity.AddComponent<CameraComponent>();
			else
				ML_CORE_WARN("This entity already has the Camera Component!");
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Sprite Renderer", NULL, false, !m_SelectedEntity.HasComponent<SpriteRendererComponent>()))
		{
			if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
				m_SelectedEntity.AddComponent<SpriteRendererComponent>();
			else
				ML_CORE_WARN("This entity already has the Sprite Renderer Component!");
			ImGui::CloseCurrentPopup();
		}
	}

	template<typename Comp, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction, bool deleteable = true)
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

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
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
			DrawEntityMenu(entityDeleted);
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

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
		static float contextButtonWidth = 50.0f;
		float pos = contextButtonWidth + itemSpacing + 10.0f;
		ImGui::SameLine(ImGui::GetWindowWidth() - pos);
		if (ImGui::Button("..."))
			ImGui::OpenPopup("ComponentContextMenu");
		contextButtonWidth = ImGui::GetItemRectSize().x;

		bool entityDeleted = false;
		if (ImGui::BeginPopup("ComponentContextMenu"))
		{
			DrawEntityMenu(entityDeleted);
			ImGui::EndPopup();
		}
		ImGui::PushItemWidth(-1);
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");
		ImGui::PopItemWidth();

		if (ImGui::BeginPopup("AddComponent"))
		{
			DrawAddComponentMenu();
			ImGui::EndPopup();
		}

		DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component)
			{
				DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f);
			}, false);

		DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
			{
				auto& camera = component.Camera;

				DrawBoolControl("Primary", &component.Primary);
				std::vector<std::string> projectionTypeStrings = { "Perspective", "Orthographic" };
				int projType = (int)camera.GetProjectionType();
				if (DrawComboControl("Projection", projectionTypeStrings, projType))
					camera.SetProjectionType((SceneCamera::ProjectionType)projType);

				switch (camera.GetProjectionType())
				{
				case SceneCamera::ProjectionType::Perspective:
					{
						float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
						if (DrawFloatControl("Vertical FOV", &perspectiveVerticalFov))
							camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

						float perspectiveNear = camera.GetPerspectiveNearClip();
						if (DrawFloatControl("Near", &perspectiveNear))
							camera.SetPerspectiveNearClip(perspectiveNear);

						float perspectiveFar = camera.GetPerspectiveFarClip();
						if (DrawFloatControl("Far", &perspectiveFar))
							camera.SetPerspectiveFarClip(perspectiveFar);
						break;
					}
				case SceneCamera::ProjectionType::Orthographic:
					{
						float orthoSize = camera.GetOrthographicSize();
						if (DrawFloatControl("Size", &orthoSize))
							camera.SetOrthographicSize(orthoSize);

						float orthoNear = camera.GetOrthographicNearClip();
						if (DrawFloatControl("Near", &orthoNear))
							camera.SetOrthographicNearClip(orthoNear);

						float orthoFar = camera.GetOrthographicFarClip();
						if (DrawFloatControl("Far", &orthoFar))
							camera.SetOrthographicFarClip(orthoFar);

						DrawBoolControl("Fixed Aspect\nRatio", &component.FixedAspectRatio);

						break;
					}
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
			{
				// TODO: Textures and materials
				DrawColorControl("Color", component.Color);
			});

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}
	}
}
