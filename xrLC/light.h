#ifndef _XR_LIGHT_
#define _XR_LIGHT_
#pragma once

// Light types
const DWORD XRLIGHT_LMAPS		= (1ul << 15ul);	// for LMaps
const DWORD XRLIGHT_MODELS		= (1ul << 14ul);	// for Dynamic Models
const DWORD XRLIGHT_PROCEDURAL	= (1ul << 13ul);	// flashing, etc (affect both)

const DWORD XRLIGHT_MIN_ANIM_KEYS	= 4;
const DWORD XRLIGHT_MAX_ANIM_KEYS	= 32;
const DWORD XRLIGHT_MAX_SECTORS		= 16;

#pragma pack(push,8)
struct xrLIGHT : public Flight
{
	DWORD		dwFrame;

	WORD		flags;
	float		p_time;								// current time (0..1)
	float		p_speed;							// entries/s

	WORD		p_key_start;
	WORD		p_key_count;

	IC float spline(float *m, float p0, float p1, float p2, float p3)
	{	return p0 * m[0] + p1 * m[1] + p2 * m[2] + p3 * m[3]; }

	IC void t_spline(float t, float *m) 
	{
		float     t2  = t * t;
		float     t3  = t2 * t;
		
		m[0] = ( 0.5f * ( (-1.0f * t3) + ( 2.0f * t2) + (-1.0f * t) ) );
		m[1] = ( 0.5f * ( ( 3.0f * t3) + (-5.0f * t2) + ( 0.0f * t) + 2.0f ) );
		m[2] = ( 0.5f * ( (-3.0f * t3) + ( 4.0f * t2) + ( 1.0f * t) ) );
		m[3] = ( 0.5f * ( ( 1.0f * t3) + (-1.0f * t2) + ( 0.0f * t) ) );
	}

	IC void		interpolate(float dt, Flight* keyframes)
	{
		// correct time
		p_time	+= dt*p_speed;
		while (p_time>=1.f)	p_time-=1.f;

		float	p		= p_time * p_key_count;
		DWORD	ip		= iFloor(p);
		float	fp		= p-float(ip);

		WORD	f1=WORD(ip);			Flight &L1	= keyframes[f1+p_key_start];
		WORD	f2=(f1+1)%p_key_count;	Flight &L2	= keyframes[f2+p_key_start];
		WORD	f3=(f1+2)%p_key_count;	Flight &L3	= keyframes[f3+p_key_start];
		WORD	f4=(f1+3)%p_key_count;	Flight &L4	= keyframes[f4+p_key_start];

		float	M[4]; t_spline(fp,M);

		diffuse.r	= spline(M, L1.diffuse.r, L2.diffuse.r, L3.diffuse.r, L4.diffuse.r);
		diffuse.g	= spline(M, L1.diffuse.g, L2.diffuse.g, L3.diffuse.g, L4.diffuse.g);
		diffuse.b	= spline(M, L1.diffuse.b, L2.diffuse.b, L3.diffuse.b, L4.diffuse.b);

		position.x	= spline(M, L1.position.x, L2.position.x, L3.position.x, L4.position.x);
		position.y	= spline(M, L1.position.y, L2.position.y, L3.position.y, L4.position.y);
		position.z	= spline(M, L1.position.z, L2.position.z, L3.position.z, L4.position.z);

		range		= spline(M, L1.range, L2.range, L3.range, L4.range);
		attenuation0= spline(M, L1.attenuation0, L2.attenuation0, L3.attenuation0, L4.attenuation0);
		attenuation1= spline(M, L1.attenuation1, L2.attenuation1, L3.attenuation1, L4.attenuation1);
		attenuation2= spline(M, L1.attenuation2, L2.attenuation2, L3.attenuation2, L4.attenuation2);
	}
};
#pragma pack(pop)

#endif