#ifndef _STD_EXT_internal
#define _STD_EXT_internal

// float
IC float	_abs	(float x)		{ return fabsf(x); }
IC float	_sqrt	(float x)		{ return sqrtf(x); }

// double
IC double	_abs	(double x)		{ return fabs(x); }
IC double	_sqrt	(double x)		{ return sqrt(x); }

// int8
IC s8		_abs	(s8 x)			{ return x & 0x7f; }
IC s8		_sqrt	(s8 x)			{ return s8(iFloor(_sqrt(float(x))));	}

// unsigned int8
IC u8		_abs	(u8 x)			{ return x; }
IC u8		_sqrt	(u8 x)			{ return u8(iFloor(_sqrt(float(x))));	}

// int16
IC s16		_abs	(s16 x)			{ return x & 0x7fff; }
IC s16		_sqrt	(s16 x)			{ return s16(iFloor(_sqrt(float(x))));	}

// unsigned int16
IC u16		_abs	(u16 x)			{ return x; }
IC u16		_sqrt	(u16 x)			{ return u16(iFloor(_sqrt(float(x))));  }

// int32
IC s32		_abs	(s32 x)			{ return x & 0x7fffffff; }
IC s32		_sqrt	(s32 x)			{ return s32(iFloor(_sqrt(float(x))));	}

// unsigned int32
IC u32		_abs	(u32 x)			{ return x; }
IC u32		_sqrt	(u32 x)			{ return u32(iFloor(_sqrt(float(x))));  }

// generic
template <class T>	IC T	_min	(T a, T b)	{ return a<b?a:b;	}
template <class T>	IC T	_max	(T a, T b)	{ return a>b?a:b;	}

#endif