#pragma once

#include <cstdint>
#include <string>

namespace ImGui
{
	extern uint32_t				GetCodeFromName( const std::string& name );
	extern const std::string&	GetNameFromCode( uint32_t code );
}