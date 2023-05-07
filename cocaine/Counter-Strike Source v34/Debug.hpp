#pragma once

namespace Debug
{
	extern void Print( const char* fmt, ... );
}

#ifdef _DEBUG
#define DPRINT( fmt, ... ) Debug::Print( fmt, __VA_ARGS__ )
#else
#define DPRINT( fmt, ... )
#endif // _DEBUG