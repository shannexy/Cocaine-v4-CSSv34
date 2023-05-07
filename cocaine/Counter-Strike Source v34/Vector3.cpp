#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

Vector3::Vector3()
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
}

Vector3::Vector3( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3( const Vector3& v )
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}

Vector3::Vector3( const float* v )
{
	this->x = v[ 0 ];
	this->y = v[ 1 ];
	this->z = v[ 2 ];
}

void Vector3::Set( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

bool Vector3::operator==( const Vector3& v ) const
{
	return ( this->x == v.x &&
		this->y == v.y &&
		this->z == v.z );
}

bool Vector3::operator!=( const Vector3& v ) const
{
	return ( this->x != v.x ||
		this->y != v.y ||
		this->z != v.z );
}

float& Vector3::operator[ ]( unsigned int i )
{
	return ( ( float* )this )[ i ];
}

float Vector3::operator[ ]( unsigned int i ) const
{
	return ( ( float* )this )[ i ];
}

Vector3 Vector3::operator-() const
{
	return Vector3( -x, -y, -z );
}

Vector3& Vector3::operator=( const Vector3& v )
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;

	return *this;
}

Vector3& Vector3::operator=( const float* v )
{
	this->x = v[ 0 ];
	this->y = v[ 1 ];
	this->z = v[ 2 ];

	return *this;
}

Vector3& Vector3::operator+=( const Vector3& v )
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;

	return *this;
}

Vector3& Vector3::operator-=( const Vector3& v )
{
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;

	return *this;
}

Vector3& Vector3::operator*=( const Vector3& v )
{
	this->x *= v.x;
	this->y *= v.y;
	this->z *= v.z;

	return *this;
}

Vector3& Vector3::operator/=( const Vector3& v )
{
	this->x /= v.x;
	this->y /= v.y;
	this->z /= v.z;

	return *this;
}

Vector3& Vector3::operator+=( float v )
{
	this->x += v;
	this->y += v;
	this->z += v;

	return *this;
}

Vector3& Vector3::operator-=( float v )
{
	this->x -= v;
	this->y -= v;
	this->z -= v;

	return *this;
}

Vector3& Vector3::operator*=( float v )
{
	this->x *= v;
	this->y *= v;
	this->z *= v;

	return *this;
}

Vector3& Vector3::operator/=( float v )
{
	this->x /= v;
	this->y /= v;
	this->z /= v;

	return *this;
}

Vector3 Vector3::operator+( const Vector3& v ) const
{
	return Vector3( this->x + v.x, this->y + v.y, this->z + v.z );
}

Vector3 Vector3::operator-( const Vector3& v ) const
{
	return Vector3( this->x - v.x, this->y - v.y, this->z - v.z );
}

Vector3 Vector3::operator*( const Vector3& v ) const
{
	return Vector3( this->x * v.x, this->y * v.y, this->z * v.z );
}

Vector3 Vector3::operator/( const Vector3& v ) const
{
	return Vector3( this->x / v.x, this->y / v.y, this->z / v.z );
}

Vector3 Vector3::operator+( float v ) const
{
	return Vector3( this->x + v, this->y + v, this->z + v );
}

Vector3 Vector3::operator-( float v ) const
{
	return Vector3( this->x - v, this->y - v, this->z - v );
}

Vector3 Vector3::operator*( float v ) const
{
	return Vector3( this->x * v, this->y * v, this->z * v );
}

Vector3 Vector3::operator/( float v ) const
{
	return Vector3( this->x / v, this->y / v, this->z / v );
}

float Vector3::Dot( const Vector3& v ) const
{
	return ( this->x * v.x + this->y * v.y + this->z * v.z );
}

float Vector3::Length() const
{
	return sqrt( this->x * this->x + this->y * this->y + this->z * this->z );
}

float Vector3::LengthSqr() const
{
	return ( this->x * this->x + this->y * this->y + this->z * this->z );
}

float Vector3::Length2D() const
{
	return sqrt( this->x * this->x + this->y * this->y );
}

float Vector3::Length2DSqr() const
{
	return ( this->x * this->x + this->y * this->y );
}

float Vector3::DistTo( const Vector3& v ) const
{
	return ( *this - v ).Length();
}

float Vector3::DistToSqr( const Vector3& v ) const
{
	return ( *this - v ).LengthSqr();
}

float Vector3::Normalize()
{
	auto length = Length();
	auto length_normal = 1.0f / length;

	x *= length_normal;
	y *= length_normal;
	z *= length_normal;

	return length;
}

bool Vector3::IsZero() const
{
	return ( this->x > -0.01f && this->x < 0.01f &&
		this->y > -0.01f && this->y < 0.01f &&
		this->z > -0.01f && this->z < 0.01f );
}

Vector2 Vector3::ToVector2() const
{
	return Vector2( x, y );
}

Vector4 Vector3::ToVector4() const
{
	return Vector4( x, y, z, 0.0f );
}