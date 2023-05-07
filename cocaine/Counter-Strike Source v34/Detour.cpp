#include "Detour.hpp"
#include "Debug.hpp"
#include "Crypt.hpp"

namespace Memory
{
	Detour::Detour()
		:	m_uTarget( 0 ),
			m_uJumpBack( 0 ),
			m_uHooked( 0 ),
			m_nSize( 0 ),
			m_pBackupCode( nullptr )
	{

	}

	Detour::~Detour()
	{
		Release();
	}

	bool Detour::Apply( std::uintptr_t uTarget, std::uintptr_t uHooked, std::size_t nSize )
	{
		if( !uTarget || !uHooked || !nSize )
		{
			DPRINT( XorStr( "[Detour::Apply] Parameters are invalid!" ) );
			return false;
		}

		m_uTarget = uTarget;
		m_uHooked = uHooked;
		m_nSize = nSize;

		ScopedMemProtect ProtectGuard( m_uTarget, m_nSize, PAGE_EXECUTE_READWRITE );

		m_pBackupCode = std::make_unique< std::uint8_t[ ] >( m_nSize );

		memcpy( m_pBackupCode.get(), ( const void* )m_uTarget, m_nSize );

		auto uRelative = ( m_uHooked - m_uTarget ) - 5;

		*( std::uint8_t* )( m_uTarget ) = OPCODE_JMP;
		*( std::uintptr_t* )( m_uTarget + 1 ) = uRelative;

		for( std::size_t i = 5; i < m_nSize; i++ )
			*( std::uint8_t* )( m_uTarget + i ) = OPCODE_NOP;

		m_uJumpBack = m_uTarget + m_nSize;

		return true;
	}

	void Detour::Release()
	{
		if( !m_uTarget || !m_uJumpBack || !m_uHooked )
		{
			DPRINT( XorStr( "[Detour::Release] Detour is not set!" ) );
			return;
		}

		ScopedMemProtect ProtectGuard( m_uTarget, m_nSize, PAGE_EXECUTE_READWRITE );
		memcpy( ( void* )m_uTarget, m_pBackupCode.get(), m_nSize );
	}

	const std::uintptr_t Detour::GetReturnLocation() const
	{
		return m_uJumpBack;
	}
}