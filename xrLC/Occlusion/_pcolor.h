#ifndef __PC__
#define __PC__

typedef struct _Pcolor {
public:
	union {
		struct {
			uchar b,g,r,a;
		};
		DWORD c;
	};
	__forceinline	void	set(DWORD color)							{
		c=color;
	};
	__forceinline	void	set(_Pcolor &color)							{
		c=color.c;
	};
	__forceinline	void	set(uchar _r, uchar _g, uchar _b, uchar _a)	{
		a=_a; r=_r; g=_g; b=_b;
	};
} FPcolor;

#endif
