#pragma once

#include "Milky/Core/Window.h"
#include "Milky/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Milky {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline void SetEveventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; };

		virtual std::string GetTitle() override;
		virtual void SetTitle(std::string title) override;

		virtual bool HasFocus() override;
		virtual void RequestFocus() override;
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		Scope<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string Title;
			uint32_t Width, Height;
			bool VSync, Focused;

			EventCallbackFn EventCallback;
		};
		
		WindowData m_Data;
	};

}
