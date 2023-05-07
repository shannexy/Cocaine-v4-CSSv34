#include "Renderer.hpp"
#include "Memory.hpp"
#include "Debug.hpp"
#include "Crypt.hpp"

#include <cmath>

namespace Direct3D9
{
	Renderer::Renderer()
		:	m_pDevice( nullptr ),
			m_pStateBlock( nullptr )
	{

	}

	Renderer::~Renderer()
	{
		Memory::SafeRelease( m_pStateBlock );

		for( auto& pFont : m_pFontList )
		{
			pFont->InvalidateDeviceObjects();
			pFont->DeleteDeviceObjects();
			pFont.reset();
		}
	}

	bool Renderer::Create( IDirect3DDevice9* pDevice )
	{
		if( !pDevice )
		{
			DPRINT( XorStr( "[Renderer::Initialize] Device parameter is invalid!" ) );
			return false;
		}

		m_pDevice = pDevice;

		if( FAILED( m_pDevice->CreateStateBlock( D3DSBT_ALL, &m_pStateBlock ) ) )
		{
			DPRINT( XorStr( "[Renderer::Initialize] Failed to create 'state block'!" ) );
			return false;
		}

		return true;
	}

	void Renderer::OnLostDevice()
	{
		Memory::SafeRelease( m_pStateBlock );

		for( auto& pFont : m_pFontList )
			pFont->InvalidateDeviceObjects();
	}

	void Renderer::OnResetDevice()
	{
		m_pDevice->CreateStateBlock( D3DSBT_ALL, &m_pStateBlock );

		for( auto& pFont : m_pFontList )
			pFont->RestoreDeviceObjects();
	}

	void Renderer::Begin()
	{
		m_pStateBlock->Capture();

		m_pDevice->SetVertexShader(nullptr);
		m_pDevice->SetPixelShader(nullptr);
		m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
		m_pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);

		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

		m_pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN
			| D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	}

	void Renderer::End()
	{
		m_pStateBlock->Apply();
	}

	HFont Renderer::CreateFont( const std::string& name, std::uint32_t size, std::uint8_t flags )
	{
		m_pFontList.emplace_back( std::make_unique< Font >( name.c_str(), size, flags ) );

		HFont hRet = m_pFontList.size() - 1;

		m_pFontList[ hRet ]->Create( m_pDevice );
		m_pFontList[ hRet ]->RestoreDeviceObjects();
		
		return hRet;
	}

	void Renderer::DrawText( HFont hFont, float x, float y, int flags, const Color& color, const char* fmt, ... )
	{
		char format[ 4096 ];

		va_list args;
		va_start( args, fmt );
		vsnprintf( format, sizeof( format ), fmt, args );
		va_end( args );

		SIZE size;
		m_pFontList[ hFont ]->GetTextExtent( format, &size );

		if( flags & FONT_ALIGN_RIGHT )
			x -= size.cx;
		else if( flags & FONT_ALIGN_CENTER_H )
			x -= size.cx / 2.0f;

		if( flags & FONT_ALIGN_CENTER_V )
			y -= size.cy / 2.0f;

		x = std::floor( x + 0.5f );
		y = std::floor( y + 0.5f );

		if( m_pFontList[ hFont ]->GetFlags() & FONT_CREATE_DROPSHADOW )
		{
			m_pFontList[ hFont ]->DrawText( x + 1.0f, y + 1.0f, Color::Black, format );
		}
		else if( m_pFontList[ hFont ]->GetFlags() & FONT_CREATE_OUTLINED )
		{
			m_pFontList[ hFont ]->DrawText( x + 1.0f, y + 0.0f, Color::Black, format );
			m_pFontList[ hFont ]->DrawText( x - 1.0f, y + 0.0f, Color::Black, format );
			m_pFontList[ hFont ]->DrawText( x + 0.0f, y + 1.0f, Color::Black, format );
			m_pFontList[ hFont ]->DrawText( x + 0.0f, y - 1.0f, Color::Black, format );
		}

		m_pFontList[ hFont ]->DrawText( x, y, color, format );
	}

	struct Vertex
	{
		Vertex( float _x, float _y, float _z, const Color& color )
			:	x( _x ),
				y( _y ),
				z( _z ),
				color( color.Code() )
		{

		}

		float x;
		float y;
		float z;
		float rhw = 1.0f;
		D3DCOLOR color;

		enum
		{
			FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE
		};
	};

	void Renderer::DrawRect( int x, int y, int w, int h, const Color& color )
	{
		Vertex vertices[ 4 ] =
		{
			Vertex( x, y + h, 1.0f, color ),
			Vertex( x, y, 1.0f, color ),
			Vertex( x + w, y + h, 1.0f, color ),
			Vertex( x + w, y, 1.0f, color )
		};

		m_pDevice->SetFVF( Vertex::FVF );
		m_pDevice->SetTexture( 0, nullptr );
		m_pDevice->SetPixelShader( nullptr );

		m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices, sizeof( Vertex ) );
	}

	void Renderer::DrawRectOut( int x, int y, int w, int h, const Color& color, const Color& out )
	{
		DrawRect( x, y, w, h, color );
		DrawBorderBox( x, y, w, h, 1, out );
	}

	void Renderer::DrawBorderBox( int x, int y, int w, int h, int t, const Color& color )
	{
		DrawRect( x, y, w, t, color );
		DrawRect( x, y, t, h, color );
		DrawRect( x + w, y, t, h, color );
		DrawRect( x, y + h, w + t, t, color );
	}

	void Renderer::DrawBorderBoxOut( int x, int y, int w, int h, int t, const Color& color, const Color& out )
	{
		DrawBorderBox( x, y, w, h, t, color );
		DrawBorderBox( x - t, y - t, w + t * 2, h + t * 2, 1, out );
		DrawBorderBox( x + t, y + t, w - t * 2, h - t * 2, 1, out );
	}

	void Renderer::DrawLine( int x0, int y0, int x1, int y1, const Color& color )
	{
		Vertex vertices[ 2 ] =
		{
			Vertex( x0, y0, 1.0f, color ),
			Vertex( x1, y1, 1.0f, color )
		};

		m_pDevice->SetFVF( Vertex::FVF );
		m_pDevice->SetTexture( 0, nullptr );

		m_pDevice->DrawPrimitiveUP( D3DPT_LINELIST, 2, vertices, sizeof( Vertex ) );
	}
}