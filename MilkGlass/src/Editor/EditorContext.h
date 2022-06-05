#pragma once

#include "Milky/Renderer/Framebuffer.h"
#include "Milky/Scene/Scene.h"

#include "Editor/SelectionContext.h"

#include <filesystem>

#define MILKY_SCENE_FILE_EXT		".mlscn"
#define MILKY_SCENE_FILE_DECRIPTION "Milky Scene (*" MILKY_SCENE_FILE_EXT ")"
#define MILKY_SCENE_FILE_FILTER		MILKY_SCENE_FILE_DECRIPTION "\0*" MILKY_SCENE_FILE_EXT "\0"

namespace Milky {

	enum class EditorState
	{
		Edit = 0,
		Play = 1,
		Pause = 2,
		Simulate = 3
	};

	struct EditorContext
	{
	public:
		Ref<Scene> ActiveScene;
		Ref<Scene> EditorScene, RuntimeScene;
		Ref<SelectionContext> Selection;
		Ref<EditorCamera> Camera;
		Ref<Framebuffer> Framebuffer;
		Entity HoveredEntity;
		EditorState State;
		int GizmoType = -1;
		int GizmoMode = 0;

	public:
		void NewScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene(const std::filesystem::path& path = std::string());
		void SetActiveFilepath(const std::filesystem::path& path);
		void OpenSceneDialog();
		void SaveSceneDialog();

		void OnScenePlay();
		void OnSceneStop();

		bool Select(Entity uuid);
		
		const std::filesystem::path ActivePath() const { return m_ActivePath; };
		const std::vector<std::filesystem::path> RecentPaths() const { return m_RecentPaths; };
		const glm::vec2 ViewportSize() const { return m_ViewportSize; };
		void SetViewportSize(const glm::vec2& viewportSize);

		SelectionType GetTypeOfUUID(UUID uuid) const;
		bool IsSameType(const std::vector<UUID>& uuids) const;
	private:
		void SetScene(Ref<Scene> scene);
	private:
		std::vector<std::filesystem::path> m_RecentPaths;
		std::filesystem::path m_ActivePath;
		glm::vec2 m_ViewportSize = { 0, 0 };
		int m_TempGizmoType = GizmoType;
	};

}
