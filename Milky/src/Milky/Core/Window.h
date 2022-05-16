#pragma once

#include "mlpch.h"

#include "Milky/Core/Base.h"
#include "Milky/Events/Event.h"

namespace Milky {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width, Height;
		bool DefaultMaximized;

		WindowProps(const std::string& title = "Milky Engine",
						uint32_t width = 1280,
						uint32_t height = 720, bool defaultMaximized = true)
			: Title(title), Width(width), Height(height), DefaultMaximized(defaultMaximized)
		{}
	};

	// Interface for a window on a desktop app
	class  Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attribs
		virtual void SetEveventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};

}
