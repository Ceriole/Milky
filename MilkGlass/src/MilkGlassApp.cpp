#include <Milky.h>
#include <Milky/Core/EntryPoint.h>

#include <Platform/OpenGL/OpenGLShader.h>

#include <imgui/imgui.h>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Editor/EditorLayer.h"

namespace Milky {

	class MilkGlass : public Application
	{
	public:
		MilkGlass(ApplicationCommandLineArgs args)
			: Application("MilkGlass", args)
		{
			PushLayer(new EditorLayer());
		}

		~MilkGlass()
		{

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new MilkGlass(args);
	}

}
