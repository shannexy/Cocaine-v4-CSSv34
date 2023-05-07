#pragma once

#include "ScopedMemProtect.hpp"

namespace Memory
{
	enum
	{
		OPCODE_NOP = 0x90,
		OPCODE_JMP = 0xE9,
	};

	class Detour
	{
	public:
		Detour();
		~Detour();

		bool								Apply( std::uintptr_t uTarget, std::uintptr_t uHooked, std::size_t nSize );
		void								Release();

		const std::uintptr_t				GetReturnLocation() const;

	private:
		std::uintptr_t						m_uTarget;
		std::uintptr_t						m_uJumpBack;
		std::uintptr_t						m_uHooked;
		std::size_t							m_nSize;
		std::unique_ptr< std::uint8_t[ ] >	m_pBackupCode;
	};
}