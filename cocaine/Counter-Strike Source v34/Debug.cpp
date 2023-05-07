#include "Debug.hpp"

#include <windows.h>

#include <iostream>

namespace Debug
{
	void Print( const char* fmt, ... )
	{
		char format[ 2048 ];

		va_list args;
		va_start( args, fmt );
		int i = vsprintf_s( format, sizeof( format ), fmt, args );
		va_end( args );

		format[ i + 0 ] = '\r';
		format[ i + 1 ] = '\n';
		format[ i + 2 ] = '\0';

		OutputDebugString( format );
	}
}