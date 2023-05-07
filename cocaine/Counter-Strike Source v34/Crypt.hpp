#pragma once

#include <string>
#include <array>
#include <cstdarg>

namespace Crypt
{
	namespace CompileTime
	{
		constexpr auto time = __TIME__;
		constexpr auto seed = static_cast< int >( time[ 7 ] ) + static_cast< int >( time[ 6 ] ) * 10 + static_cast< int >( time[ 4 ] ) * 60 + static_cast< int >( time[ 3 ] ) * 600 + static_cast< int >( time[ 1 ] ) * 3600 + static_cast< int >( time[ 0 ] ) * 36000;
	}

	template< int N >
	struct RandomGenerator
	{
	private:
		static constexpr unsigned a		= 16807;
		static constexpr unsigned m		= 2147483647;

		static constexpr unsigned s		= RandomGenerator< N - 1 >::value;
		static constexpr unsigned lo	= a * ( s & 0xFFFF );
		static constexpr unsigned hi	= a * ( s >> 16 );
		static constexpr unsigned lo2	= lo + ( ( hi & 0x7FFF ) << 16 );
		static constexpr unsigned hi2	= hi >> 15;
		static constexpr unsigned lo3	= lo2 + hi;

	public:
		static constexpr unsigned max	= m;
		static constexpr unsigned value	= lo3 > m ? lo3 - m : lo3;
	};

	template< >
	struct RandomGenerator< 0 >
	{
		static constexpr unsigned value = CompileTime::seed;
	};

	template< int N, int M >
	struct RandomInt
	{
		static constexpr auto value = RandomGenerator< N + 1 >::value % M;
	};

	template< int N >
	struct RandomChar
	{
		static const char value = static_cast< char >( 1 + RandomInt< N, 0x7F - 1 >::value );
	};


	template< size_t N, int K >
	struct XorString
	{
	public:
		template< size_t... IS >
		constexpr __forceinline XorString( const char* s, std::index_sequence< IS... > ) : _key( RandomChar< K >::value ), _encrypted{ enc( s[ IS ] )... }
		{
		}

		__forceinline decltype( auto ) decrypt()
		{
			for( size_t i = 0; i < N; i++ )
				_encrypted[ i ] = dec( _encrypted[ i ] );

			_encrypted[ N ] = '\0';

			return _encrypted.data();
		}

	private:
		constexpr char enc( char c ) const
		{
			return c ^ _key;
		}

		char dec( char c ) const
		{
			return c ^ _key;
		}

	private:
		const char _key;
		std::array< char, N + 1 > _encrypted;
	};

	static auto printf = [ ]( const char* fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		vprintf( fmt, args );
		va_end( args );
	};

	static auto printf_s = [ ]( const char* fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		vprintf_s( fmt, args );
		va_end( args );
	};

#pragma warning( disable : 4996 )
	static auto sprintf = [ ]( char* dest, const char* fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		vsprintf( dest, fmt, args );
		va_end( args );
	};
#pragma warning( default : 4996 )

	static auto sprintf_s = [ ]( char* dest, std::size_t size, const char* fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		vsprintf_s( dest, size, fmt, args );
		va_end( args );
	};
}

#define XorStr( s ) s//( Crypt::XorString< sizeof( s ) - 1, __COUNTER__ >( s, std::make_index_sequence< sizeof( s ) - 1 >() ).decrypt() )