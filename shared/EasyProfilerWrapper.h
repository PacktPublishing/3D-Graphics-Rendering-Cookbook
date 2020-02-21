#pragma once

#if BUILD_WITH_EASY_PROFILER && BUILD_WITH_OPTICK
#	error Cannot enable both profilers at once. Just pick one.
#endif // BUILD_WITH_EASY_PROFILER && BUILD_WITH_OPTICK

#if !BUILD_WITH_EASY_PROFILER && !BUILD_WITH_OPTICK
#	define EASY_FUNCTION(...)
#	define EASY_BLOCK(...)
#	define EASY_END_BLOCK
#	define EASY_THREAD_SCOPE(...)
#	define EASY_PROFILER_ENABLE
#	define EASY_MAIN_THREAD
#	define PROFILER_FRAME(...)
#	define PROFILER_DUMP(fileName)
#endif // !BUILD_WITH_EASY_PROFILER && !BUILD_WITH_OPTICK

#if BUILD_WITH_EASY_PROFILER
#	include "easy/profiler.h"
#	define PROFILER_FRAME(...)
#	define PROFILER_DUMP(fileName) profiler::dumpBlocksToFile(fileName);
#endif // BUILD_WITH_EASY_PROFILER

#if BUILD_WITH_OPTICK
#	include "optick.h"
#	define EASY_FUNCTION(...) OPTICK_EVENT()
#	define EASY_BLOCK(name, ...) { OptickScopeWrapper Wrapper(name);
#	define EASY_END_BLOCK };
#	define EASY_THREAD_SCOPE(...) OPTICK_START_THREAD(__VA_ARGS__)
#	define EASY_PROFILER_ENABLE OPTICK_START_CAPTURE()
#	define EASY_MAIN_THREAD OPTICK_THREAD( "MainThread" )
#	define PROFILER_FRAME(name) OPTICK_FRAME(name)
#	define PROFILER_DUMP(fileName) OPTICK_STOP_CAPTURE(); OPTICK_SAVE_CAPTURE(fileName);

namespace profiler
{
	namespace colors
	{
		const int32_t Magenta = Optick::Color::Magenta;
		const int32_t Green = Optick::Color::Green;
		const int32_t Red = Optick::Color::Red;
	} // namespace colors
} // namespace profiler

class OptickScopeWrapper
{
public:
	OptickScopeWrapper( const char* name )
	{
		OPTICK_PUSH( name );
	}
	~OptickScopeWrapper()
	{
		OPTICK_POP();
	}
};
#endif // BUILD_WITH_OPTICK
