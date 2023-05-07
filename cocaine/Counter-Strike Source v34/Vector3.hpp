#pragma once

#include <cmath>

class Vector2;
class Vector3;
class Vector4;

class Vector3
{
public:
	Vector3();
	Vector3( float x, float y, float z );
	Vector3( const Vector3& v );
	Vector3( const float* v );

	void		Set( float x = 0.0f, float y = 0.0f, float z = 0.0f );

	bool		operator==( const Vector3& v ) const;
	bool		operator!=( const Vector3& v ) const;

	float&		operator[ ]( unsigned int i );
	float		operator[ ]( unsigned int i ) const;

	Vector3		operator-() const;

	Vector3&	operator=( const Vector3& v );
	Vector3&	operator=( const float* v );

	Vector3&	operator+=( const Vector3& v );
	Vector3&	operator-=( const Vector3& v );
	Vector3&	operator*=( const Vector3& v );
	Vector3&	operator/=( const Vector3& v );

	Vector3&	operator+=( float v );
	Vector3&	operator-=( float v );
	Vector3&	operator*=( float v );
	Vector3&	operator/=( float v );

	Vector3		operator+( const Vector3& v ) const;
	Vector3		operator-( const Vector3& v ) const;
	Vector3		operator*( const Vector3& v ) const;
	Vector3		operator/( const Vector3& v ) const;

	Vector3		operator+( float v ) const;
	Vector3		operator-( float v ) const;
	Vector3		operator*( float v ) const;
	Vector3		operator/( float v ) const;

	float		Dot( const Vector3& v ) const;

	float		Length() const;
	float		LengthSqr() const;

	float		Length2D() const;
	float		Length2DSqr() const;

	float		DistTo( const Vector3& v ) const;
	float		DistToSqr( const Vector3& v ) const;

	float		Normalize();

	bool		IsZero() const;

	Vector2		ToVector2() const;
	Vector4		ToVector4() const;

public:
	float		x;
	float		y;
	float		z;
};

__forceinline Vector3 operator*( float f, const Vector3& v )
{
	return v * f;
}

static Vector3 vec3_zero( 0.0f, 0.0f, 0.0f );