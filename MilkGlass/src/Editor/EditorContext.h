#pragma once

#include "Milky/Renderer/Framebuffer.h"
#include "Milky/Scene/Scene.h"

#include "Editor/SelectionContext.h"

#include <filesystem>

#define MILKY_SCENE_FILE_EXT		".mlscn"
#define MILKY_SCENE_FILE_DECRIPTION "Milky Scene (*" MILKY_SCENE_FILE_EXT ")"
#define MILKY_SCENE_FILE_FILTER		MILKY_SCENE_FILE_DECRIPTION "\0*" MILKY_SCENE_FILE_EXT "\0"

namespace Milky {

	struct EditorContext
	{
	public:
		Ref<Scene> ActiveScene;
		Ref<SelectionContext> Selection;
		Ref<EditorCamera> Camera;
		Ref<Framebuffer> Framebuffer;
		Entity HoveredEntity;

	public:
		void CreateEmptyScene();
		void NewScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene(const std::filesystem::path& path = std::string());
		void SetActiveFilepath(const std::filesystem::path& path);
		void OpenSceneDialog();
		void SaveSceneDialog();

		bool SelectEntity(Entity entity);
		
		const std::filesystem::path ActivePath() const { return m_ActivePath; };
		const std::vector<std::filesystem::path> RecentPaths() const { return m_RecentPaths; };
		const glm::vec2 ViewportSize() const { return m_ViewportSize; };
		void SetViewportSize(const glm::vec2& viewportSize);
	private:
		std::vector<std::filesystem::path> m_RecentPaths;
		std::filesystem::path m_ActivePath;
		glm::vec2 m_ViewportSize = { 0, 0 };
	};

}
