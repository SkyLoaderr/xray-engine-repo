#ifndef __C__
#define __C__

typedef struct _color {
public:
	union{
  		struct{ float r,g,b,a; };
  		float val[4];
    };

	__inline	D3DCOLOR	*d3d(void) { return (D3DCOLOR *)this;	};
	__inline	void	set(DWORD dw)
	{
		const float f = 1.0f / 255.0f;
		a = f * (float) (uchar) (dw >> 24);
		r = f * (float) (uchar) (dw >> 16);
		g = f * (float) (uchar) (dw >>  8);
		b = f * (float) (uchar) (dw >>  0);
	};
	__inline	void	set(_Pcolor dw)
	{
		const float f = 1.0f / 255.0f;
		a = f * (float) dw.a;
		r = f * (float) dw.r;
		g = f * (float) dw.g;
		b = f * (float) dw.b;
	};
	__inline	void	set(float _r, float _g, float _b, float _a)
	{	r = _r; g = _g; b = _b; a = _a;		};
	__inline	void	set(_color dw)
	{	r=dw.r; g=dw.g; b=dw.b; a = dw.a;	};
	__inline	DWORD	get( )
	{
		uchar _a, _r, _g, _b;
		_a = (uchar)(a*255.f);
		_r = (uchar)(r*255.f);
		_g = (uchar)(g*255.f);
		_b = (uchar)(b*255.f);
		return ((DWORD)(_a<<24) | (_r<<16) | (_g<<8) | (_b));
	};
	__inline	DWORD	get_ideal( )
	{
		float max, dif;

		max = a;
		if (r>max) max=r;
		if (g>max) max=g;
		if (b>max) max=b;
		dif = 255.f/max;
#define CNV(x) DWORD(uchar((x<0?0:x)*dif))
		return ((DWORD)(CNV(a)<<24) | (CNV(r)<<16) | (CNV(g)<<8) | (CNV(b)));
#undef CNV
	};
	__inline	DWORD	get_scale( )
	{
		uchar _a, _r, _g, _b;
		float min, max, dif;

		min = max = a;
		if (r<min) min=r; if (r>max) max=r;
		if (g<min) min=g; if (g>max) max=g;
		if (b<min) min=b; if (b>max) max=b;
		dif = 255.f/(max-min);

		_a = (uchar)((a+min)*dif);
		_r = (uchar)((r+min)*dif);
		_g = (uchar)((g+min)*dif);
		_b = (uchar)((b+min)*dif);

		return ((DWORD)(_a<<24) | (_r<<16) | (_g<<8) | (_b));
	};
	__inline	DWORD	get_maximized( )
	{
		_color t;
		float max = a;
		if (r>max) max=r;
		if (g>max) max=g;
		if (b>max) max=b;
		float dif = 1.0f-max;
		t.set(r+dif,g+dif,b+dif,a+dif);
		return t.get();
	};
	__inline	void	adjust_contrast(float f)				// >1 - contrast will be increased
	{
		r = 0.5f + f * (r - 0.5f);
		g = 0.5f + f * (g - 0.5f);
		b = 0.5f + f * (b - 0.5f);
	};
	__inline	void	adjust_contrast(_color &in, float f)	// >1 - contrast will be increased
	{
		r = 0.5f + f * (in.r - 0.5f);
		g = 0.5f + f * (in.g - 0.5f);
		b = 0.5f + f * (in.b - 0.5f);
	};
	__inline	void	adjust_saturation(float s)
	{
		// Approximate values for each component's contribution to luminance.
		// Based upon the NTSC standard described in ITU-R Recommendation BT.709.
		float grey = r * 0.2125f + g * 0.7154f + b * 0.0721f;

		r = grey + s * (r - grey);
		g = grey + s * (g - grey);
		b = grey + s * (b - grey);
	};
	__inline	void	adjust_saturation(_color &in, float s)
	{
		// Approximate values for each component's contribution to luminance.
		// Based upon the NTSC standard described in ITU-R Recommendation BT.709.
		float grey = in.r * 0.2125f + in.g * 0.7154f + in.b * 0.0721f;

		r = grey + s * (in.r - grey);
		g = grey + s * (in.g - grey);
		b = grey + s * (in.b - grey);
	};
	__inline	void	adjust_hue(float s)
	{
// AlexMX
/*		float grey = r2 * 0.2125f + g2 * 0.7154f + b2 * 0.0721f;

		r1= (r2 - grey + s*grey)/s;
		g1= (g2 - grey + s*grey)/s;
		b1= (b2 - grey + s*grey)/s;
		s*grey = (r2 - grey + s*grey) * 0.2125f + (g2 - grey + s*grey) * 0.7154f +  (b2 - grey + s*grey) * 0.0721f;
		s*grey-s*grey*0.2125f = (r2-grey)*0.2125f
*/	};
	__inline	void	modulate(_color &in)
	{
		r*=in.r;
		g*=in.g;
		b*=in.b;
		a*=in.a;
	};
	__inline	void	modulate(_color &in1, _color &in2)
	{
		r=in1.r*in2.r;
		g=in1.g*in2.g;
		b=in1.b*in2.b;
		a=in1.a*in2.a;
	};
	__inline	void	negative(_color &in)
	{
		r=1.0f-in.r;
		g=1.0f-in.g;
		b=1.0f-in.b;
		a=1.0f-in.a;
	};
	__inline	void	negative(void)
	{
		r=1.0f-r;
		g=1.0f-g;
		b=1.0f-b;
		a=1.0f-a;
	};
	__inline	void	sub(float s)
	{
		r-=s;
		g-=s;
		b-=s;
//		a=1.0f-a;
	};
	__inline	void	add(float s)
	{
		r+=s;
		g+=s;
		b+=s;
//		a=1.0f-a;
	};
	__inline	void	mul(float s) {
		r*=s;
		g*=s;
		b*=s;
//		a*=s;
	};
	__inline	void	mul(_color &c, float s) {
		r=c.r*s;
		g=c.g*s;
		b=c.b*s;
//		a=c.a*s;
	};
/*
	__inline	FPcolor	pack(void) {
	};
*/
} Fcolor;

#endif
