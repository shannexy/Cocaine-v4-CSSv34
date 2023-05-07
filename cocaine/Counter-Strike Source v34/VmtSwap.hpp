#pragma once

#include "ScopedMemProtect.hpp"

namespace Memory
{
	class VmtSwap
	{
	public:
		VmtSwap();

		bool Apply( const void* pInstance );
		void Release();

		void Replace();
		void Restore();

		void Hook( const void* pHooked, const std::size_t nIndex );

		template< typename T >
		inline T VCall( const std::size_t nIndex )
		{
			return ( T )( m_pBackupVmt[ nIndex ] );
		}

	private:
		std::uintptr_t**						m_ppInstance;
		std::uintptr_t*							m_pBackupVmt;
		std::unique_ptr< std::uintptr_t[ ] >	m_pCustomVmt;
		std::size_t								m_nSize;
	};
}