#pragma once

#include "Milky/Core/Base.h"

#include <glm/glm.hpp>

#define ML_ENABLE_CUSTOM_FONT 1

namespace Milky {

	class GuiThemeManager
	{
	public:
		enum class GuiTheme
		{ ImGuiDefault = 0, Dark, Light, Orangensaft, _Count };
	public:
		static void ShowThemeMenu();
		static void LoadFonts();
		static void SetTheme(GuiTheme theme);
		static GuiTheme GetTheme() { return s_GuiTheme; };
		static std::string GetThemeName(GuiTheme theme);
	private:
		static void SetDarkTheme();
		static void SetLightTheme();
		static void SetOrangensaftTheme();
	private:
		static GuiTheme s_GuiTheme;
	};

	template<typename DrawFunction>
	bool DrawControl(const std::string& label, DrawFunction drawFunction, float columnWidth = 100.0f);

	bool DrawComboControl(const std::string& label, std::vector<std::string> values, int& selected, float columnWidth = 100.0f);
	bool DrawBoolControl(const std::string& label, bool* value, float columnWidth = 100.0f);
	
	bool DrawFloatControl(const std::string& label, float* value, float resetValue = 0.0f, float columnWidth = 100.0f, const char* format = "%.2f");
	bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f, const char* format = "%.2f");
	
	bool DrawColorControl(const std::string& label, glm::vec4& color, float columnWidth = 100.0f);
	
}
