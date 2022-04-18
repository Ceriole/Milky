#pragma once

#ifdef ML_PLATFORM_WINDOWS 
	#ifdef ML_BUILD_DLL
		#define MILKY_API __declspec(dllexport)
	#else
		#define MILKY_API __declspec(dllimport)
	#endif
#else
	#error Milky only supports Windows!
#endif

#define BIT(x) (1 << x)