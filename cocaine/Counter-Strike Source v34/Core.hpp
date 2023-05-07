#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

#define M_PI ( float )3.14159265358979323846

class Matrix3x4;
class Matrix4x4;

class Matrix3x4
{
public:
	float*			operator[ ]( unsigned int i );
	const float*	operator[ ]( unsigned int i ) const;

public:
	float m[ 3 ][ 4 ];
};

class Matrix4x4
{
public:
	float*			operator[ ]( unsigned int i );
	const float*	operator[ ]( unsigned int i ) const;

public:
	float m[ 4 ][ 4 ];
};

float square( float x );

void SinCos( float radians, float* sine, float* cosine );

float ToRadians( float degrees );
float ToDegrees( float radians );

void VectorClear( Vector3& v );
void VectorCopy( const Vector3& src, Vector3& dst );

void VectorAdd( const Vector3& a, const Vector3& b, Vector3& result );
void VectorSubtract( const Vector3& a, const Vector3& b, Vector3& result );
void VectorMultiply( const Vector3& a, float b, Vector3& result );
void VectorMultiply( const Vector3& a, const Vector3& b, Vector3& result );
void VectorDivide( const Vector3& a, float b, Vector3& result );
void VectorDivide( const Vector3& a, const Vector3& b, Vector3& result );
void VectorScale( const Vector3& in, float scale, Vector3& result );
void VectorMA( const Vector3& start, float scale, const Vector3& direction, Vector3& dest );

void CrossProduct( const float* a, const float* b, float* result );
void CrossProduct( const Vector3& a, const Vector3& b, Vector3& result );

float DotProduct( const float* a, const float* b );
float DotProduct( const Vector3& a, const Vector3& b );

float VectorLength( const float* v );
float VectorLength( const Vector3& v );

float VectorNormalize( float* v );
float VectorNormalize( Vector3& v );

void VectorTransform( const float* in, const Matrix3x4& mat, float* out );
void VectorTransform( const Vector3& in, const Matrix3x4& mat, Vector3& out );

float AngleNormalize( float angle );

void AnglesNormalize( float* angles );
void AnglesNormalize( Vector3& angles );

void ClampAngles( Vector3& angles );

void VectorAngles( const Vector3& forward, Vector3& angles );
void VectorAngles( const Vector3& forward, const Vector3& pseudoup, Vector3& angles );

void AngleVectors( const Vector3& angles, Vector3* forward );
void AngleVectors( const Vector3& angles, Vector3* forward, Vector3* right, Vector3* up );