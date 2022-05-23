#include "mlpch.h"
#include "ImGuiUtil.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include <IconsFontAwesome5.h>

namespace Milky {

	static ImFont* s_ThemeFonts[3];
	static ImFont* s_IconFont;

	GuiThemeManager::GuiTheme GuiThemeManager::s_GuiTheme = GuiTheme::ImGui;

	void GuiThemeManager::ShowThemeMenu()
	{
		if (ImGui::BeginMenuEx("Theme", ICON_FA_PALETTE))
		{
			for (int i = 0; i < (int)GuiTheme::_Count; i++)
			{
				GuiTheme theme = (GuiTheme)i;
				ImGui::BeginDisabled(theme == GuiTheme::Orangensaft);
				if (ImGui::MenuItem(GetThemeName(theme).c_str(), NULL, theme == GetTheme()))
						SetTheme(theme);
				if (theme == GuiTheme::Orangensaft && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
					ImGui::SetTooltip("Not Yet Implemented");
				ImGui::EndDisabled();
			}
			ImGui::EndMenu();
		}
	}

	void GuiThemeManager::LoadFonts()
	{
		ImGuiIO& io = ImGui::GetIO();

		constexpr float fontSize = 18.0f;

		// FontAwesome5 icons
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true; icons_config.PixelSnapH = true;
		// Default ImGui font
		s_ThemeFonts[0] = io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome5/" FONT_ICON_FILE_NAME_FAS, 13.0f, &icons_config, icons_ranges); // Merge icons into defualt
		// OpenSans
		s_ThemeFonts[1] = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);
		io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome5/" FONT_ICON_FILE_NAME_FAS, fontSize, &icons_config, icons_ranges); // Merge icons into OpenSans regular
		s_ThemeFonts[2] = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
		io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome5/" FONT_ICON_FILE_NAME_FAS, fontSize, &icons_config, icons_ranges); // Merge icons into OpenSans bold

	}

	void GuiThemeManager::SetTheme(GuiTheme theme)
	{
		if (theme == s_GuiTheme)
			return;
		s_GuiTheme = theme;
		auto& io = ImGui::GetIO();
		switch(s_GuiTheme)
		{
		default:
		case GuiTheme::ImGui:
			io.FontDefault = s_ThemeFonts[0];
			ImGui::StyleColorsDark();
			break;
		case GuiTheme::Dark:
			SetDarkTheme();
			break;
		case GuiTheme::Light:
			SetLightTheme();
			break;
		case GuiTheme::Orangensaft:
			SetOrangensaftTheme();
			break;
		}
	}

	std::string GuiThemeManager::GetThemeName(GuiTheme theme)
	{
		std::string themeName = "Unknown Theme";
		switch (theme)
		{
		case GuiTheme::ImGui:
			themeName = "ImGui Default";
			break;
		case GuiTheme::Dark:
			themeName = "Dark"; 
			break;
		case GuiTheme::Light:
			themeName = "Light";
			break;
		case GuiTheme::Orangensaft:
			themeName = "Orangensaft";
			break;
		}
		if (theme == s_DefaultGuiTheme)
			themeName.append(" (Default)");
		return themeName;
	}

	void GuiThemeManager::SetDarkTheme()
	{
		ImGui::StyleColorsDark();

		auto& io = ImGui::GetIO();
		io.FontDefault = s_ThemeFonts[1];

		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	void GuiThemeManager::SetLightTheme()
	{
		ImGui::StyleColorsLight();

		auto& io = ImGui::GetIO();
		io.FontDefault = s_ThemeFonts[1];

		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.972f, 0.984f, 0.984f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.556f, 0.945f, 0.611f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.811f, 0.988f, 0.839f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.247f, 0.913f, 0.345f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.556f, 0.945f, 0.611f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.811f, 0.988f, 0.839f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.247f, 0.913f, 0.345f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.901f, 0.937f, 0.937f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.811f, 0.988f, 0.839f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.247f, 0.913f, 0.345f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.556f, 0.945f, 0.611f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.811f, 0.988f, 0.839f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.247f, 0.913f, 0.345f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.254f, 0.725f, 0.254f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.274f, 0.866f, 0.274f, 1.0f };
	}

	void GuiThemeManager::SetOrangensaftTheme()
	{
		// TODO
	}

	namespace UIControls {

		template<typename DrawFunction>
		bool ShowControl(const std::string& label, DrawFunction drawFunction, float columnWidth)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2{ 0, 0 });
			ImGui::BeginTable(label.c_str(), 2);
			ImGui::TableSetupColumn((label + "Label").c_str(), ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, columnWidth);
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text(label.c_str());
			ImGui::TableNextColumn();

