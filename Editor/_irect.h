#ifndef __IRECT
#define __IRECT


typedef struct _irect {
public:
	union{
		struct{
			int 	x1, y1, x2, y2;
		};
		int 		m[4];
	};

	__forceinline	RECT*	get_LPRECT()								{ return LPRECT(&m); };

	__forceinline	void	set(int  _x1, int  _y1, int  _x2, int  _y2)	{ x1=_x1;	y1=_y1;		x2=_x2;		y2=_y2;		};
	__forceinline   void	set(_irect &r)								{ x1=r.x1;	y1=r.y1;	x2=r.x2;	y2=r.y2;	};
	__forceinline   void	null( )										{ x1=0;		y1=0;		x2=0;		y2=0;		};

	__forceinline   void	add(int x, int y)							{ x1+=x;	y1+=y;		x2+=x;		y2+=y;		};
	__forceinline   void	sub(int x, int y)							{ x1-=x;	y1-=y;		x2-=x;		y2-=y;		};
	__forceinline   void	mul(int x, int y)							{ x1*=x;	y1*=y;		x2*=x;		y2*=y;		};
	__forceinline   void	div(int x, int y)							{ x1/=x;	y1/=y;		x2/=x;		y2/=y;		};

	__forceinline   void	add(_irect &r, int x, int y)				{ x1=r.x1+x;	y1=r.y1+y;	x2=r.x2+x;	y2=r.y2+y; };
	__forceinline   void	sub(_irect &r, int x, int y)				{ x1=r.x1-x;	y1=r.y1-y;	x2=r.x2-x;	y2=r.y2-y; };
	__forceinline   void	mul(_irect &r, int x, int y)				{ x1=r.x1*x;	y1=r.y1*y;	x2=r.x2*x;	y2=r.y2*y; };
	__forceinline   void	div(_irect &r, int x, int y)				{ x1=r.x1/x;	y1=r.y1/y;	x2=r.x2/x;	y2=r.y2/y; };

	__forceinline   BOOL	in (int  x, int  y)							{ return (x>=x1) && (x<=x2) && (y>=y1) && (y<=y2);	};
	__forceinline   BOOL	cmp(_irect &r)								{ return x1==r.x1 && y1==r.y1 && x2==r.x2 && y2==r.y2; };
} Irect;

#endif
