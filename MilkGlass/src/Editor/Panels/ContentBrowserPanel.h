#pragma once

#include "EditorPanel.h"
#include "Milky/Renderer/Texture.h"

namespace Milky {

	// TODO: Set by project
	constexpr char* s_AssetDirectory = "assets";

	class ContentBrowserPanel : public EditorPanel
	{
	public:
		ContentBrowserPanel(const Ref<EditorData>& context, const std::string& title, const std::string& icon, const std::string& shortcut);
	protected:
		virtual void ShowContent() override;
	private:
		Ref<Texture2D> m_DirectoryIcon, m_FileIcon;
		std::filesystem::path m_AssetPath = std::filesystem::path(s_AssetDirectory);
		std::filesystem::path m_CurrentDirectory;
	};

}
