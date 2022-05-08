#include <Milky.h>
#include <Milky/Core/EntryPoint.h>

#include <Platform/OpenGL/OpenGLShader.h>

#include <imgui/imgui.h>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.h"

namespace Milky {

	class MilkGlass : public Application
	{
	public:
		MilkGlass()
			: Application("MilkGlass")
		{
			// PushLayer(new ExampleLayer());
			PushLayer(new EditorLayer());
		}

		~MilkGlass()
		{

		}
	};

	Application* CreateApplication()
	{
		return new MilkGlass();
	}

}
