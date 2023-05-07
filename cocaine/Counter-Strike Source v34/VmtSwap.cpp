#include "VmtSwap.hpp"
#include "Debug.hpp"
#include "Crypt.hpp"

namespace Memory
{
	VmtSwap::VmtSwap()
		:	m_ppInstance( nullptr ),
			m_pBackupVmt( nullptr ),
			m_pCustomVmt( nullptr ),
			m_nSize( 0 ) 
	{

	}

	bool VmtSwap::Apply( const void* pInstance )
	{
		if( !pInstance )
		{
			DPRINT( XorStr( "[VmtSwap::Apply] Can't hook at invalid address! (0x%X)" ), pInstance );
			return false;
		}

		m_ppInstance = ( std::uintptr_t** )pInstance;
		m_pBackupVmt = *m_ppInstance;

		while( m_pBackupVmt[ m_nSize ] )
			m_nSize++;

		m_pCustomVmt = std::make_unique< std::uintptr_t[ ] >( m_nSize );

		memcpy( m_pCustomVmt.get(), m_pBackupVmt, m_nSize * sizeof( std::uintptr_t ) );

		Replace();

		return true;
	}

	void VmtSwap::Release()
	{
		Restore();
	}

	void VmtSwap::Replace()
	{
		ScopedMemProtect ProtectGuard( m_ppInstance, sizeof( std::uintptr_t ), PAGE_READWRITE );
		*m_ppInstance = m_pCustomVmt.get();
	}

	void VmtSwap::Restore()
	{
		ScopedMemProtect ProtectGuard( m_ppInstance, sizeof( std::uintptr_t ), PAGE_READWRITE );
		*m_ppInstance = m_pBackupVmt;
	}

	void VmtSwap::Hook( const void* pHooked, const std::size_t nIndex )
	{
		m_pCustomVmt[ nIndex ] = ( std::uintptr_t )pHooked;
	}
}