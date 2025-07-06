#pragma once

#include <iostream>
#include <string_view>
#include <chrono>
#include <format>
#ifdef _WIN32
#define NOMINMAX
	#include <Windows.h>
#endif

#define LOG_INFO(...)	Logger::Instance().Info(__VA_ARGS__)
#define LOG_WARN(...)	Logger::Instance().Warn(__VA_ARGS__)
#define LOG_ERROR(...)	Logger::Instance().Error(__VA_ARGS__)

class Logger
{
public:
	static Logger& Instance();

	static void Init();
	
	void SetOutputStream(std::ostream& out) { m_OutStream = &out; }

	template <typename... Args>
	void Info(std::string_view format, Args&&... args);

	template <typename... Args>
	void Warn(std::string_view format, Args&&... args);

	template <typename... Args>
	void Error(std::string_view format, Args&&... args);
private:
	Logger() = default;

	template <typename... Args>
	void Log(std::string_view level, std::string_view format, Args&&... args);

	std::ostream* m_OutStream{&std::cout};
};

inline Logger& Logger::Instance()
{
	static Logger logger{};
	return logger;
}

inline void Logger::Init()
{
// Allows for colored text through escape sequences on Windows 10 and 11
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hConsole, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hConsole, dwMode);
#endif
}

template <typename... Args>
inline void Logger::Log(std::string_view level, std::string_view format, Args&&... args)
{
	static constexpr std::string_view k_LogFormat = "[{:%T}] [{}]: {}\n";

	auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());

	if constexpr (sizeof...(args) > 0)
	{
		(*m_OutStream) << std::format(k_LogFormat, time, level, std::vformat(format, std::make_format_args(std::forward<Args>(args)...)));
	}
	else
	{
		(*m_OutStream) << std::format(k_LogFormat, time, level, format);
	}
}

template <typename... Args>
inline void Logger::Info(std::string_view format, Args&&... args)
{
	Log("\033[32minfo\033[0m", format, std::forward<Args>(args)...); // Green
}

template <typename... Args>
inline void Logger::Warn(std::string_view format, Args&&... args) {
	Log("\033[33mwarn\033[0m", format, std::forward<Args>(args)...); // Yellow
}

template <typename... Args>
inline void Logger::Error(std::string_view format, Args&&... args) {
	Log("\033[31merror\033[0m", format, std::forward<Args>(args)...); // Red
}