#pragma once
#include "Milky/Core/Base.h"
#include "Milky/Core/Application.h"

#ifdef ML_PLATFORM_WINDOWS

extern Milky::Application* Milky::CreateApplication();

int main(int argc, char** argv)
{
	Milky::Log::Init();

	ML_PROFILE_BEGIN_SESSION("Startup", "MilkyProfile-Startup.json");
	auto app = Milky::CreateApplication();
	ML_PROFILE_END_SESSION();
	
	ML_PROFILE_BEGIN_SESSION("Runtime", "MilkyProfile-Runtime.json");
	app->Run();
	ML_PROFILE_END_SESSION();

	ML_PROFILE_BEGIN_SESSION("Shutdown", "MilkyProfile-Shutdown.json");
	delete app;
	ML_PROFILE_END_SESSION();
}

#else
	#error Milky only supports Windows!
#endif
