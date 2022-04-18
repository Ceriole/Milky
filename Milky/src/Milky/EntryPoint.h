#pragma once

#ifdef ML_PLATFORM_WINDOWS

extern Milky::Application* Milky::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Milky::CreateApplication();
	app->Run();
	delete app;
}

#else
#error Milky only supports Windows!
#endif