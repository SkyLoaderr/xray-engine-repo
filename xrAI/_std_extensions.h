#ifndef _STD_EXT_internal
#define _STD_EXT_internal

#ifdef abs
#undef abs
#endif

#ifdef _MIN
#undef _MIN
#undef _MAX
#undef min
#undef max
#endif

// token type definition
struct XRCORE_API xr_token
{
	LPCSTR	name;
	int 	id;
};

// generic
template <class T>	IC T		_min	(T a, T b)	{ return a<b?a:b;	}
template <class T>	IC T		_max	(T a, T b)	{ return a>b?a:b;	}
template <class T>	IC T		_sqr	(T a)		{ return a*a;		}

// float
IC float	_abs	(float x)		{ return fabsf(x); }
IC float	_sqrt	(float x)		{ return sqrtf(x); }
IC float	_sin	(float x)		{ return sinf(x); }
IC float	_cos	(float x)		{ return cosf(x); }
IC BOOL		_valid	(const float x)
{
	// check for: Signaling NaN, Quiet NaN, Negative infinity ( �INF), Positive infinity (+INF), Negative denormalized, Positive denormalized
	int			cls			= _fpclass		(double(x));
	if (cls&(_FPCLASS_SNAN+_FPCLASS_QNAN+_FPCLASS_NINF+_FPCLASS_PINF+_FPCLASS_ND+_FPCLASS_PD))	
		return FALSE;	

	/*	*****other cases are*****
	_FPCLASS_NN Negative normalized non-zero 
	_FPCLASS_NZ Negative zero ( � 0) 
	_FPCLASS_PZ Positive 0 (+0) 
	_FPCLASS_PN Positive normalized non-zero 
	*/
	return		TRUE;
}


// double
IC double	_abs	(double x)		{ return fabs(x); }
IC double	_sqrt	(double x)		{ return sqrt(x); }
IC double	_sin	(double x)		{ return sin(x); }
IC double	_cos	(double x)		{ return cos(x); }
IC BOOL		_valid	(const double x)
{
	// check for: Signaling NaN, Quiet NaN, Negative infinity ( �INF), Positive infinity (+INF), Negative denormalized, Positive denormalized
	int			cls			= _fpclass		(x);
	if (cls&(_FPCLASS_SNAN+_FPCLASS_QNAN+_FPCLASS_NINF+_FPCLASS_PINF+_FPCLASS_ND+_FPCLASS_PD))	
		return FALSE;	

	/*	*****other cases are*****
	_FPCLASS_NN Negative normalized non-zero 
	_FPCLASS_NZ Negative zero ( � 0) 
	_FPCLASS_PZ Positive 0 (+0) 
	_FPCLASS_PN Positive normalized non-zero 
	*/
	return		TRUE;
}

// int8
IC s8		_abs	(s8  x)			{ return (x>=0)? x : s8(-x); }
IC s8 		_min	(s8  x, s8  y)	{ return y + ((x - y) & ((x - y) >> (sizeof(s8 ) * 8 - 1))); };
IC s8 		_max	(s8  x, s8  y)	{ return x - ((x - y) & ((x - y) >> (sizeof(s8 ) * 8 - 1))); };

// unsigned int8
IC u8		_abs	(u8 x)			{ return x; }

// int16
IC s16		_abs	(s16 x)			{ return (x>=0)? x : s16(-x); }
IC s16		_min	(s16 x, s16 y)	{ return y + ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1))); };
IC s16		_max	(s16 x, s16 y)	{ return x - ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1))); };

// unsigned int16
IC u16		_abs	(u16 x)			{ return x; }

// int32
IC s32		_abs	(s32 x)			{ return (x>=0)? x : s32(-x); }
IC s32		_min	(s32 x, s32 y)	{ return y + ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1))); };
IC s32		_max	(s32 x, s32 y)	{ return x - ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1))); };

// string management
IC char*						strconcat				( char* dest, const char* S1, const char* S2)
{	return strcat(strcpy(dest,S1),S2); }

// dest = S1+S2+S3
IC char*						strconcat				( char* dest, const char* S1, const char* S2, const char* S3)
{	return strcat(strcat(strcpy(dest,S1),S2),S3); }

// dest = S1+S2+S3+S4
IC char*						strconcat				( char* dest, const char* S1, const char* S2, const char* S3, const char* S4)
{	return strcat(strcat(strcat(strcpy(dest,S1),S2),S3),S4); }

// dest = S1+S2+S3+S4+S5
IC char*						strconcat				( char* dest, const char* S1, const char* S2, const char* S3, const char* S4, const char* S5)
{	return strcat(strcat(strcat(strcat(strcpy(dest,S1),S2),S3),S4),S5); }

// dest = S1+S2+S3+S4+S5+S6
IC char*						strconcat				( char* dest, const char* S1, const char* S2, const char* S3, const char* S4, const char* S5, const char* S6)
{	return strcat(strcat(strcat(strcat(strcat(strcpy(dest,S1),S2),S3),S4),S5),S6); }

// return pointer to ".ext"
IC char*						strext					( const char* S )
{	return (char*) strchr(S,'.');	}

IC u32							xr_strlen				( const char* S )
{	return (u32)strlen(S);			}
IC int							xr_strcmp				( const char* S1, const char* S2 )
{	return (int)strcmp(S);			}

IC char*						timestamp				(string64& dest)
{
	string64	temp;

	/* Set time zone from TZ environment variable. If TZ is not set,
	* the operating system is queried to obtain the default value 
	* for the variable. 
	*/
	_tzset		();
	u32			it;

	// date
	_strdate	( temp );
	for (it=0; it<xr_strlen(temp); it++)
		if ('/'==temp[it]) temp[it]='-';
	strconcat	( dest, temp, "_" );

	// time
	_strtime	( temp );
	for (it=0; it<xr_strlen(temp); it++)
		if (':'==temp[it]) temp[it]='-';
	strcat		( dest, temp);
    return dest;
}


#endif
