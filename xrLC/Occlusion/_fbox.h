#ifndef __FBOX
#define __FBOX

typedef struct _fbox {
public:
	union{
		struct{
			_vector	min;
			_vector max;
		};
		struct{
			float x1, y1, z1;
			float x2, y2, z2;
		};
	};

	__forceinline	void	set(_vector &_min, _vector &_max)	{ min.set(_min);	max.set(_max);	};
	__forceinline   void	set(_fbox &b)						{ min.set(b.min);	max.set(b.max);	};
	__forceinline   void	null( )								{ min.set(0,0,0);	max.set(0,0,0);	};

	__forceinline   void	contract(float  s)					{ x1+=s;	y1+=s;	z1+=s;	x2-=s;	y2-=s;	z2-=s;	};
	__forceinline   void	expand(float  s)					{ x1-=s;	y1-=s;	z1-=s;	x2+=s;	y2+=s;	z2+=s;	};

	__forceinline   void	add(_vector &p)						{ min.add(p); max.add(p);				};
	__forceinline   void	add(_fbox &b, _vector &p)			{ min.add(b.min, p); max.add(b.max, p);	};

	__forceinline   bool	in (_vector &p)						{ return (p.x>=x1) && (p.x<=x2) && (p.y>=y1) && (p.y<=y2) && (p.z>=z1) && (p.z<=z2); };
	__forceinline   bool	in (float x, float y, float z)		{ return (x>=x1) && (x<=x2) && (y>=y1) && (y<=y2) && (z>=z1) && (z<=z2); };
	__forceinline   bool	cmp(_fbox &b)						{ return x1==b.x1 && y1==b.y1 && z1==b.z1 && x2==b.x2 && y2==b.y2 && z2==b.z2; };
	__forceinline   bool	overlap (_fbox &b)					{ return in(b.min) || in(b.max) || b.in(min) || b.in(max); };

	__forceinline	void	transform(_matrix &m)				{ min.transform_tiny(m); max.transform_tiny(m);	}
} Fbox;

#endif
