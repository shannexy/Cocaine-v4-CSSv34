#pragma once

#include "Font.hpp"

#include <vector>
#include <memory>
#include <string>

#define INVALID_FONT_HANDLE -1

enum
{
	FONT_ALIGN_LEFT = 0,
	FONT_ALIGN_RIGHT = 1,
	FONT_ALIGN_CENTER_V = 2,
	FONT_ALIGN_CENTER_H = 4,
	FONT_ALIGN_CENTER = FONT_ALIGN_CENTER_V | FONT_ALIGN_CENTER_H,
};

namespace Direct3D9
{
	typedef long HFont;

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		bool									Create( IDirect3DDevice9* pDevice );

		void									OnLostDevice();
		void									OnResetDevice();

		void									Begin();
		void									End();

		HFont									CreateFont( const std::string& name, std::uint32_t size, std::uint8_t flags = FONT_CREATE_NORMAL );

		void									DrawText( HFont hFont, float x, float y, int flags, const Color& color, const char* fmt, ... );

		void									DrawRect( int x, int y, int w, int h, const Color& color );
		void									DrawRectOut( int x, int y, int w, int h, const Color& color, const Color& out = Color::Black );
		void									DrawBorderBox( int x, int y, int w, int h, int t, const Color& color );
		void									DrawBorderBoxOut( int x, int y, int w, int h, int t, const Color& color, const Color& out = Color::Black );

		void									DrawLine( int x0, int y0, int x1, int y1, const Color& color );

	private:
		IDirect3DDevice9*						m_pDevice;
		IDirect3DStateBlock9*					m_pStateBlock;
		std::vector< std::unique_ptr< Font > >	m_pFontList;
	};
}