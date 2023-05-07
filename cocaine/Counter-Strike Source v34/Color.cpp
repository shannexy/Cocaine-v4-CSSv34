#include "Color.hpp"

#include <d3d9.h>

namespace Direct3D9
{
	Color Color::Empty( 0, 0, 0, 0 );
	Color Color::White( 255, 255, 255, 255 );
	Color Color::Black( 0, 0, 0, 255 );
	Color Color::Red( 255, 0, 0, 255 );
	Color Color::Green( 0, 255, 0, 255 );
	Color Color::Blue( 0, 0, 255, 255 );
	Color Color::Cyan( 0, 128, 255, 255 );
	Color Color::Yellow( 255, 255, 0, 255 );
	Color Color::Pink( 255, 0, 255, 255 );

	Color::Color()
		:	R( 0 ),
			G( 0 ),
			B( 0 ),
			A( 0 )
	{
	
	}

	Color::Color( std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a )
		:	R( r ),
			G( g ),
			B( b ),
			A( a )
	{
	
	}

	Color::Color( const Color& color )
		:	R( color.R ),
			G( color.G ),
			B( color.B ),
			A( color.A )
	{
	
	}

	Color& Color::operator=( const Color& color )
	{
		R = color.R;
		G = color.G;
		B = color.B;
		A = color.A;

		return *this;
	}

	std::uint8_t& Color::operator[ ]( std::uint32_t i )
	{
		return ( ( std::uint8_t* )this )[ i ];
	}


	std::uint8_t Color::operator[ ]( std::uint32_t i ) const
	{
		return ( ( std::uint8_t* )this )[ i ];
	}

	std::uint32_t Color::Code() const
	{
		return D3DCOLOR_RGBA( R, G, B, A );
	}
}