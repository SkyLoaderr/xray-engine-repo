#ifndef __IPOINT
#define __IPOINT

typedef struct _ipoint {
public:
	union{
		struct{
			int 	x, y;
		};
		int 		f[2];
	};

	__forceinline	POINT	*d3d(void)				{ return (POINT*)this;	};
	__forceinline	void	set(int  _x, int  _y)	{ x=_x;	y=_y; };
	__forceinline   void	set(_ipoint &p)			{ x=p.x;y=p.y;};
	__forceinline   void	set(_fpoint &p);
	__forceinline   void	null( )					{ x=0;	y=0;  };

	__forceinline   void	add(int  s)				{ x+=s;	y+=s; };
	__forceinline   void	sub(int  s)				{ x-=s;	y-=s; };
	__forceinline   void	mul(int  s)				{ x*=s;	y*=s; };
	__forceinline   void	div(int  s)				{ x/=s;	y/=s; };

	__forceinline   void	add(int   s1, int   s2)	{ x+=s1; y+=s2; };
	__forceinline   void	sub(int   s1, int   s2)	{ x-=s1; y-=s2; };
	__forceinline   void	mul(int   s1, int   s2)	{ x*=s1; y*=s2; };
	__forceinline   void	div(int   s1, int   s2)	{ x/=s1; y/=s2; };

	__forceinline   BOOL	cmp(_ipoint &p)			{ return x==p.x && y==p.y; };
	__forceinline	BOOL	operator == (_ipoint &a )
	{
		return (a.x==x)&&(a.y==y);
	};
} Ipoint;

#endif
