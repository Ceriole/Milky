#include "EditorUtils.h"

#include "Milky/Core/Input.h"
#include "Milky/Scene/Entity.h"
#include "Milky/Scene/Components/Components.h"
#include "Milky/Renderer/Texture.h"

#include "Milky/ImGui/ImGuiUtil.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <IconsFontAwesome5.h>


namespace Milky {

	extern const std::filesystem::path g_AssetPath;

	namespace EditorUtils {

		void ShowNewEntityMenu(const Ref<Scene>& scene)
		{
			if (ImGui::BeginMenu("New Entity"))
			{
				if (ImGui::MenuItem("Empty"))
					scene->CreateEntity("New Entity");
				if (ImGui::MenuItem("Square"))
				{
					Entity entity = scene->CreateEntity("New Square");
					entity.AddComponent<SpriteRendererComponent>();
				}
				if (ImGui::BeginMenu("Camera"))
				{
					if (ImGui::MenuItem("Orthographic"))
					{
						Entity entity = scene->CreateEntity("New Camera");
						auto& cam = entity.AddComponent<CameraComponent>();
						cam.Primary = false;
					}
					if (ImGui::MenuItem("Perspective"))
					{
						Entity entity = scene->CreateEntity("New Camera");
						auto& cam = entity.AddComponent<CameraComponent>();
						cam.Camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);
						cam.Primary = false;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
		}

		void ShowEntityMenuItems(Entity entity, bool& entityDeleted)
		{
			if (ImGui::MenuItemEx("Delete Entity", ICON_FA_TRASH))
				entityDeleted = true;
		}

		void ShowAddComponentMenuItems(Entity entity)
		{
			if (ImGui::MenuItem("Camera", NULL, false, !entity.HasComponent<CameraComponent>()))
			{
				if (!entity.HasComponent<CameraComponent>())
					entity.AddComponent<CameraComponent>();
				else
					ML_CORE_WARN("This entity already has a Camera Component!");
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Sprite Renderer", NULL, false, !entity.HasComponent<SpriteRendererComponent>()))
			{
				if (!entity.HasComponent<SpriteRendererComponent>())
					entity.AddComponent<SpriteRendererComponent>();
				else
					ML_CORE_WARN("This entity already has a Sprite Renderer Component!");
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Rigid Body 2D", NULL, false, !entity.HasComponent<RigidBody2DComponent>()))
			{
				if (!entity.HasComponent<RigidBody2DComponent>())
					entity.AddComponent<RigidBody2DComponent>();
				else
					ML_CORE_WARN("This entity already has a Rigid Body 2D Component!");
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Box Collider 2D", NULL, false, !entity.HasComponent<BoxCollider2DComponent>()))
			{
				if (!entity.HasComponent<BoxCollider2DComponent>())
					entity.AddComponent<BoxCollider2DComponent>();
				else
					ML_CORE_WARN("This entity already has a Box Collider 2D Component!");
				ImGui::CloseCurrentPopup();
			}
		}

		void ShowEntityHeader(Entity entity, bool& entityDeleted)
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
				ShowAddComponentMenuItems(entity);
				ImGui::EndPopup();
			}
		}

		void ShowComponents(Entity entity)
		{
			ShowComponent<TransformComponent>(ICON_FA_EXPAND_ARROWS_ALT " Transform", entity, [](TransformComponent& component)
				{
					UIControls::ShowXYZControl("Translation", component.Translation);
					glm::vec3 rotation = glm::degrees(component.Rotation);
					UIControls::ShowXYZControl("Rotation", rotation);
					component.Rotation = glm::radians(rotation);
					UIControls::ShowXYZControl("Scale", component.Scale, 1.0f);
				}, false);

			ShowComponent<CameraComponent>(ICON_FA_VIDEO " Camera", entity, [](CameraComponent& component)
				{
					auto& camera = component.Camera;

					UIControls::ShowBoolControl("Primary", &component.Primary);
					int projType = (int)camera.GetProjectionType();
					if (UIControls::ShowComboControl("Projection", { "Perspective", "Orthographic" }, projType))
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

			ShowComponent<SpriteRendererComponent>(ICON_FA_IMAGES " Sprite Renderer", entity, [](SpriteRendererComponent& component)
				{
					// TODO: Textures and materials
					UIControls::ShowColorControl("Color", component.Color);
					UIControls::ShowButtonControl("Texture", "Add Texture...", { 100, 0 });
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ML_PAYLOAD_TYPE_ASSET))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
							component.Texture = Texture2D::Create(texturePath.string());
						}
						ImGui::EndDragDropTarget();
					}
					UIControls::ShowFloatControl("Tiling Factor", &component.TilingFactor);
				});

			ShowComponent<RigidBody2DComponent>(ICON_FA_OBJECT_GROUP " Rigid Body 2D", entity, [](RigidBody2DComponent& component)
				{
					int bodyType = (int)component.Type;
					if (UIControls::ShowComboControl("Type", { "Static", "Dynamic", "Kinematic" }, bodyType))
						component.Type = (RigidBody2DComponent::BodyType)bodyType;
					UIControls::ShowBoolControl("Fixed Rotation", &component.FixedRotation);
				});

			ShowComponent<BoxCollider2DComponent>(ICON_FA_VECTOR_SQUARE " Box Collider 2D", entity, [](BoxCollider2DComponent& component)
				{
					UIControls::ShowXYControl("Offset", component.Offset);
					UIControls::ShowXYControl("Size", component.Size, 0.5f);
					UIControls::ShowFloatControl("Density", &component.Density, 1.0f, 0.01f, 0.0f, 1.0f);
					UIControls::ShowFloatControl("Friction", &component.Friction, 0.5f, 0.01f, 0.0f, 1.0f);
					UIControls::ShowFloatControl("Restitution", &component.Restitution, 0.0f, 0.01f, 0.0f, 1.0f);
					UIControls::ShowFloatControl("Restitution\nThreshold", &component.RestitutionThreshold, 0.5f, 0.01f, 0.0f);
				});
		}

		glm::vec3 GetAverageVector(const std::vector<glm::vec3>& vectors)
		{
			glm::vec3 result(0.0f);
			if (vectors.empty())
				return result;
			for (const glm::vec3& vec : vectors)
				result += vec;
			result /= vectors.size();
			return result;
		}

		template<typename Comp, typename UIFunction>
		void ShowComponent(const std::string& name, Entity entity, UIFunction uiFunction, bool deleteable)
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
				if (ImGui::Button(ICON_FA_SLIDERS_H, ImVec2{ lineHeight, lineHeight }))
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
	}
}
