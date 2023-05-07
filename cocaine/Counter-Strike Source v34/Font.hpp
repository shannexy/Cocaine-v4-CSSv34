#pragma once

#include "Color.hpp"

#include <d3d9.h>

#ifdef CreateFont
#undef CreateFont
#endif
#ifdef DrawText
#undef DrawText
#endif

enum
{
	FONT_CREATE_NORMAL = 0,
	FONT_CREATE_BOLD = 1,
	FONT_CREATE_ITALIC = 2,
	FONT_CREATE_DROPSHADOW = 4,
	FONT_CREATE_OUTLINED = 8,
};

namespace Direct3D9
{
	class Font
	{
	public:
		Font( const char* szName, std::uint32_t uHeight, std::uint32_t uFlags = FONT_CREATE_NORMAL );
		~Font();

		HRESULT					DrawText( float x, float y, const Color& color, const char* szText, std::uint32_t uFlags = 0 );
		HRESULT					GetTextExtent( const char* szText, SIZE* pSize );
		HRESULT					Create( IDirect3DDevice9* pDevice );
		HRESULT					RestoreDeviceObjects();
		HRESULT					InvalidateDeviceObjects();
		HRESULT					DeleteDeviceObjects();

		std::uint32_t			GetFlags() const;

	private:
		HRESULT					CreateGdiFont( HDC hDC, HFONT* pFont );
		HRESULT					PaintAlphabet( HDC hDC, bool bMeasureOnly = false );

	private:
		char					m_szFontName[ 80 ];
		std::uint32_t			m_uFontHeight;
		std::uint32_t			m_uFontFlags;

		IDirect3DDevice9*		m_pDevice;
		IDirect3DTexture9*		m_pTexture;
		IDirect3DVertexBuffer9*	m_pVertexList;

		std::uint32_t			m_uTexWidth;
		std::uint32_t			m_uTexHeight;

		float					m_fTextScale;
		float					m_fTexCoords[ 128 - 32 ][ 4 ];

		std::uint32_t			m_uSpacing;
	};
}