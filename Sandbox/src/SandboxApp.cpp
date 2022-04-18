#include <Milky.h>

class SandboxApp : public Milky::Application
{
public:
	SandboxApp()
	{

	}

	~SandboxApp()
	{

	}
};

Milky::Application* Milky::CreateApplication()
{
	return new SandboxApp();
}
