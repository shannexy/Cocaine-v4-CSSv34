#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

Vector4::Vector4()
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->w = 0.0f;
}

Vector4::Vector4( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector4::Vector4( const Vector4& v )
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = v.w;
}

Vector4::Vector4( const float* v )
{
	this->x = v[ 0 ];
	this->y = v[ 1 ];
	this->z = v[ 2 ];
	this->w = v[ 3 ];
}

void Vector4::Set( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

bool Vector4::operator==( const Vector4& v ) const
{
	return ( this->x == v.x &&
		this->y == v.y &&
		this->z == v.z &&
		this->w == v.w );
}

bool Vector4::operator!=( const Vector4& v ) const
{
	return ( this->x != v.x ||
		this->y != v.y ||
		this->z != v.z ||
		this->w != v.w );
}

float& Vector4::operator[ ]( unsigned int i )
{
	return ( ( float* )this )[ i ];
}

float Vector4::operator[ ]( unsigned int i ) const
{
	return ( ( float* )this )[ i ];
}

Vector4& Vector4::operator=( const Vector4& v )
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = v.w;

	return *this;
}

Vector4& Vector4::operator=( const float* v )
{
	this->x = v[ 0 ];
	this->y = v[ 1 ];
	this->z = v[ 2 ];
	this->w = v[ 3 ];

	return *this;
}

Vector4& Vector4::operator+=( const Vector4& v )
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	this->w += v.w;

	return *this;
}

Vector4& Vector4::operator-=( const Vector4& v )
{
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
	this->w -= v.w;

	return *this;
}

Vector4& Vector4::operator*=( const Vector4& v )
{
	this->x *= v.x;
	this->y *= v.y;
	this->z *= v.z;
	this->w *= v.w;

	return *this;
}

Vector4& Vector4::operator/=( const Vector4& v )
{
	this->x /= v.x;
	this->y /= v.y;
	this->z /= v.z;
	this->w /= v.w;

	return *this;
}

Vector4& Vector4::operator+=( float v )
{
	this->x += v;
	this->y += v;
	this->z += v;
	this->w += v;

	return *this;
}

Vector4& Vector4::operator-=( float v )
{
	this->x -= v;
	this->y -= v;
	this->z -= v;
	this->w -= v;

	return *this;
}

Vector4& Vector4::operator*=( float v )
{
	this->x *= v;
	this->y *= v;
	this->z *= v;
	this->w *= v;

	return *this;
}

Vector4& Vector4::operator/=( float v )
{
	this->x /= v;
	this->y /= v;
	this->z /= v;
	this->w /= v;

	return *this;
}

Vector4 Vector4::operator+( const Vector4& v ) const
{
	return Vector4( this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w );
}

Vector4 Vector4::operator-( const Vector4& v ) const
{
	return Vector4( this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w );
}

Vector4 Vector4::operator*( const Vector4& v ) const
{
	return Vector4( this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w );
}

Vector4 Vector4::operator/( const Vector4& v ) const
{
	return Vector4( this->x / v.x, this->y / v.y, this->z / v.z, this->w / v.w );
}

Vector4 Vector4::operator+( float v ) const
{
	return Vector4( this->x + v, this->y + v, this->z + v, this->w + v );
}

Vector4 Vector4::operator-( float v ) const
{
	return Vector4( this->x - v, this->y - v, this->z - v, this->w - v );
}

Vector4 Vector4::operator*( float v ) const
{
	return Vector4( this->x * v, this->y * v, this->z * v, this->w * v );
}

Vector4 Vector4::operator/( float v ) const
{
	return Vector4( this->x / v, this->y / v, this->z / v, this->w / v );
}

float Vector4::Dot( const Vector4& v ) const
{
	return ( this->x * v.x + this->y * v.y + this->z * v.z + this->w * v.w );
}

float Vector4::Length() const
{
	return sqrt( this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w );
}

float Vector4::LengthSqr() const
{
	return ( this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w );
}

float Vector4::DistTo( const Vector4& v ) const
{
	return ( *this - v ).Length();
}

float Vector4::DistToSqr( const Vector4& v ) const
{
	return ( *this - v ).LengthSqr();
}

bool Vector4::IsZero() const
{
	return ( this->x > -0.01f && this->x < 0.01f &&
		this->y > -0.01f && this->y < 0.01f &&
		this->z > -0.01f && this->z < 0.01f &&
		this->w > -0.01f && this->w < 0.01f );
}

Vector2 Vector4::ToVector2() const
{
	return Vector2( x, y );
}

Vector3 Vector4::ToVector3() const
{
	return Vector3( x, y, z );
}