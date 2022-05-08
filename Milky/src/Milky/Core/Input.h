#pragma once

#include "Milky/Core/Base.h"
#include "Milky/Core/KeyCodes.h"
#include "Milky/Core/MouseCodes.h"

namespace Milky {

	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);

		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}
