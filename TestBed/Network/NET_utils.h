#pragma once

#include "net_compressor.h"

const	DWORD		NET_PacketSizeLimit	= 4096;

struct	ENGINE_API	NET_Buffer	
{
	DWORD	count;
	BYTE	data	[NET_PacketSizeLimit];
};

class	ENGINE_API	NET_Packet
{
public:
	NET_Buffer		B;
	DWORD			r_pos;
public:
	// writing - main
	IC void	w_begin	( u16 type		)				// begin of packet 'type'
	{ 
		B.count=0; 
		w_u16(type);	
	}
	IC void	w		( const void* p, DWORD count )
	{
		VERIFY		(p && count);
		CopyMemory	(&B.data[B.count],p,count);
		B.count		+= count;
		VERIFY		(B.count<NET_PacketSizeLimit);
	}

	// writing - utilities
	IC void	w_float		( float a       )	{ w(&a,4);					}			// float
	IC void w_vec3		( Fvector& a    )   { w(&a,3*sizeof(float));	}			// vec3
	IC void w_vec4		( Fvector4& a   )   { w(&a,4*sizeof(float));	}			// vec4
	IC void w_u64		( u64 a			)	{ w(&a,8);					}			// qword (8b)
	IC void w_s64		( s64 a			)	{ w(&a,8);					}			// qword (8b)
	IC void w_u32		( u32 a			)	{ w(&a,4);					}			// dword (4b)
	IC void w_s32		( s32 a			)	{ w(&a,4);					}			// dword (4b)
	IC void w_u16		( u16 a			)	{ w(&a,2);					}			// word (2b)
	IC void w_s16		( s16 a			)	{ w(&a,2);					}			// word (2b)
	IC void	w_u8		( u8 a			)	{ w(&a,1);					}			// byte (1b)
	IC void	w_s8		( s8 a			)	{ w(&a,1);					}			// byte (1b)
	IC void w_float_q16	( float a, float min, float max)
	{
		VERIFY(a>=min && a<=max);
		float q = (a-min)/(max-min);
		w_u16( u16(iFloor((q+EPS_L)*65535.f)));
	}
	IC void w_float_q8	( float a, float min, float max)
	{
		VERIFY(a>=min && a<=max);
		float q = (a-min)/(max-min);
		w_u8( u8(iFloor((q+EPS_L)*255.f)));
	}
	IC void w_angle16	( float a		)	{
		if (a<0)	a+=PI_MUL_2;
		else if (a>PI_MUL_2) a-=PI_MUL_2;
		w_float_q16	(a,0,PI_MUL_2);
	}
	IC void w_angle8	( float a		)	{
		if (a<0)	a+=PI_MUL_2;
		else if (a>PI_MUL_2) a-=PI_MUL_2;
		w_float_q8	(a,0,PI_MUL_2);
	}
	IC void w_dir		( const Fvector& D) {
		w_u16(pvCompress(D));
	}
	IC void w_sdir		( const Fvector& D) {
		Fvector C;
		float mag		= D.magnitude();
		if (mag>EPS_S)	{
			C.div		(D,mag);
		} else {
			C.set		(0,0,1);
			mag			= 0;
		}
		w_dir	(C);
		w_float (mag);
	}
	IC void w_string	( LPCSTR S )
	{
		w	(S,strlen(S)+1);
	}
	
	// reading
	IC void	r_begin	( u16& type		)
	{
		r_pos		= 0;
		type		= r_u16();
	}
	IC void r		( void* p, DWORD count)
	{
		VERIFY		(p && count);
		CopyMemory	(p,&B.data[r_pos],count);
		r_pos		+= count;
		VERIFY		(r_pos<=B.count);
	}
	IC BOOL r_eof	()
	{
		return r_pos>=B.count;
	}

	// reading - utilities
	IC Fvector	r_vec3			( )	{ Fvector a; r(&a,3*sizeof(float));	return a;	} // vec3
	IC Fvector4 r_vec4			( )	{ Fvector4 a;r(&a,4*sizeof(float));	return a;	} // vec4
	IC float	r_float			( )	{ float a; r(&a,4);	return a;	}			// float
	IC u64		r_u64			( )	{	u64 a; r(&a,8);	return a;	}			// qword (8b)
	IC s64		r_s64			( )	{	s64 a; r(&a,8); return a; 	}			// qword (8b)
	IC u32		r_u32			( )	{	u32 a; r(&a,4);	return a;	}			// dword (4b)
	IC s32		r_s32			( )	{	s32 a; r(&a,4);	return a; 	}			// dword (4b)
	IC u16		r_u16			( )	{	u16 a; r(&a,2);	return a; 	}			// word (2b)
	IC s16		r_s16			( )	{	s16 a; r(&a,2); return a; 	}			// word (2b)
	IC u8		r_u8			( )	{	 u8 a; r(&a,1); return a; 	}			// byte (1b)
	IC s8		r_s8			( )	{	 s8 a; r(&a,1); return a; 	}			// byte (1b)

	IC float	r_float_q16		( float min, float max)
	{
		u16		val = r_u16();
		float	v	= (float(val)/65535.f)*(max-min) + min;
		VERIFY	(v>=min && v<=max);
		return	v;
	}
	IC float	r_float_q8		( float min, float max)
	{
		u8		val = r_u8();
		float	v	= (float(val)/255.f)*(max-min) + min;
		VERIFY	(v>=min && v<=max);
		return	v;
	}
	IC float	r_angle16		( )	{ return r_float_q16(0,PI_MUL_2);	}
	IC float	r_angle8		( )	{ return r_float_q8	(0,PI_MUL_2);	}
	IC Fvector	r_dir			( ) { Fvector D; pvDecompress(D,r_u16()); return D; }

	IC Fvector	r_sdir			( )
	{
		Fvector	D;
		pvDecompress	(D,r_u16());
		D.mul			(r_float());
		return	D;
	}

	IC void		r_string		( LPSTR S )
	{
		LPCSTR	data	= LPCSTR(&B.data[r_pos]);
		int		len		= strlen(data);
		r		(S,len+1);
	}
};
