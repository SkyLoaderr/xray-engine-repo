#ifndef __C__
#define __C__

typedef struct _color {
public:
	float r,g,b,a;

	IC	D3DCOLOR	*d3d(void) { return (D3DCOLOR *)this;	};
    IC 	float*	asDATA(void) { return (float*)this; };
	IC	void	set(DWORD dw)
	{
		const float f = 1.0f / 255.0f;
		a = f * (float) BYTE(dw >> 24);
		r = f * (float) BYTE(dw >> 16);
		g = f * (float) BYTE(dw >>  8);
		b = f * (float) BYTE(dw >>  0);
	};
	IC	void	set(float _r, float _g, float _b, float _a)
	{	r = _r; g = _g; b = _b; a = _a;		};
	IC	void	set(_color dw)
	{	r=dw.r; g=dw.g; b=dw.b; a = dw.a;	};
	IC	DWORD	get( ) const
	{
		DWORD _r = iFloor(r*255.f); clamp(_r,0ul,255ul);
		DWORD _g = iFloor(g*255.f); clamp(_g,0ul,255ul);
		DWORD _b = iFloor(b*255.f); clamp(_b,0ul,255ul);
		DWORD _a = iFloor(a*255.f); clamp(_a,0ul,255ul);
		return D3DCOLOR_RGBA(_r,_g,_b,_a);
	};
	IC	DWORD	get_windows( ) const
	{
		BYTE _a, _r, _g, _b;
		_a = (BYTE)(a*255.f);
		_r = (BYTE)(r*255.f);
		_g = (BYTE)(g*255.f);
		_b = (BYTE)(b*255.f);
		return ((DWORD)(_a<<24) | (_b<<16) | (_g<<8) | (_r));
	};
	IC	void	set_windows(DWORD dw)
	{
		const float f = 1.0f / 255.0f;
		a = f * (float) (BYTE) (dw >> 24);
		b = f * (float) (BYTE) (dw >> 16);
		g = f * (float) (BYTE) (dw >>  8);
		r = f * (float) (BYTE) (dw >>  0);
	};
	IC	DWORD	get_ideal( ) const
	{
		float max, dif;

		max = a;
		if (r>max) max=r;
		if (g>max) max=g;
		if (b>max) max=b;
		dif = 255.f/max;
#define CNV(x) DWORD(BYTE((x<0?0:x)*dif))
		return ((DWORD)(CNV(a)<<24) | (CNV(r)<<16) | (CNV(g)<<8) | (CNV(b)));
#undef CNV
	};
	IC	DWORD	get_scale( ) const
	{
		BYTE _a, _r, _g, _b;
		float min, max, dif;

		min = max = a;
		if (r<min) min=r; if (r>max) max=r;
		if (g<min) min=g; if (g>max) max=g;
		if (b<min) min=b; if (b>max) max=b;
		dif = 255.f/(max-min);

		_a = (BYTE)((a+min)*dif);
		_r = (BYTE)((r+min)*dif);
		_g = (BYTE)((g+min)*dif);
		_b = (BYTE)((b+min)*dif);

		return ((DWORD)(_a<<24) | (_r<<16) | (_g<<8) | (_b));
	};
	IC	DWORD	get_maximized( ) const
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
	IC	void	adjust_hue(float s)
	{
// AlexMX
/*		float grey = r2 * 0.2125f + g2 * 0.7154f + b2 * 0.0721f;

		r1= (r2 - grey + s*grey)/s;
		g1= (g2 - grey + s*grey)/s;
		b1= (b2 - grey + s*grey)/s;
		s*grey = (r2 - grey + s*grey) * 0.2125f + (g2 - grey + s*grey) * 0.7154f +  (b2 - grey + s*grey) * 0.0721f;
		s*grey-s*grey*0.2125f = (r2-grey)*0.2125f
*/	};
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
} Fcolor;

#endif
