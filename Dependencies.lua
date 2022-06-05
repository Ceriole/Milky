
-- Milky dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}

IncludeDir["yaml_cpp"] = "%{wks.location}/Milky/vendor/yaml-cpp/include"

IncludeDir["entt"] = "%{wks.location}/Milky/vendor/entt/include"
IncludeDir["Box2D"] = "%{wks.location}/Milky/vendor/Box2D/include"

IncludeDir["ImGui"] = "%{wks.location}/Milky/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Milky/vendor/ImGuizmo"
IncludeDir["IconFonts"] = "%{wks.location}/Milky/vendor/IconFonts"

IncludeDir["stb_image"] = "%{wks.location}/Milky/vendor/stb_image"
IncludeDir["GLFW"] = "%{wks.location}/Milky/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Milky/vendor/Glad/include"
IncludeDir["glm"] = "%{wks.location}/Milky/vendor/glm"

IncludeDir["shaderc"] = "%{wks.location}/Milky/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Milky/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/Milky/vendor/VulkanSDK/Lib"
LibraryDir["VulkanSDK_DebugDLL"] = "%{wks.location}/Milky/vendor/VulkanSDK/Bin"

Library = {}

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
