#pragma once

#ifdef ML_PLATFORM_WINDOWS

extern Milky::Application* Milky::CreateApplication();

int main(int argc, char** argv)
{
	Milky::Log::Init();
	ML_CORE_WARN("Initialized Logging!");
	ML_INFO("Hello world!");
	auto app = Milky::CreateApplication();
	app->Run();
	delete app;
}

#else
#error Milky only supports Windows!
#endif