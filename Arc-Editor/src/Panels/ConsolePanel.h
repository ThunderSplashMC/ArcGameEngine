#pragma once

#include <imgui/imgui.h>

#include "BasePanel.h"

namespace ArcEngine
{
	class ConsolePanel : public BasePanel
	{
	public:
		struct Message
		{
			uint32_t ID;
			std::string Buffer;
			std::string Filepath;
			std::string Function;
			int32_t Line;
			Log::Level Level;

			Message(uint32_t id, std::string_view message, const char* filepath, const char* function, int32_t line, Log::Level level = Log::Level::Trace);
			void OnImGuiRender() const;

			static const char* GetLevelName(Log::Level level);
			static glm::vec4 GetRenderColor(Log::Level level);
			static const char8_t* GetLevelIcon(Log::Level level);
		};

	public:
		explicit ConsolePanel(const char* name = "Console");
		~ConsolePanel() override;

		ConsolePanel(const ConsolePanel& other) = delete;
		ConsolePanel(ConsolePanel&& other) = delete;
		ConsolePanel& operator=(const ConsolePanel& other) = delete;
		ConsolePanel& operator=(ConsolePanel&& other) = delete;

		void AddMessage(std::string_view message, const char* filepath, const char* function, int32_t line, Log::Level level);
		[[nodiscard]] const Message* GetRecentMessage() const;
		void Clear();
		void SetFocus() const;

		void OnImGuiRender() override;

	private:
		void ImGuiRenderHeader();
		void ImGuiRenderSettings();
		void ImGuiRenderMessages();

	private:
		float m_DisplayScale = 1.0f;
		uint16_t m_Capacity = 200;
		uint16_t m_BufferSize = 0;
		uint16_t m_BufferBegin = 0;
		uint32_t s_MessageBufferRenderFilter = Log::Level::Trace;
		bool m_AllowScrollingToBottom = true;
		bool m_RequestScrollToBottom = false;
		std::vector<Ref<Message>> m_MessageBuffer;
		ImGuiTextFilter m_Filter;
	};
}
