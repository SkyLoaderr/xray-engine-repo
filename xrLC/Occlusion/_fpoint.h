#ifndef __FPOINT
#define __FPOINT

typedef struct _fpoint {
public:
	union{
		struct{
			float 	x, y;
		};
		float		f[2];
	};

	__forceinline	void	set(float _x, float _y)	{ x=_x;	y=_y; };
	__forceinline   void	set(_fpoint &p)			{ x=p.x;y=p.y;};
	__forceinline   void	set(_ipoint &p);
	__forceinline   void	null( )					{ x=0;	y=0;  };

	__forceinline   void	add(float s)			{ x+=s;	y+=s; };
	__forceinline   void	sub(float s)			{ x-=s;	y-=s; };
	__forceinline   void	mul(float s)			{ x*=s;	y*=s; };
	__forceinline   void	div(float s)			{ x/=s;	y/=s; };

	__forceinline   void	add(float s1, float s2)	{ x+=s1; y+=s2; };
	__forceinline   void	sub(float s1, float s2)	{ x-=s1; y-=s2; };
	__forceinline   void	mul(float s1, float s2)	{ x*=s1; y*=s2; };
	__forceinline   void	div(float s1, float s2)	{ x/=s1; y/=s2; };

	__forceinline   bool	cmp(_fpoint &p)			{ return x==p.x && y==p.y; };
} Fpoint;

#endif
