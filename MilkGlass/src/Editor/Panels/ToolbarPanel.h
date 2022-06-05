#pragma once

#include "EditorPanel.h"

#include "Milky/Renderer/Texture.h"

namespace Milky {

	class ToolbarPanel : public EditorPanel
	{
	public:
		ToolbarPanel(const Ref<EditorContext>& context, const std::string& title, const std::string& icon, const std::string& shortcut);
	protected:
		virtual void ShowContent() override;
		virtual int GetWindowFlags() override;
	private:
		Ref<Texture2D> m_IconPlay, m_IconStop, m_IconPause;
	};

}
