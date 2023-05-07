#pragma once

#include <cstdint>

namespace Direct3D9
{
	class Color
	{
	public:
		Color();
		Color( std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255 );
		Color( const Color& color );

		Color&			operator=( const Color& color );

		std::uint8_t&	operator[ ]( std::uint32_t i );
		std::uint8_t	operator[ ]( std::uint32_t i ) const;

		std::uint32_t	Code() const;

	public:
		static Color	Empty;
		static Color	White;
		static Color	Black;
		static Color	Red;
		static Color	Green;
		static Color	Blue;
		static Color	Cyan;
		static Color	Yellow;
		static Color	Pink;

	public:
		std::uint8_t	R, G, B, A;
	};
}