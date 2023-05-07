#include "NetVarManager.hpp"

namespace Source
{
	bool NetVarManager::Create( IBaseClientDLL* pClient )
	{
		auto pList = pClient->GetAllClasses();

		if( !pList )
		{
			DPRINT( XorStr( "[NetVarManager::Initialize] Can't get 'client class' list!" ) );
			return false;
		}

		while( pList )
		{
			auto pSlot = pList->m_pRecvTable;

			if( pSlot )
				m_pList.push_back( pSlot );

			pList = pList->m_pNext;
		}

		if( m_pList.empty() )
		{
			DPRINT( XorStr( "[NetVarManager::Initialize] 'recv table' list is empty!" ) );
			return false;
		}

		return true;
	}

	int NetVarManager::Get( const char* pszListName, const char* pszPropName )
	{
		auto iResult = GetProp( pszListName, pszPropName );

		if( !iResult )
			return 0;

		return iResult;
	}

	RecvVarProxyFn NetVarManager::HookProp( const char* pszListName, const char* pszPropName, RecvVarProxyFn pHooked )
	{
		RecvProp* pProp = nullptr;

		if( !GetProp( pszListName, pszPropName, &pProp ) )
			return nullptr;

		RecvVarProxyFn pResult = pProp->m_ProxyFn;

		pProp->m_ProxyFn = pHooked;

		return pResult;
	}

	int NetVarManager::GetProp( const char* pszListName, const char* pszPropName, RecvProp** ppProp )
	{
		auto pList = GetList( pszListName );

		if( !pList )
			return 0;

		auto iResult = GetProp( pList, pszPropName, ppProp );

		if( !iResult )
			return 0;

		return iResult;
	}

	int NetVarManager::GetProp( RecvTable* pList, const char* pszPropName, RecvProp** ppProp )
	{
		auto iExtra = 0;

		for( auto i = 0; i < pList->m_nProps; i++ )
		{
			auto pProp = &pList->m_pProps[ i ];
			auto pChild = pProp->m_pDataTable;

			if( pChild && pChild->m_nProps )
			{
				auto iAdd = GetProp( pChild, pszPropName, ppProp );

				if( iAdd )
					iExtra += ( pProp->m_Offset + iAdd );
			}

			if( std::strcmp( pszPropName, pProp->m_pVarName ) )
				continue;

			if( ppProp )
				*ppProp = pProp;

			return ( pProp->m_Offset + iExtra );
		}

		return iExtra;
	}

	RecvTable* NetVarManager::GetList( const char* pszName )
	{
		if( !pszName )
			return nullptr;

		if( !m_pList.empty() )
		{
			for( auto& pList : m_pList )
			{
				if( std::strcmp( pszName, pList->m_pNetTableName ) == 0 )
					return pList;
			}
		}

		return nullptr;
	}
}