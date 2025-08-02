#pragma once

#include <limits>
#include <format>
#include <string>

// Per second debug state, to be created as a global and accessed throughout the engine
struct DebugState
{
	int Remeshes = 0;
	int Loaded = 0;
	int DrawCalls = 0;
	int Frames = 0;
	int Ticks = 0;

	void Reset()
	{
		Remeshes = 0;
		Loaded = 0;
		DrawCalls = 0;
		Frames = 0;
		Ticks = 0;
	}
};

template <>
struct std::formatter<DebugState>
{
	constexpr auto parse(std::format_parse_context& ctx) const { return ctx.begin(); }

	auto format(const DebugState& debugState, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), 
			"Debug Info:\nRemeshed chunks: {}\nLoaded chunks: {}\nDraw calls: {}\nFPS: {}\nTPS: {}", 
			debugState.Remeshes, debugState.Loaded, debugState.DrawCalls, debugState.Frames, debugState.Ticks
		);
	}
};
