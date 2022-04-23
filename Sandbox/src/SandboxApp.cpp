#include <Milky.h>
#include <Milky/EntryPoint.h>

#include <imgui/imgui.h>

class ExampleLayer : public Milky::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{

	}

	void OnUpdate() override
	{
		if (Milky::Input::IsKeyPressed(ML_KEY_TAB))
			ML_TRACE("Tab is pressed!");
	}

	void OnEvent(Milky::Event& event) override
	{
		
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}
};

class SandboxApp : public Milky::Application
{
public:
	SandboxApp()
	{
		PushLayer(new ExampleLayer());
	}

	~SandboxApp()
	{

	}
};

Milky::Application* Milky::CreateApplication()
{
	return new SandboxApp();
}
