#ifndef __C__
#define __C__

typedef struct _color {
public:
	float r,g,b,a;

	IC	D3DCOLOR	*d3d(void) { return (D3DCOLOR *)this;	};
    IC 	float*	asDATA(void) { return (float*)this; };
	IC	void	set(u32 dw)
	{
		const float f = 1.0f / 255.0f;
		a = f * float((dw >> 24)& 0xff);
		r = f * float((dw >> 16)& 0xff);
		g = f * float((dw >>  8)& 0xff);
		b = f * float((dw >>  0)& 0xff);
	};
	IC	void	set(float _r, float _g, float _b, float _a)
	{	r = _r; g = _g; b = _b; a = _a;		};
	IC	void	set(_color dw)
	{	r=dw.r; g=dw.g; b=dw.b; a = dw.a;	};
	IC	u32	get( ) const
	{
		int	 _r = iFloor(r*255.f); clamp(_r,0,255);
		int	 _g = iFloor(g*255.f); clamp(_g,0,255);
		int	 _b = iFloor(b*255.f); clamp(_b,0,255);
		int	 _a = iFloor(a*255.f); clamp(_a,0,255);
		return D3DCOLOR_RGBA(_r,_g,_b,_a);
	};
	IC	u32	get_windows( ) const
	{
		BYTE _a, _r, _g, _b;
		_a = (BYTE)(a*255.f);
		_r = (BYTE)(r*255.f);
		_g = (BYTE)(g*255.f);
		_b = (BYTE)(b*255.f);
		return ((u32)(_a<<24) | (_b<<16) | (_g<<8) | (_r));
	};
	IC	void	set_windows(u32 dw)
	{
		const float f = 1.0f / 255.0f;
		a = f * (float) (BYTE) (dw >> 24);
		b = f * (float) (BYTE) (dw >> 16);
		g = f * (float) (BYTE) (dw >>  8);
		r = f * (float) (BYTE) (dw >>  0);
	};
	IC	void	adjust_contrast(float f)				// >1 - contrast will be increased
	{
		r = 0.5f + f * (r - 0.5f);
		g = 0.5f + f * (g - 0.5f);
		b = 0.5f + f * (b - 0.5f);
	};
	IC	void	adjust_contrast(_color &in, float f)	// >1 - contrast will be increased
	{
		r = 0.5f + f * (in.r - 0.5f);
		g = 0.5f + f * (in.g - 0.5f);
		b = 0.5f + f * (in.b - 0.5f);
	};
	IC	void	adjust_saturation(float s)
	{
		// Approximate values for each component's contribution to luminance.
		// Based upon the NTSC standard described in ITU-R Recommendation BT.709.
		float grey = r * 0.2125f + g * 0.7154f + b * 0.0721f;

		r = grey + s * (r - grey);
		g = grey + s * (g - grey);
		b = grey + s * (b - grey);
	};
	IC	void	adjust_saturation(_color &in, float s)
	{
		// Approximate values for each component's contribution to luminance.
		// Based upon the NTSC standard described in ITU-R Recommendation BT.709.
		float grey = in.r * 0.2125f + in.g * 0.7154f + in.b * 0.0721f;

		r = grey + s * (in.r - grey);
		g = grey + s * (in.g - grey);
		b = grey + s * (in.b - grey);
	};
	IC	void	modulate(_color &in)
	{
		r*=in.r;
		g*=in.g;
		b*=in.b;
		a*=in.a;
	};
	IC	void	modulate(_color &in1, _color &in2)
	{
		r=in1.r*in2.r;
		g=in1.g*in2.g;
		b=in1.b*in2.b;
		a=in1.a*in2.a;
	};
	IC	void	negative(_color &in)
	{
		r=1.0f-in.r;
		g=1.0f-in.g;
		b=1.0f-in.b;
		a=1.0f-in.a;
	};
	IC	void	negative(void)
	{
		r=1.0f-r;
		g=1.0f-g;
		b=1.0f-b;
		a=1.0f-a;
	};
	IC	void	sub(float s)
	{
		r-=s;
		g-=s;
		b-=s;
//		a=1.0f-a;
	};
	IC	void	add_rgb(float s)
	{
		r+=s;
		g+=s;
		b+=s;
	};
	IC	void	add_rgba(float s)
	{
		r+=s;
		g+=s;
		b+=s;
		a+=s;
	};
	IC	void	mul_rgba(float s) {
		r*=s;
		g*=s;
		b*=s;
		a*=s;
	};
	IC	void	mul_rgb(float s) {
		r*=s;
		g*=s;
		b*=s;
	};
	IC	void	mul_rgba(_color &c, float s) {
		r=c.r*s;
		g=c.g*s;
		b=c.b*s;
		a=c.a*s;
	};
	IC	void	mul_rgb(_color &c, float s) {
		r=c.r*s;
		g=c.g*s;
		b=c.b*s;
	};
	
	// SQ magnitude
	IC	float	magnitude_sqr_rgb(void) {
		return r*r + g*g + b*b;
	}
	// magnitude
	IC	float	magnitude_rgb(void) {
		return sqrtf(magnitude_sqr_rgb());
	}

	// Normalize
	IC	void	normalize_rgb(void) {
		VERIFY(magnitude_sqr_rgb()>EPS_S);
		mul_rgb(1.f/magnitude_rgb());
	}
	IC	void	normalize_rgb(_color &c) {
		VERIFY(c.magnitude_sqr_rgb()>EPS_S);
		mul_rgb(c,1.f/c.magnitude_rgb());
	}
	IC	void	lerp(const _color& c1, const _color& c2, float t){
		float invt = 1.f-t;
		r = c1.r*invt + c2.r*t;
		g = c1.g*invt + c2.g*t;
		b = c1.b*invt + c2.b*t;
		a = c1.a*invt + c2.a*t;
	}
	IC	void	lerp(const _color& c1, const _color& c2, const _color& c3, float t){
		if (t>.5f){
			lerp(c2,c3,t*2.f-1.f);
		}else{
			lerp(c1,c2,t*2.f);
		}
	}
	IC  BOOL	similar_rgba(const _color &v, float E=EPS_L) 	const	{ return _abs(r-v.r)<E && _abs(g-v.g)<E && _abs(b-v.b)<E && _abs(a-v.a)<E;};
	IC  BOOL	similar_rgb	(const _color &v, float E=EPS_L) 	const	{ return _abs(r-v.r)<E && _abs(g-v.g)<E && _abs(b-v.b)<E;};
} Fcolor;

#endif
