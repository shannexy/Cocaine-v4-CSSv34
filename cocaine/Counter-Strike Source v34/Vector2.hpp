#pragma once

#include <cmath>

class Vector2;
class Vector3;
class Vector4;

class Vector2
{
public:
	Vector2();
	Vector2( float x, float y );
	Vector2( const Vector2& v );
	Vector2( const float* v );

	void		Set( float x = 0.0f, float y = 0.0f );

	bool		operator==( const Vector2& v ) const;
	bool		operator!=( const Vector2& v ) const;

	float&		operator[ ]( unsigned int i );
	float		operator[ ]( unsigned int i ) const;

	Vector2&	operator=( const Vector2& v );
	Vector2&	operator=( const float* v );

	Vector2&	operator+=( const Vector2& v );
	Vector2&	operator-=( const Vector2& v );
	Vector2&	operator*=( const Vector2& v );
	Vector2&	operator/=( const Vector2& v );

	Vector2&	operator+=( float v );
	Vector2&	operator-=( float v );
	Vector2&	operator*=( float v );
	Vector2&	operator/=( float v );

	Vector2		operator+( const Vector2& v ) const;
	Vector2		operator-( const Vector2& v ) const;
	Vector2		operator*( const Vector2& v ) const;
	Vector2		operator/( const Vector2& v ) const;

	Vector2		operator+( float v ) const;
	Vector2		operator-( float v ) const;
	Vector2		operator*( float v ) const;
	Vector2		operator/( float v ) const;

	float		Dot( const Vector2& v ) const;

	float		Length() const;
	float		LengthSqr() const;

	float		DistTo( const Vector2& v ) const;
	float		DistToSqr( const Vector2& v ) const;

	bool		IsZero() const;

	Vector3		ToVector3() const;
	Vector4		ToVector4() const;

public:
	float		x;
	float		y;
};

__forceinline Vector2 operator*( float f, const Vector2& v )
{
	return v * f;
}

static Vector2 vec2_zero( 0.0f, 0.0f );