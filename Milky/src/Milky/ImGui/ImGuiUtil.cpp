#include "mlpch.h"
#include "ImGuiUtil.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace Milky {

	static ImFont* s_ThemeFonts[3];

	GuiThemeManager::GuiTheme GuiThemeManager::s_GuiTheme = GuiTheme::ImGuiDefault;

	void GuiThemeManager::ShowThemeMenu()
	{
		if (ImGui::BeginMenu("Theme"))
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
		s_ThemeFonts[0] = io.Fonts->AddFontDefault();
		s_ThemeFonts[1] = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);
		s_ThemeFonts[2] = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
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
		case GuiTheme::ImGuiDefault:
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
		switch (theme)
		{
		case GuiTheme::ImGuiDefault:
			return "ImGui Default";
		case GuiTheme::Dark:
			return "Dark";
		case GuiTheme::Light:
			return "Light";
		case GuiTheme::Orangensaft:
			return "Orangensaft";
		}
		return "Unknown Theme";
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

	template<typename DrawFunction>
	bool DrawControl(const std::string& label, DrawFunction drawFunction, float columnWidth)
	{
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		
		bool edited = drawFunction();

		ImGui::Columns(1);
		ImGui::PopID();

		return edited;
	}

	bool DrawColorControl(const std::string& label, glm::vec4& color, float columnWidth)
	{
		return DrawControl(label, [&]() -> bool
			{
				return ImGui::ColorEdit4("##Color", glm::value_ptr(color));
			}, columnWidth);
	}

	bool DrawBoolControl(const std::string& label, bool* value, float columnWidth)
	{
		return DrawControl(label, [&]() -> bool
			{
				return ImGui::Checkbox("##Boolean", value);
			}, columnWidth);
	}

	bool DrawFloatControl(const std::string& label, float* value, float resetValue, float columnWidth, const char* format)
	{
		return DrawControl(label, [&]() -> bool
			{
				return ImGui::DragFloat("##Value", value, 0.1f, 0.0f, 0.0f, format);
			}, columnWidth);
	}

	bool DrawComboControl(const std::string& label, std::vector<std::string> values, int& selected, float columnWidth)
	{
		return DrawControl(label, [&]() -> bool
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

	bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth, const char* format)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		return DrawControl(label, [&]() -> bool
			{
				ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

				bool edited = false;

				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
				if (ImGui::Button("X", buttonSize))
				{ values.x = resetValue; edited = true; }
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				edited |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, format);
				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
				if (ImGui::Button("Y", buttonSize))
				{ values.y = resetValue; edited = true; }
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				edited |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, format);
				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				if (ImGui::Button("Z", buttonSize))
				{ values.z = resetValue; edited = true; }
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				edited |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, format);
				ImGui::PopItemWidth();

				ImGui::PopStyleVar();

				return edited;
			}, columnWidth);
	}

}
