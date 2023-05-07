#include "Core.hpp"

float* Matrix3x4::operator[ ]( unsigned int i )
{
	return m[ i ];
}

const float* Matrix3x4::operator[ ]( unsigned int i ) const
{
	return m[ i ];
}

float* Matrix4x4::operator[ ]( unsigned int i )
{
	return m[ i ];
}

const float* Matrix4x4::operator[ ]( unsigned int i ) const
{
	return m[ i ];
}

__forceinline float square( float x )
{
	return ( x * x );
}

void SinCos( float radians, float* sine, float* cosine )
{
	if( sine )
		*sine = std::sin( radians );

	if( cosine )
		*cosine = std::cos( radians );
}

float ToRadians( float degrees )
{
	return ( degrees * ( M_PI / 180.0f ) );
}

float ToDegrees( float radians )
{
	return ( radians * ( 180.0f / M_PI ) );
}

void VectorClear( Vector3& a )
{
	a.x = 0.0f;
	a.y = 0.0f;
	a.z = 0.0f;
}

void VectorCopy( const Vector3& src, Vector3& dst )
{
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

void VectorAdd( const Vector3& a, const Vector3& b, Vector3& result )
{
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
}

void VectorSubtract( const Vector3& a, const Vector3& b, Vector3& result )
{
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
}

void VectorMultiply( const Vector3& a, float b, Vector3& result )
{
	result.x = a.x * b;
	result.y = a.y * b;
	result.z = a.y * b;
}

void VectorMultiply( const Vector3& a, const Vector3& b, Vector3& result )
{
	result.x = a.x * b.x;
	result.y = a.y * b.y;
	result.z = a.z * b.z;
}

void VectorDivide( const Vector3& a, float b, Vector3& result )
{
	float oob = 1.0f / b;

	result.x = a.x * oob;
	result.y = a.y * oob;
	result.z = a.z * oob;
}

void VectorDivide( const Vector3& a, const Vector3& b, Vector3& result )
{
	result.x = a.x / b.x;
	result.y = a.y / b.y;
	result.z = a.z / b.z;
}

void VectorScale( const Vector3& in, float scale, Vector3& result )
{
	VectorMultiply( in, scale, result );
}

void VectorMA( const Vector3& start, float scale, const Vector3& direction, Vector3& dest )
{
	dest.x = start.x + direction.x * scale;
	dest.y = start.y + direction.y * scale;
	dest.z = start.z + direction.z * scale;
}

void CrossProduct( const float* a, const float* b, float* result )
{
	result[ 0 ] = a[ 1 ] * b[ 2 ] - a[ 2 ] * b[ 1 ];
	result[ 1 ] = a[ 2 ] * b[ 0 ] - a[ 0 ] * b[ 2 ];
	result[ 2 ] = a[ 0 ] * b[ 1 ] - a[ 1 ] * b[ 0 ];
}

void CrossProduct( const Vector3& a, const Vector3& b, Vector3& result )
{
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
}

float DotProduct( const float* a, const float* b )
{
	return ( a[ 0 ] * b[ 0 ] + a[ 1 ] * b[ 1 ] + a[ 2 ] * b[ 2 ] );
}

float DotProduct( const Vector3& a, const Vector3& b )
{
	return ( a.x * b.x + a.y * b.y + a.z * b.z );
}

float VectorLength( const float* v )
{
	return std::sqrt( square( v[ 0 ] ) + square( v[ 1 ] ) + square( v[ 2 ] ) );
}

float VectorLength( const Vector3& v )
{
	return std::sqrt( square( v.x ) + square( v.y ) + square( v.z ) );
}

float VectorNormalize( float* v )
{
	float l = VectorLength( v );

	if( l != 0.0f )
	{
		v[ 0 ] /= l;
		v[ 1 ] /= l;
		v[ 2 ] /= l;
	}
	else
	{
		v[ 0 ] = 0.0f;
		v[ 1 ] = 0.0f;
		v[ 2 ] = 1.0f;
	}

	return l;
}

float VectorNormalize( Vector3& v )
{
	float l = VectorLength( v );

	if( l != 0.0f )
	{
		v /= l;
	}
	else
	{
		v.x = 0.0f;
		v.y = 0.0f;
		v.z = 1.0f;
	}

	return l;
}

void VectorTransform( const float* in, const Matrix3x4& mat, float* out )
{
	out[ 0 ] = DotProduct( in, mat[ 0 ] ) + mat[ 0 ][ 3 ];
	out[ 1 ] = DotProduct( in, mat[ 1 ] ) + mat[ 1 ][ 3 ];
	out[ 2 ] = DotProduct( in, mat[ 2 ] ) + mat[ 2 ][ 3 ];
}

void VectorTransform( const Vector3& in, const Matrix3x4& mat, Vector3& out )
{
	out.x = in.Dot( mat[ 0 ] ) + mat[ 0 ][ 3 ];
	out.y = in.Dot( mat[ 1 ] ) + mat[ 1 ][ 3 ];
	out.z = in.Dot( mat[ 2 ] ) + mat[ 2 ][ 3 ];
}

float AngleNormalize( float angle )
{
	/*auto normalized = 0.0f;
	auto revolutions = angle / 360.0f;

	if( angle > 180.0f || angle < -180.0f )
	{
		if( revolutions < 0.0f )
			revolutions = -revolutions;

		revolutions = std::round( revolutions );

		if( angle < 0.0f )
			normalized = ( angle + 360.0f * revolutions );
		else
			normalized = ( angle - 360.0f * revolutions );
	}

	return normalized;*/

	while( angle < -180.0f )
		angle += 360.0f;

	while( angle > 180.0f )
		angle -= 360.0f;

	return angle;
}

void AnglesNormalize( float* angles )
{
	angles[ 0 ] = AngleNormalize( angles[ 0 ] );
	angles[ 1 ] = AngleNormalize( angles[ 1 ] );
	angles[ 2 ] = AngleNormalize( angles[ 2 ] );
}

void AnglesNormalize( Vector3& angles )
{
	angles.x = AngleNormalize( angles.x );
	angles.y = AngleNormalize( angles.y );
	angles.z = AngleNormalize( angles.z );
}

void ClampAngles( Vector3& angles )
{
	AnglesNormalize( angles );

	if( angles.x < -89.0f )
		angles.x = -89.0f;
	else if( angles.x > 89.0f )
		angles.x = 89.0f;

	if( angles.y < -180.0f )
		angles.y = -180.0f;
	else if( angles.y > 180.0f )
		angles.y = 180.0f;

	if( angles.z < -50.0f )
		angles.z = -50.0f;
	else if( angles.z > 50.0f )
		angles.z = 50.0f;
}

void VectorAngles( const Vector3& forward, Vector3& angles )
{
	float tmp, yaw, pitch;

	if( forward.x == 0.0f && forward.y == 0.0f )
	{
		yaw = 0.0f;

		if( forward.z > 0.0f )
			pitch = 270.0f;
		else
			pitch = 90.0f;
	}
	else
	{
		yaw = ToDegrees( atan2( forward.y, forward.x ) );

		if( yaw < 0.0f )
			yaw += 360.0f;

		tmp = sqrt( square( forward.x ) + square( forward.y ) );

		pitch = ToDegrees( atan2( -forward.z, tmp ) );

		if( pitch < 0.0f )
			pitch += 360.0f;
	}

	angles.x = pitch;
	angles.y = yaw;
	angles.z = 0.0f;
}

void VectorAngles( const Vector3& forward, const Vector3& pseudoup, Vector3& angles )
{
	Vector3 left;

	CrossProduct( pseudoup, forward, left );

	VectorNormalize( left );

	auto dist = sqrt( square( forward.x ) + square( forward.y ) );

	if( dist > 0.001f )
	{
		angles.x = ToDegrees( atan2( -forward.z, dist ) );
		angles.y = ToDegrees( atan2( forward.y, forward.x ) );

		auto up_z = ( left.y * forward.x ) - ( left.x * forward.y );

		angles.z = ToDegrees( atan2( left.z, up_z ) );
	}
	else
	{
		angles.x = ToDegrees( atan2( -forward.z, dist ) );
		angles.y = ToDegrees( atan2( -left.x, left.y ) );
		angles.z = 0.0f;
	}
}

void AngleVectors( const Vector3& angles, Vector3* forward )
{
	float sp, sy, cp, cy;

	SinCos( ToRadians( angles.x ), &sp, &cp );
	SinCos( ToRadians( angles.y ), &sy, &cy );

	if( forward )
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
}

void AngleVectors( const Vector3& angles, Vector3* forward, Vector3* right, Vector3* up )
{
	float sr, sp, sy, cr, cp, cy;

	SinCos( ToRadians( angles.x ), &sp, &cp );
	SinCos( ToRadians( angles.y ), &sy, &cy );
	SinCos( ToRadians( angles.z ), &sr, &cr );

	if( forward )
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if( right )
	{
		right->x = ( -1.0f * sr * sp * cy + -1.0f * cr * -sy );
		right->y = ( -1.0f * sr * sp * sy + -1.0f * cr * cy );
		right->z = -1.0f * sr * cp;
	}

	if( up )
	{
		up->x = ( cr * sp * cy + -sr * -sy );
		up->y = ( cr * sp * sy + -sr * cy );
		up->z = cr * cp;
	}
}