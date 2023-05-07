#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

Vector2::Vector2()
{
	this->x = 0.0f;
	this->y = 0.0f;
}

Vector2::Vector2( float x, float y )
{
	this->x = x;
	this->y = y;
}

Vector2::Vector2( const Vector2& v )
{
	this->x = v.x;
	this->y = v.y;
}

Vector2::Vector2( const float* v )
{
	this->x = v[ 0 ];
	this->y = v[ 1 ];
}

void Vector2::Set( float x, float y )
{
	this->x = x;
	this->y = y;
}

bool Vector2::operator==( const Vector2& v ) const
{
	return ( this->x == v.x &&
		this->y == v.y );
}

bool Vector2::operator!=( const Vector2& v ) const
{
	return ( this->x != v.x ||
		this->y != v.y );
}

float& Vector2::operator[ ]( unsigned int i )
{
	return ( ( float* )this )[ i ];
}

float Vector2::operator[ ]( unsigned int i ) const
{
	return ( ( float* )this )[ i ];
}

Vector2& Vector2::operator=( const Vector2& v )
{
	this->x = v.x;
	this->y = v.y;

	return *this;
}

Vector2& Vector2::operator=( const float* v )
{
	this->x = v[ 0 ];
	this->y = v[ 1 ];

	return *this;
}

Vector2& Vector2::operator+=( const Vector2& v )
{
	this->x += v.x;
	this->y += v.y;

	return *this;
}

Vector2& Vector2::operator-=( const Vector2& v )
{
	this->x -= v.x;
	this->y -= v.y;

	return *this;
}

Vector2& Vector2::operator*=( const Vector2& v )
{
	this->x *= v.x;
	this->y *= v.y;

	return *this;
}

Vector2& Vector2::operator/=( const Vector2& v )
{
	this->x /= v.x;
	this->y /= v.y;

	return *this;
}

Vector2& Vector2::operator+=( float v )
{
	this->x += v;
	this->y += v;

	return *this;
}

Vector2& Vector2::operator-=( float v )
{
	this->x -= v;
	this->y -= v;

	return *this;
}

Vector2& Vector2::operator*=( float v )
{
	this->x *= v;
	this->y *= v;

	return *this;
}

Vector2& Vector2::operator/=( float v )
{
	this->x /= v;
	this->y /= v;

	return *this;
}

Vector2 Vector2::operator+( const Vector2& v ) const
{
	return Vector2( this->x + v.x, this->y + v.y );
}

Vector2 Vector2::operator-( const Vector2& v ) const
{
	return Vector2( this->x - v.x, this->y - v.y );
}

Vector2 Vector2::operator*( const Vector2& v ) const
{
	return Vector2( this->x * v.x, this->y * v.y );
}

Vector2 Vector2::operator/( const Vector2& v ) const
{
	return Vector2( this->x / v.x, this->y / v.y );
}

Vector2 Vector2::operator+( float v ) const
{
	return Vector2( this->x + v, this->y + v );
}

Vector2 Vector2::operator-( float v ) const
{
	return Vector2( this->x - v, this->y - v );
}

Vector2 Vector2::operator*( float v ) const
{
	return Vector2( this->x * v, this->y * v );
}

Vector2 Vector2::operator/( float v ) const
{
	return Vector2( this->x / v, this->y / v );
}

float Vector2::Dot( const Vector2& v ) const
{
	return ( this->x * v.x + this->y * v.y );
}

float Vector2::Length() const
{
	return sqrt( this->x * this->x + this->y * this->y );
}

float Vector2::LengthSqr() const
{
	return ( this->x * this->x + this->y * this->y );
}

float Vector2::DistTo( const Vector2& v ) const
{
	return ( *this - v ).Length();
}

float Vector2::DistToSqr( const Vector2& v ) const
{
	return ( *this - v ).LengthSqr();
}

bool Vector2::IsZero() const
{
	return ( this->x > -0.01f && this->x < 0.01f &&
		this->y > -0.01f && this->y < 0.01f );
}

Vector3 Vector2::ToVector3() const
{
	return Vector3( x, y, 0.0f );
}

Vector4 Vector2::ToVector4() const
{
	return Vector4( x, y, 0.0f, 0.0f );
}