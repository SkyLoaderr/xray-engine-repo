#ifndef __FBOX2
#define __FBOX2

typedef struct _fbox2 {
public:
	union{
		struct{
			Fvector2 min;
			Fvector2 max;
		};
		struct{
			float x1, y1;
			float x2, y2;
		};
	};

	IC 	void	set(const Fvector2 &_min, const Fvector2 &_max){ min.set(_min);	max.set(_max);	};
	IC	void	set(float x1, float y1, float x2, float y2){ min.set(x1,y1);	max.set(x2,y2);};
	IC	void	set(const _fbox2 &b)				{ min.set(b.min);	max.set(b.max);	};
	IC	void	null( )								{ min.set(0.f,0.f);	max.set(0.f,0.f); };

	IC	void	shrink(float s)						{ min.add(s); max.sub(s); };
	IC	void	shrink(const Fvector2& s)			{ min.add(s); max.sub(s); };
	IC	void	grow(float s)						{ min.sub(s); max.add(s); };
	IC	void	grow(const Fvector2& s)				{ min.sub(s); max.add(s); };

	IC	void	add		(const Fvector2 &p)			{ min.add(p); max.add(p); };
	IC	void	offset	(const Fvector2 &p)			{ min.add(p); max.add(p); };
	IC	void	add(const _fbox2 &b, const Fvector2 &p){ min.add(b.min, p); max.add(b.max, p);	};

	IC	BOOL	contains(float x, float y)			{ return (x>=x1) && (x<=x2) && (y>=y1) && (y<=y2); };
	IC	BOOL	contains(const Fvector2 &p)			{ return contains(p.x,p.y);	};
	IC	BOOL	contains(const _fbox2 &b)			{ return contains(b.min) && contains(b.max); };

	IC	BOOL	similar(const _fbox2 &b)			{ return min.similar(b.min) && max.similar(b.max); };

	IC	void	invalidate	()						{ min.set(flt_max,flt_max); max.set(flt_min,flt_min); }
	IC	void	modify		(const Fvector2 &p)		{ min.min(p); max.max(p);	}
	IC	void	merge		(const _fbox2 &b)		{ modify(b.min); modify(b.max); };
	IC	void	merge		(const _fbox2 &b1, const _fbox2 &b2) { invalidate(); merge(b1); merge(b2); }

	IC	void	getsize		(Fvector2& R )	const 	{ R.sub( max, min ); };
	IC	void	getradius	(Fvector2& R )	const 	{ getsize(R); R.mul(0.5f); };
	IC	float	getradius	( )				const 	{ Fvector2 R; getsize(R); R.mul(0.5f); return R.magnitude(); };

	IC	void	getcenter	(Fvector2& C )	const 	{
		C.x = (min.x + max.x) * 0.5f;
		C.y = (min.y + max.y) * 0.5f;
	};
	IC	void	getsphere	(Fvector2 &C, float &R) const {
		getcenter(C);
		R = C.distance_to(max);
	};

	// Detects if this box intersect other
	IC	BOOL	intersect(const _fbox2& box )
	{
		if( max.x < box.min.x )	return FALSE;
		if( max.y < box.min.y )	return FALSE;
		if( min.x > box.max.x )	return FALSE;
		if( min.y > box.max.y )	return FALSE;
		return TRUE;
	};

	// Make's this box valid AABB
    IC void sort(){
    	float tmp;
		if( min.x > max.x ) { tmp = min.x; min.x = max.x; max.x = tmp; }
		if( min.y > max.y ) { tmp = min.y; min.y = max.y; max.y = tmp; }
	};

	// Does the vector intersects box
	IC BOOL Pick( const Fvector2& start, const Fvector2& dir ){
		float	alpha,xt,yt;
		Fvector2 rvmin,rvmax;

		rvmin.sub( min, start );
		rvmax.sub( max, start );

		if( !fis_zero(dir.x) ){
			alpha = rvmin.x / dir.x;
			yt = alpha * dir.y;
			if( yt >= rvmin.y && yt <= rvmax.y )
				return true;
			alpha = rvmax.x / dir.x;
			yt = alpha * dir.y;
			if( yt >= rvmin.y && yt <= rvmax.y )
				return true;
		}

		if( !fis_zero(dir.y) ){
			alpha = rvmin.y / dir.y;
			xt = alpha * dir.x;
			if( xt >= rvmin.x && xt <= rvmax.x )
				return true;
			alpha = rvmax.y / dir.y;
			xt = alpha * dir.x;
			if( xt >= rvmin.x && xt <= rvmax.x )
				return true;
		}
		return false;
	};

	IC DWORD& IR(float &x) { return (DWORD&)x; }
	IC BOOL Pick2(const Fvector2& origin, const Fvector2& dir, Fvector2& coord){
		BOOL Inside = TRUE;
		Fvector2	MaxT;
		MaxT.x=MaxT.y=-1.0f;
		
		// Find candidate planes.
		{
			if(origin[0] < min[0]) {
				coord[0]	= min[0];
				Inside		= FALSE;
				if(IR(dir[0]))	MaxT[0] = (min[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
			} else if(origin[0] > max[0]) {
				coord[0]	= max[0];
				Inside		= FALSE;
				if(IR(dir[0]))	MaxT[0] = (max[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
			}
		}
		{
			if(origin[1] < min[1]) {
				coord[1]	= min[1];
				Inside		= FALSE;
				if(IR(dir[1]))	MaxT[1] = (min[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
			} else if(origin[1] > max[1]) {
				coord[1]	= max[1];
				Inside		= FALSE;
				if(IR(dir[1]))	MaxT[1] = (max[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
			}
		}
		
		// Ray origin inside bounding box
		if(Inside)
		{
			coord = origin;
			return true;
		}
		
		// Get largest of the maxT's for final choice of intersection
		DWORD WhichPlane = 0;
		if(MaxT[1] > MaxT[0])			WhichPlane = 1;
		
		// Check final candidate actually inside box
		if(IR(MaxT[WhichPlane])&0x80000000) return false;
		
		switch (WhichPlane) {
			case 0:
				// 1
				coord[1] = origin[1] + MaxT[0] * dir[1];
				if((coord[1] < min[1]) || (coord[1] > max[1]))	return false;
				return true;
			case 1:
				// 0
				coord[0] = origin[0] + MaxT[1] * dir[0];
				if((coord[0] < min[0]) || (coord[0] > max[0]))	return false;
				return true;
			default: 
				NODEFAULT;
				return false;
		}
	}
	
	IC void getpoint( int index,  Fvector2& result ){
		switch( index ){
		case 0: result.set( min.x, min.y ); break;
		case 1: result.set( min.x, min.y ); break;
		case 2: result.set( max.x, min.y ); break;
		case 3: result.set( max.x, min.y ); break;
		default: result.set(0.f,0.f); break; }
	};
	IC void getpoints(Fvector2* result){
		result[0].set( min.x, min.y );
		result[1].set( min.x, min.y );
		result[2].set( max.x, min.y );
		result[3].set( max.x, min.y );
	};

} Fbox2;

#endif
