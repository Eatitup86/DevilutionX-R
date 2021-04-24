#pragma once

#include "fmt/core.h"
#include "fmt/ranges.h"

#include <SDL.h>

#ifdef USE_SDL1
#include "sdl2_to_1_2_backports.h"
#endif

namespace devilution {

enum class LogCategory {
	Application = SDL_LOG_CATEGORY_APPLICATION,
	Error = SDL_LOG_CATEGORY_ERROR,
	Assert = SDL_LOG_CATEGORY_ASSERT,
	System = SDL_LOG_CATEGORY_SYSTEM,
	Audio = SDL_LOG_CATEGORY_AUDIO,
	Video = SDL_LOG_CATEGORY_VIDEO,
	Render = SDL_LOG_CATEGORY_RENDER,
	Input = SDL_LOG_CATEGORY_INPUT,
	Test = SDL_LOG_CATEGORY_TEST,
};

enum class LogPriority {
    Verbose = SDL_LOG_PRIORITY_VERBOSE,
    Debug = SDL_LOG_PRIORITY_DEBUG,
    Info = SDL_LOG_PRIORITY_INFO,
    Warn = SDL_LOG_PRIORITY_WARN,
    Error = SDL_LOG_PRIORITY_ERROR,
    Critical = SDL_LOG_PRIORITY_CRITICAL,
};

template <typename... Args>
void Log(const char *fmt, Args &&... args)
{
	auto str = fmt::format(fmt, std::forward<Args>(args)...);
	SDL_Log("%s", str.c_str());
}

template <typename... Args>
void LogVerbose(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = fmt::format(fmt, std::forward<Args>(args)...);
	SDL_LogVerbose(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogDebug(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = fmt::format(fmt, std::forward<Args>(args)...);
	SDL_LogDebug(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogInfo(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = fmt::format(fmt, std::forward<Args>(args)...);
	SDL_LogInfo(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogWarn(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = fmt::format(fmt, std::forward<Args>(args)...);
	SDL_LogWarn(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogError(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = fmt::format(fmt, std::forward<Args>(args)...);
	SDL_LogError(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogCritical(LogCategory category, const char *fmt, Args &&... args)
{
	auto str = fmt::format(fmt, std::forward<Args>(args)...);
	SDL_LogCritical(static_cast<int>(category), "%s", str.c_str());
}

template <typename... Args>
void LogMessageV(LogCategory category, LogPriority priority, const char *fmt, Args &&... args)
{
	auto str = fmt::format(fmt, std::forward<Args>(args)...);
	SDL_LogMessageV(static_cast<int>(category), static_cast<SDL_LogPriority>(priority), "%s", str.c_str());
}

} // namespace devilution
