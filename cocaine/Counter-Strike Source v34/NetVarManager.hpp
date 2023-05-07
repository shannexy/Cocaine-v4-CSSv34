#pragma once

#include "Valve.hpp"

#include <vector>

namespace Source
{
	class NetVarManager
	{
	public:
		bool						Create( IBaseClientDLL* pClient );

		int							Get( const char* pszListName, const char* pszPropName );

		RecvVarProxyFn				HookProp( const char* pszListName, const char* pszPropName, RecvVarProxyFn pHooked );

	private:
		int							GetProp( const char* pszListName, const char* pszPropName, RecvProp** ppProp = nullptr );
		int							GetProp( RecvTable* pList, const char* pszPropName, RecvProp** ppProp = nullptr );

		RecvTable*					GetList( const char* pszName );

	private:
		std::vector< RecvTable* >	m_pList;
	};
}