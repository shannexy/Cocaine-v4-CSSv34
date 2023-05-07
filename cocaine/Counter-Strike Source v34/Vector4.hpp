#pragma once

#include <cmath>

class Vector2;
class Vector3;
class Vector4;

class Vector4
{
public:
	Vector4();
	Vector4( float x, float y, float z, float w );
	Vector4( const Vector4& v );
	Vector4( const float* v );

	void		Set( float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f );

	bool		operator==( const Vector4& v ) const;
	bool		operator!=( const Vector4& v ) const;

	float&		operator[ ]( unsigned int i );
	float		operator[ ]( unsigned int i ) const;

	Vector4&	operator=( const Vector4& v );
	Vector4&	operator=( const float* v );

	Vector4&	operator+=( const Vector4& v );
	Vector4&	operator-=( const Vector4& v );
	Vector4&	operator*=( const Vector4& v );
	Vector4&	operator/=( const Vector4& v );

	Vector4&	operator+=( float v );
	Vector4&	operator-=( float v );
	Vector4&	operator*=( float v );
	Vector4&	operator/=( float v );

	Vector4		operator+( const Vector4& v ) const;
	Vector4		operator-( const Vector4& v ) const;
	Vector4		operator*( const Vector4& v ) const;
	Vector4		operator/( const Vector4& v ) const;

	Vector4		operator+( float v ) const;
	Vector4		operator-( float v ) const;
	Vector4		operator*( float v ) const;
	Vector4		operator/( float v ) const;

	float		Dot( const Vector4& v ) const;

	float		Length() const;
	float		LengthSqr() const;

	float		DistTo( const Vector4& v ) const;
	float		DistToSqr( const Vector4& v ) const;

	bool		IsZero() const;

	Vector2		ToVector2() const;
	Vector3		ToVector3() const;

public:
	float		x;
	float		y;
	float		z;
	float		w;
};

__forceinline Vector4 operator*( float f, const Vector4& v )
{
	return v * f;
}