#ifndef __FRECT
#define __FRECT

typedef struct _frect {
public:
	union{
		struct{
			float 	x1, y1, x2, y2;
		};
		float		m[4];
	};

	__forceinline	void	set(float _x1, float _y1, float _x2, float _y2)	{ x1=_x1;	y1=_y1;		x2=_x2;		y2=_y2;		};
	__forceinline   void	set(_frect &r)								{ x1=r.x1;	y1=r.y1;	x2=r.x2;	y2=r.y2;	};
	__forceinline   void	null( )										{ x1=0;		y1=0;		x2=0;		y2=0;		};

	__forceinline   void	add(float x, float y)						{ x1+=x;	y1+=y;		x2+=x;		y2+=y;		};
	__forceinline   void	sub(float x, float y)						{ x1-=x;	y1-=y;		x2-=x;		y2-=y;		};
	__forceinline   void	mul(float x, float y)						{ x1*=x;	y1*=y;		x2*=x;		y2*=y;		};
	__forceinline   void	div(float x, float y)						{ x1/=x;	y1/=y;		x2/=x;		y2/=y;		};

	__forceinline   void	add(_frect r, float x, float y)				{ x1=r.x1+x;	y1=r.y1+y;	x2=r.x2+x;	y2=r.y2+y; };
	__forceinline   void	sub(_frect r, float x, float y)				{ x1=r.x1-x;	y1=r.y1-y;	x2=r.x2-x;	y2=r.y2-y; };
	__forceinline   void	mul(_frect r, float x, float y)				{ x1=r.x1*x;	y1=r.y1*y;	x2=r.x2*x;	y2=r.y2*y; };
	__forceinline   void	div(_frect r, float x, float y)				{ x1=r.x1/x;	y1=r.y1/y;	x2=r.x2/x;	y2=r.y2/y; };

	__forceinline   bool	in (float  x, float  y)						{ return (x>=x1) && (x<=x2) && (y>=y1) && (y<=y2);	};
	__forceinline   bool	in (_fpoint &p)								{ return (p.x>=x1) && (p.x<=x2) && (p.y>=y1) && (p.y<=y2);	};
	__forceinline   bool	cmp(_frect &r)								{ return x1==r.x1 && y1==r.y1 && x2==r.x2 && y2==r.y2; };
} Frect;


#endif
