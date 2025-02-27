#pragma once

#include "Arc/Core/Base.h"
#include "Arc/Events/Event.h"

namespace ArcEngine
{
	using WindowHandle = void*;

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Arc Engine", uint32_t width = 1600, uint32_t height = 900)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;
		[[nodiscard]] virtual uint32_t GetWidth() const = 0;
		[[nodiscard]] virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallBack(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		[[nodiscard]] virtual bool IsVSync() const = 0;

		[[nodiscard]] virtual bool IsMaximized() = 0;
		virtual void Minimize() = 0;
		virtual void Maximize() = 0;
		virtual void Restore() = 0;
		virtual void RegisterOverTitlebar(bool value) = 0;

		[[nodiscard]] virtual WindowHandle GetNativeWindow() const = 0;
		
		[[nodiscard]] static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}
