#pragma once
#include "Milky/Core/Core.h"
#include "Milky/Core/Application.h"

#ifdef ML_PLATFORM_WINDOWS

extern Milky::Application* Milky::CreateApplication();

int main(int argc, char** argv)
{
	Milky::Log::Init();
	auto app = Milky::CreateApplication();
	app->Run();
	delete app;
}

#else
#error Milky only supports Windows!
#endif
