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

	IC	void	set(float _x, float _y)	{ x=_x;	y=_y; };
	IC   void	set(_fpoint &p)			{ x=p.x;y=p.y;};
	IC   void	set(_ipoint &p);
	IC   void	null( )					{ x=0;	y=0;  };

	IC   void	add(float s)			{ x+=s;	y+=s; };
	IC   void	sub(float s)			{ x-=s;	y-=s; };
	IC   void	mul(float s)			{ x*=s;	y*=s; };
	IC   void	div(float s)			{ x/=s;	y/=s; };

	IC   void	add(float s1, float s2)	{ x+=s1; y+=s2; };
	IC   void	sub(float s1, float s2)	{ x-=s1; y-=s2; };
	IC   void	mul(float s1, float s2)	{ x*=s1; y*=s2; };
	IC   void	div(float s1, float s2)	{ x/=s1; y/=s2; };

	IC   BOOL	cmp(_fpoint &p)			{ return x==p.x && y==p.y; };
} Fpoint;

#endif