			bool edited = drawFunction();

			ImGui::EndTable();
			ImGui::PopStyleVar();
			return edited;
		}

		bool ShowColorControl(const std::string& label, glm::vec4& color, float columnWidth)
		{
			return ShowControl(label, [&]() -> bool
				{
					return ImGui::ColorEdit4("##Color", glm::value_ptr(color));
				}, columnWidth);
		}

		bool ShowBoolControl(const std::string& label, bool* value, float columnWidth)
		{
			return ShowControl(label, [&]() -> bool
				{
					return ImGui::Checkbox("##Boolean", value);
				}, columnWidth);
		}

		bool ShowFloatControl(const std::string& label, float* value, float resetValue, float columnWidth, const char* format, const char* drilldownFormat)
		{
			return ShowControl(label, [&]() -> bool
				{
					bool edited = ImGui::DragFloat("##Value", value, 0.1f, 0.0f, 0.0f, format);
					if (ImGui::BeginPopupContextItem("Reset"))
					{
						ImGui::SetNextItemWidth(100);
						edited |= ImGui::InputFloat("##X", value, 0.0f, 0.0f, drilldownFormat);
						if (ImGui::MenuItem("Reset"))
						{ *value = resetValue; edited = true; }
						ImGui::EndPopup();
					}
					return edited;
				}, columnWidth);
		}

		bool ShowComboControl(const std::string& label, std::vector<std::string> values, int& selected, float columnWidth)
		{
			return ShowControl(label, [&]() -> bool
				{
					bool edited = false;
					if (ImGui::BeginCombo("##Combo", values[selected].c_str()))
					{
						for (int i = 0; i < values.size(); i++)
						{
							bool isSelected = selected == i;
							if (ImGui::Selectable(values[i].c_str(), isSelected))
							{
								selected = i;
								edited = true;
							}

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}
					return edited;
				}, columnWidth);
		}

		bool ShowXYZControl(const std::string& label, glm::vec3& values, float resetValue, float columnWidth, const char* format, const char* drilldownFormat)
		{
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			return ShowControl(label, [&]() -> bool
				{
					bool edited = false;

					ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

					float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
					ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
					if (ImGui::Button("X", buttonSize))
					{ values.x = resetValue; edited = true; }
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Reset X");
					ImGui::OpenPopupContextItem("XEditPopup");
					ImGui::PopStyleColor(3);

					ImGui::SameLine();
					edited |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, format);
					ImGui::OpenPopupContextItem("XEditPopup");
					ImGui::PopItemWidth();
					ImGui::SameLine();

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
					if (ImGui::Button("Y", buttonSize))
					{ values.y = resetValue; edited = true; }
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Reset Y");
					ImGui::OpenPopupContextItem("YEditPopup");
					ImGui::PopStyleColor(3);

					ImGui::SameLine();
					edited |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, format);
					ImGui::OpenPopupContextItem("YEditPopup");
					ImGui::PopItemWidth();
					ImGui::SameLine();

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
					if (ImGui::Button("Z", buttonSize))
					{ values.z = resetValue; edited = true; }
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Reset Z");
					ImGui::OpenPopupContextItem("ZEditPopup");
					ImGui::PopStyleColor(3);

					ImGui::SameLine();
					edited |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, format);
					ImGui::OpenPopupContextItem("ZEditPopup");
					ImGui::PopItemWidth();

					if (ImGui::BeginPopup("XEditPopup"))
					{
						ImGui::SetNextItemWidth(100);
						edited |= ImGui::InputFloat("##X", &values.x, 0.0f, 0.0f, drilldownFormat);
						if (ImGui::MenuItem("Reset All"))
						{ values = glm::vec3(resetValue); edited = true; }
						ImGui::EndPopup();
					}
					if (ImGui::BeginPopup("YEditPopup"))
					{
						ImGui::SetNextItemWidth(100);
						edited |= ImGui::InputFloat("##Y", &values.y, 0.0f, 0.0f, drilldownFormat);
						if (ImGui::MenuItem("Reset All"))
						{ values = glm::vec3(resetValue); edited = true; }
						ImGui::EndPopup();
					}
					if (ImGui::BeginPopup("ZEditPopup"))
					{
						ImGui::SetNextItemWidth(100);
						edited |= ImGui::InputFloat("##Z", &values.z, 0.0f, 0.0f, drilldownFormat);
						if (ImGui::MenuItem("Reset All"))
						{ values = glm::vec3(resetValue); edited = true; }
						ImGui::EndPopup();
					}

					ImGui::PopStyleVar();

					return edited;
				}, columnWidth);
		}

	}
}
