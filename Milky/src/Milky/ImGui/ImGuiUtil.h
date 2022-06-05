#pragma once

#include "Milky/Core/Base.h"

#include <glm/glm.hpp>

#define ML_ENABLE_CUSTOM_FONT 1

struct ImFont;

namespace Milky {

	enum class GuiTheme
	{ ImGui = 0, Dark, Light, Orangensaft, _Count };
	enum class GuiFont
	{ ImGui = 0, Default, Header, Bold };

	class GuiThemeManager
	{
	public:
		static void ShowThemeMenu();
		static void LoadFonts();
		static void SetTheme(GuiTheme theme);
		static GuiTheme GetTheme() { return s_GuiTheme; };
		static std::string GetThemeName(GuiTheme theme);
		static ImFont* GetFont(GuiFont font);
	private:
		static void SetDarkTheme();
		static void SetLightTheme();
		static void SetOrangensaftTheme();
	public:
		static const GuiTheme s_DefaultGuiTheme = GuiTheme::Dark;
	private:
		static GuiTheme s_GuiTheme;
		static std::unordered_map<GuiFont, ImFont*> s_GuiFonts;
		static ImFont* s_IconFont;
	};

	namespace UIControls {

		template<typename DrawFunction>
		bool ShowControl(const std::string& label, DrawFunction drawFunction, float columnWidth = 100.0f);

		bool ShowComboControl(const std::string& label, std::vector<const char*> values, int& selected, float columnWidth = 100.0f);
		bool ShowBoolControl(const std::string& label, bool* value, float columnWidth = 100.0f);

		bool ShowFloatControl(const std::string& label, float* value, float resetValue = 0.0f, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.2f", const char* drilldownFormat = "%.6f", float columnWidth = 100.0f);
		bool ShowXYControl(const std::string& label, glm::vec2& values, float resetValue = 0.0f, const char* format = "%.2f", const char* drilldownFormat = "%.6f", float columnWidth = 100.0f);
		bool ShowXYZControl(const std::string& label, glm::vec3& values, float resetValue = 0.0f, const char* format = "%.2f", const char* drilldownFormat = "%.6f", float columnWidth = 100.0f);
		
		bool ShowColorControl(const std::string& label, glm::vec4& color, float columnWidth = 100.0f);

		bool ShowTextControl(const std::string& label, std::string& text, size_t maxLength = 128, float columnWidth = 100.0f);

		bool ShowButtonControl(const std::string& label, const std::string& buttonText, const glm::vec2& size = {0,0}, float columnWidth = 100.0f);
	}
}
