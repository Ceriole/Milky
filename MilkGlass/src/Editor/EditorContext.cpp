#include "EditorContext.h"

#include "Milky/Core/Application.h"
#include "Milky/Core/Input.h"
#include "Milky/Utils/PlatformUtils.h"
#include "Milky/Scene/SceneSerializer.h"

namespace Milky {

	void EditorContext::SetScene(Ref<Scene> scene)
	{
		EditorScene = scene;
		Camera = CreateRef<EditorCamera>(30.0f, 1.78f, 0.1f, 1000.0f);
		Selection->Clear();
		HoveredEntity = {};
		State = EditorState::Edit;
		GizmoType = -1;
		GizmoMode = 0;

		if (m_ViewportSize.x && m_ViewportSize.y)
		{
			EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			Camera->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}

		SetActiveFilepath("");

		ActiveScene = scene;
	}

	void EditorContext::NewScene()
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		SetScene(newScene);
	}

	void EditorContext::OpenScene(const std::filesystem::path& path)
	{
		if (!path.empty())
		{
			if (path.extension().string() != MILKY_SCENE_FILE_EXT)
			{
				ML_WARN("Could not load '{0}' - Not a scene file.", path.filename().string());
				return;
			}
			Ref<Scene> newScene = CreateRef<Scene>();
			SceneSerializer serializer(newScene);
			if (serializer.Deserialize(path.string()))
			{
				SetScene(newScene);
				SetActiveFilepath(path);
			}
		}
	}

	void EditorContext::SaveScene(const std::filesystem::path& path)
	{
		if (State != EditorState::Edit)
			return;
		SceneSerializer serializer(ActiveScene);
		if (!path.empty())
		{
			serializer.Serialize(path.string());
			SetActiveFilepath(path);
		}
		else if (!m_ActivePath.empty())
		{
			serializer.Serialize(m_ActivePath.string());
			SetActiveFilepath(m_ActivePath);
		}
		else
		{
			std::filesystem::path newFilepath = FileDialogs::Save(MILKY_SCENE_FILE_FILTER);
			serializer.Serialize(newFilepath.string());
			SetActiveFilepath(newFilepath);
		}
	}

	void EditorContext::OpenSceneDialog()
	{
		std::filesystem::path filepath = FileDialogs::Open(MILKY_SCENE_FILE_FILTER);
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorContext::SaveSceneDialog()
	{
		std::string filepath = FileDialogs::Save(MILKY_SCENE_FILE_FILTER);
		if (!filepath.empty())
			SaveScene(filepath);
	}

	void EditorContext::SetActiveFilepath(const std::filesystem::path& path)
	{
		if (!m_ActivePath.empty())
		{
			m_RecentPaths.erase(std::remove(m_RecentPaths.begin(), m_RecentPaths.end(), m_ActivePath), m_RecentPaths.end());
			m_RecentPaths.insert(m_RecentPaths.begin(), m_ActivePath.string());
		}
		m_ActivePath = path.string();
		Application::Get().GetWindow().SetTitle("MilkGlass - " + (m_ActivePath.empty() ? "Unsaved Scene" : m_ActivePath.string()));
	}

	bool EditorContext::Select(Entity entity)
	{
		if (Input::IsCtrlDown())
		{
			if (entity)
				return Selection->Toggle(entity.GetUUID());
		}
		else
		{
			if (entity)
				return Selection->Set(entity.GetUUID());
			else
				Selection->Clear();
		}
		return false;
	}

	void EditorContext::SetViewportSize(const glm::vec2& viewportSize)
	{
		if (m_ViewportSize == viewportSize)
			return;
		m_ViewportSize = viewportSize;
		if (Milky::FramebufferSpecification spec = Framebuffer->GetSpecification(); m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			Camera->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
	}

	void EditorContext::OnScenePlay()
	{
		m_TempGizmoType = GizmoType;
		GizmoType = -1;
		
		State = EditorState::Play;

		ActiveScene = Scene::Copy(EditorScene);
		ActiveScene->OnRuntimeStart();
	}

	void EditorContext::OnSceneStop()
	{
		ActiveScene->OnRuntimeStop();
		ActiveScene = EditorScene;
		
		State = EditorState::Edit;
		GizmoType = m_TempGizmoType;
	}

	SelectionType EditorContext::GetTypeOfUUID(UUID uuid) const
	{
		if (ActiveScene->GetEntity(uuid)) return SelectionType::Entity;
		// TODO: Other selection types
		return SelectionType::None;
	}

	bool EditorContext::IsSameType(const std::vector<UUID>& uuids) const
	{
		if(uuids.empty())
			return false;
		SelectionType type = SelectionType::None;
		for (UUID uuid : uuids)
		{
			if (type == SelectionType::None)
				type = GetTypeOfUUID(uuid);
			else
				if (GetTypeOfUUID(uuid) != type)
					return false;
		}
		return true;
	}

}
