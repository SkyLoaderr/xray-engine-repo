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
/*
	__forceinline	void	set(float _r, float _g, float _b, float _a)	{
		VERIFY(_r>=0 && _r<=1);
		VERIFY(_g>=0 && _g<=1);
		VERIFY(_b>=0 && _b<=1);
		VERIFY(_a>=0 && _a<=1);
		a=uchar(_a*255.0f);
		r=uchar(_r*255.0f);
		g=uchar(_g*255.0f);
		b=uchar(_b*255.0f);
	};
*/
	__forceinline	void	mul(float s) {
		VERIFY(s>=0 && s<=1);
		a = uchar(float(a)*s);
		r = uchar(float(r)*s);
		g = uchar(float(g)*s);
		b = uchar(float(b)*s);
	};
} FPcolor;

#endif
