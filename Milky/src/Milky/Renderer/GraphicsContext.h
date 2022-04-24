#pragma once

namespace Milky {

	class GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};

}