#include "EditorContext.h"

#include "Milky/Core/Application.h"
#include "Milky/Core/Input.h"
#include "Milky/Utils/PlatformUtils.h"
#include "Milky/Scene/SceneSerializer.h"

namespace Milky {

	void EditorContext::CreateEmptyScene()
	{
		ActiveScene = CreateRef<Scene>();
		Camera = CreateRef<EditorCamera>(30.0f, 1.78f, 0.1f, 1000.0f);
		Selection = CreateRef<SelectionContext>();
		HoveredEntity = {};

		if (m_ViewportSize.x && m_ViewportSize.y)
		{
			ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			Camera->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}
	}

	void EditorContext::NewScene()
	{
		CreateEmptyScene();
		SetActiveFilepath("");
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
			CreateEmptyScene();
			SceneSerializer serializer(ActiveScene);
			if (serializer.Deserialize(path.string()))
				SetActiveFilepath(path);
		}
	}

	void EditorContext::SaveScene(const std::filesystem::path& path)
	{
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

	bool EditorContext::SelectEntity(Entity entity)
	{
		if (Input::IsCtrlDown())
		{
			if (entity && Selection->CanSelect(SelectionType::Entity))
				return Selection->Toggle(entity);
		}
		else
		{
			if (entity)
				return Selection->Set(entity);
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
}
