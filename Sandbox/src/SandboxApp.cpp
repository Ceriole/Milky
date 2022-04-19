#include <Milky.h>
#include <Milky/EntryPoint.h>

class ExampleLayer : public Milky::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		ML_INFO("ExampleLayer::OnUpdate");
	}

	void OnEvent(Milky::Event& event) override
	{
		ML_TRACE("ExampleLayer::OnEvent({0})", event);
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
