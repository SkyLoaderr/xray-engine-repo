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

	IC	POINT	*d3d(void)				{ return (POINT*)this;	};
	IC	void	set(int  _x, int  _y)	{ x=_x;	y=_y; };
	IC   void	set(_ipoint &p)			{ x=p.x;y=p.y;};
	IC   void	set(_fpoint &p);
	IC   void	null( )					{ x=0;	y=0;  };

	IC   void	add(int  s)				{ x+=s;	y+=s; };
	IC   void	sub(int  s)				{ x-=s;	y-=s; };
	IC   void	mul(int  s)				{ x*=s;	y*=s; };
	IC   void	div(int  s)				{ x/=s;	y/=s; };

	IC   void	add(int   s1, int   s2)	{ x+=s1; y+=s2; };
	IC   void	sub(int   s1, int   s2)	{ x-=s1; y-=s2; };
	IC   void	mul(int   s1, int   s2)	{ x*=s1; y*=s2; };
	IC   void	div(int   s1, int   s2)	{ x/=s1; y/=s2; };

	IC   BOOL	cmp(_ipoint &p)			{ return x==p.x && y==p.y; };
	IC	BOOL	operator == (_ipoint &a )
	{
		return (a.x==x)&&(a.y==y);
	};
} Ipoint;

#endif
